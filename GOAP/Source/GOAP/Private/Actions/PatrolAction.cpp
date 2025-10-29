#include "Actions/PatrolAction.h"
#include "GOAPAgent.h"
#include "GOAPAnimInstance.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

UGOAPPatrolAction::UGOAPPatrolAction()
{
    Cost = 5.0f;

    Preconditions.Add("HasWeapon", false);
    Preconditions.Add("IsExhausted", false);

    Effects.Add("IsPatrolling", true);
}

void UGOAPPatrolAction::Execute_Implementation(AGOAPAgent* Agent)
{
    if (!Agent)
    {
        Finish(Agent, false);
        return;
    }

    GOAP_ACTION_LOG(Agent, EGOAPDebugLevel::Detailed, "%s: Starting patrol action...", *Agent->GetName());

    bIsRunning = true;

    if (UGOAPAnimInstance* GOAPAnim = Cast<UGOAPAnimInstance>(Agent->GetMesh()->GetAnimInstance()))
    {
        GOAPAnim->bIsWalking = true;
    }

    // Get the navigation system
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Agent->GetWorld());
    if (!NavSys)
    {
        GOAP_ACTION_LOG(Agent, EGOAPDebugLevel::Detailed, "No NavSystem found!");
        Finish(Agent, false);
        return;
    }

    // Pick a random point in a 1000 unit radius
    FVector Origin = Agent->GetActorLocation();
    FNavLocation RandomLocation;

    bool bFound = NavSys->GetRandomReachablePointInRadius(Origin, 1000.f, RandomLocation);
    if (!bFound)
    {
        GOAP_ACTION_LOG(Agent, EGOAPDebugLevel::Minimal, "Could not find random patrol location!");
        Finish(Agent, false);
        return;
    }

    // Move the agent to that location
    AAIController* AICon = Cast<AAIController>(Agent->GetController());
    if (!AICon)
    {
        GOAP_ACTION_LOG(Agent, EGOAPDebugLevel::Minimal, "PatrolAction: Agent has no AIController.");
        Finish(Agent, false);
        return;
    }

    GOAP_ACTION_LOG(Agent, EGOAPDebugLevel::Detailed, "%s: Moving to patrol location: %s",
        *Agent->GetName(), *RandomLocation.Location.ToString());

    AICon->MoveToLocation(RandomLocation.Location);
}

void UGOAPPatrolAction::TickAction_Implementation(float DeltaTime, AGOAPAgent* Agent)
{
    if (!bIsRunning || !Agent) return;

    AAIController* AICon = Cast<AAIController>(Agent->GetController());
    if (!AICon) return;

    EPathFollowingStatus::Type Status = AICon->GetMoveStatus();

    // Decrease exhaustion gradually while patrolling
    Agent->ExhaustionLimit -= Agent->ExhaustionDrainRate * DeltaTime;

    // Clamp to [0, 100]
    Agent->ExhaustionLimit = FMath::Clamp(Agent->ExhaustionLimit, 0.f, 100.f);

    // If exhausted update world state
    if (Agent->ExhaustionLimit <= 0.f)
    {
        GOAP_ACTION_LOG(Agent, EGOAPDebugLevel::Minimal, "%s is now exhausted!", *Agent->GetName());

        // Update world state: mark exhausted and stop patrolling
        Agent->GetWorldState()->Apply({
            { "IsExhausted", true } });

        // Stop current movement
        if (AICon->GetMoveStatus() != EPathFollowingStatus::Idle)
        {
            AICon->StopMovement();
        }

        bIsRunning = false;

        // End patrol action so GOAP can replan
        return;
    }


    // Check if reached destination
    if (Status == EPathFollowingStatus::Idle || Status == EPathFollowingStatus::Waiting)
    {
        GOAP_ACTION_LOG(Agent, EGOAPDebugLevel::Detailed, "PatrolAction: Reached destination — picking new patrol point.");

        UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Agent->GetWorld());
        if (!NavSys) return;

        FVector Origin = Agent->GetActorLocation();
        FNavLocation RandomLocation;

        if (NavSys->GetRandomReachablePointInRadius(Origin, 1000.f, RandomLocation))
        {
            GOAP_ACTION_LOG(Agent, EGOAPDebugLevel::Detailed, "%s: Moving to next patrol location: %s",
                *Agent->GetName(), *RandomLocation.Location.ToString());

            AICon->MoveToLocation(RandomLocation.Location);
        }
    }
}

void UGOAPPatrolAction::OnInterrupt_Implementation(AGOAPAgent* Agent)
{
    bIsRunning = false;

    if (AAIController* AICon = Cast<AAIController>(Agent->GetController()))
    {
        AICon->StopMovement();
    }

    if (UGOAPAnimInstance* GOAPAnim = Cast<UGOAPAnimInstance>(Agent->GetMesh()->GetAnimInstance()))
    {
        GOAPAnim->bIsWalking = false;   // stops walking animation
    }
}
