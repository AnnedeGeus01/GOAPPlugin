#include "GOAPAgent.h"
#include "Actions/GOAPAction.h"
#include "Actions/PatrolAction.h"
#include "AIController.h"
#include "ProfilingDebugging/ScopedTimers.h"
#include "ProfilingDebugging/CsvProfiler.h"
#include "Stats/Stats.h"

// Set up for categorizing debug information
#define GOAP_LOG(Agent, Level, Format, ...) \
    if (Agent && (Agent->DebugLevel >= Level)) \
    { \
        UE_LOG(LogTemp, Warning, TEXT(Format), ##__VA_ARGS__); \
    }

// Stat group "GOAP"
DECLARE_STATS_GROUP(TEXT("GOAP"), STATGROUP_GOAP, STATCAT_Advanced);

// Individual stats
DECLARE_CYCLE_STAT(TEXT("GOAP Planner Tick"), STAT_GOAPPlannerTick, STATGROUP_GOAP);
DECLARE_CYCLE_STAT(TEXT("GOAP Execute Goal"), STAT_GOAPExecureGoal, STATGROUP_GOAP);


AGOAPAgent::AGOAPAgent()
{
    // Attach the World State component
    WorldState = CreateDefaultSubobject<UGOAPWorldStateComponent>(TEXT("WorldState"));
}

void AGOAPAgent::BeginPlay()
{
    Super::BeginPlay();

    if (!WorldState) return;

    WorldState->OwningAgent = this;

    Planner = NewObject<UGOAPPlanner>(this);
    
    GOAP_LOG(this, EGOAPDebugLevel::Minimal, "Agent world state: %s", *WorldState->GetStateAsString());

    AvailableActions.Empty();

    // Spawn actions from the class array
    for (TSubclassOf<UGOAPAction> ActionClass : ActionClasses)
    {
        if (!ActionClass) continue;

        UGOAPAction* NewAction = NewObject<UGOAPAction>(this, ActionClass);
        AvailableActions.Add(NewAction);
    }

    GOAP_LOG(this, EGOAPDebugLevel::Minimal, "Available actions at runtime: %d", AvailableActions.Num());

    // Spawn goals from the class array
    for (TSubclassOf<UGOAPGoal> GoalClass : GoalClasses)
    {
        if (!GoalClass) continue;

        UGOAPGoal* NewGoal = NewObject<UGOAPGoal>(this, GoalClass);
        AvailableGoals.Add(NewGoal);
    }

    GOAP_LOG(this, EGOAPDebugLevel::Minimal, "Available goals at runtime: %d", AvailableGoals.Num());
    for (UGOAPGoal* Goal : AvailableGoals)
    {
        if (Goal)
        {
            GOAP_LOG(this, EGOAPDebugLevel::Detailed, "Goal: %s", *Goal->GetGoalName());
        }
    }

    RequestReplan();

    if (WorldState)
    {
        WorldState->OnWorldStateChanged.AddDynamic(this, &AGOAPAgent::RequestReplan);
    }

}

void AGOAPAgent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CurrentAction && CurrentAction->bIsRunning)
    {
        CurrentAction->TickAction(DeltaTime, this);
    }

    // Tick reaction timer for planning
    if (bRequestReplan)
    {
        ReactionTimer -= DeltaTime;
        if (ReactionTimer <= 0.f)
        {
            PlanActions();
            bRequestReplan = false;
        }
    }
}

FString AGOAPAgent::GetWorldStateAsString() const
{
    return WorldState ? WorldState->GetStateAsString() : TEXT("No WorldState");
}

void AGOAPAgent::ApplyEffects(const TMap<FName, bool>& Effects)
{
    if (WorldState)
    {
        WorldState->Apply(Effects);
    }
}

void AGOAPAgent::RequestReplan()
{
    // Assign a random reaction time each replan
    ReactionTimer = FMath::FRandRange(MinReactionTime, MaxReactionTime);
    bRequestReplan = true;
}

void AGOAPAgent::PlanActions()
{
    SCOPE_CYCLE_COUNTER(STAT_GOAPPlannerTick);
    
    if (!Planner || !WorldState)
    {
        GOAP_LOG(this, EGOAPDebugLevel::Minimal, "PlanActions: missing Planner or WorldState.");
        return;
    }

    // step 1: stop any currently running actions before replanning
    for (UGOAPAction* Action : AvailableActions)
    {
        if (Action && Action->bIsRunning)
        {
            GOAP_LOG(this, EGOAPDebugLevel::Detailed, "Stopping current action: %s", *Action->GetName());
            Action->OnInterrupt(this);
        }
    }

    CurrentPlan.Empty();

    // step 2: sort all goals by priority (descending)
    AvailableGoals.Sort([](const UGOAPGoal& A, const UGOAPGoal& B)
        {
            return A.Priority > B.Priority; // higher = more important
        });

    // step 3: pick the first goal that is relevant
    UGOAPGoal* BestGoal = nullptr;

    for (UGOAPGoal* Goal : AvailableGoals)
    {
        if (!Goal) continue;

        bool bRelevant = Goal->IsRelevant(WorldState->CurrentState);

        GOAP_LOG(this, EGOAPDebugLevel::Detailed, "Evaluating Goal: %s | Priority: %.1f | Relevant: %s",
            *Goal->GetGoalName(), Goal->Priority, bRelevant ? TEXT("true") : TEXT("false"));

        if (bRelevant)
        {
            BestGoal = Goal;
            break; // Found the best relevant goal
        }
    }

    // step 4: fallback — if none are relevant, pick the lowest priority goal
    if (!BestGoal && AvailableGoals.Num() > 0)
    {
        BestGoal = AvailableGoals.Last();
        GOAP_LOG(this, EGOAPDebugLevel::Minimal, "[Agent] No relevant goals found — using fallback: %s", *BestGoal->GetGoalName());
    }

    if (!BestGoal)
    {
        GOAP_LOG(this, EGOAPDebugLevel::Minimal, "[Agent] No goals available at all.");
        return;
    }

    GOAP_LOG(this, EGOAPDebugLevel::Minimal, "[Agent] Selected goal: %s (Priority %.1f)",
        *BestGoal->GetGoalName(), BestGoal->Priority);

    // step 5: run the planner for that goal
    TArray<UGOAPAction*> PlannedActions;
    bool bFoundPlan = Planner->Plan(WorldState->CurrentState, BestGoal->DesiredState, AvailableActions, PlannedActions, DebugLevel);

    if (bFoundPlan)
    {
        GOAP_LOG(this, EGOAPDebugLevel::Minimal, "PlanActions: Found plan with %d steps.", PlannedActions.Num());
        for (int32 StepIndex = 0; StepIndex < PlannedActions.Num(); ++StepIndex)
        {
            if (PlannedActions[StepIndex])
            {
                GOAP_LOG(this, EGOAPDebugLevel::Detailed, "Step %d: %s", StepIndex, *PlannedActions[StepIndex]->GetName());
            }
        }
        CurrentPlan = PlannedActions;
    }
    else
    {
        GOAP_LOG(this, EGOAPDebugLevel::Minimal, "PlanActions: No valid plan found for goal: %s", *BestGoal->GetGoalName());
    }

    // step 6: execute the plan
    if (CurrentPlan.Num() > 0)
    {
        ExecutePlan();
    }
}


void AGOAPAgent::ExecutePlan()
{
    SCOPE_CYCLE_COUNTER(STAT_GOAPExecureGoal);

    if (CurrentPlan.Num() == 0)
    {
        GOAP_LOG(this, EGOAPDebugLevel::Minimal, "No actions in plan to execute.");
        return;
    }

    UGOAPAction* NextAction = CurrentPlan[0];
    if (!NextAction)
    {
        CurrentPlan.RemoveAt(0);
        ExecutePlan();
        return;
    }

    GOAP_LOG(this, EGOAPDebugLevel::Minimal, "Executing action: %s", *NextAction->GetName());

    // Call Execute on the action
    CurrentAction = NextAction;
    CurrentAction->Execute_Implementation(this);
}


void AGOAPAgent::SetEnemyVisible(bool bVisible)
{
    if (WorldState)
    {
        WorldState->Apply({ { "EnemyVisible", bVisible } });
        WorldState->Apply({ { "EnemyAlive", bVisible } });

        GOAP_LOG(this, EGOAPDebugLevel::Detailed, "%s: Enemy visibility set to %s", *GetName(), bVisible ? TEXT("true") : TEXT("false"));
    }
}