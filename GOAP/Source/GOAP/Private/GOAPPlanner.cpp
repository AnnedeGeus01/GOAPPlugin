#include "GOAPPlanner.h"
#include "Actions/GOAPAction.h"

// Set up for categorizing debug information
#define GOAP_LOG_PLANNER(Level, RequiredLevel, Format, ...) \
    if (Level >= RequiredLevel) \
    { \
        UE_LOG(LogTemp, Warning, TEXT(Format), ##__VA_ARGS__); \
    }

// Internal planner node struct
struct FPlanNode
{
    FGOAPWorldState State;
    TArray<UGOAPAction*> Path;
    float G = 0.f;
    float H = 0.f;
    float F() const { return G + H; }
};

// Helper functions
static FString SerializeWorldState(const FGOAPWorldState& S)
{
    TArray<FName> Keys;
    S.Bools.GenerateKeyArray(Keys);
    Keys.Sort([](const FName& A, const FName& B) { return A.ToString() < B.ToString(); });

    FString Out;
    for (const FName& K : Keys)
    {
        const bool* V = S.Bools.Find(K);
        Out += FString::Printf(TEXT("%s=%d;"), *K.ToString(), V && *V ? 1 : 0);
    }
    return Out;
}

static int32 UnsatisfiedGoalCount(const FGOAPWorldState& State, const FGOAPWorldState& Goal)
{
    int32 Count = 0;
    for (const auto& Kv : Goal.Bools)
    {
        const bool* Found = State.Bools.Find(Kv.Key);
        if (!Found || *Found != Kv.Value)
        {
            ++Count;
        }
    }
    return Count;
}

static bool SatisfiesPreconditions(const FGOAPWorldState& State, const TMap<FName, bool>& Preconditions)
{
    for (const auto& Kv : Preconditions)
    {
        const bool* Found = State.Bools.Find(Kv.Key);
        if (!Found || *Found != Kv.Value)
        {
            return false;
        }
    }
    return true;
}

// Main planning function
bool UGOAPPlanner::Plan(const FGOAPWorldState& Current,
    const FGOAPWorldState& Goal,
    const TArray<UGOAPAction*>& Actions,
    TArray<UGOAPAction*>& OutPlan,
    EGOAPDebugLevel DebugLevel)
{
    OutPlan.Reset();

    if (Current.Satisfies(Goal))
    {
        GOAP_LOG_PLANNER(DebugLevel, EGOAPDebugLevel::Minimal,
            "[Planner] Current state already satisfies goal.");
        return true;
    }

    TArray<FPlanNode> Open;
    TSet<FString> Closed;

    FPlanNode Start;
    Start.State = Current;
    Start.G = 0.f;
    Start.H = (float)UnsatisfiedGoalCount(Current, Goal);
    Open.Add(MoveTemp(Start));

    const int32 MaxIterations = 5000;
    int32 Iter = 0;

    while (Open.Num() > 0 && Iter++ < MaxIterations)
    {
        int32 BestIndex = 0;
        float BestF = Open[0].F();
        for (int32 i = 1; i < Open.Num(); ++i)
        {
            if (Open[i].F() < BestF)
            {
                BestF = Open[i].F();
                BestIndex = i;
            }
        }

        FPlanNode Node = Open[BestIndex];
        Open.RemoveAtSwap(BestIndex);

        GOAP_LOG_PLANNER(DebugLevel, EGOAPDebugLevel::Detailed,
            "[Planner] Expanding node (G=%.2f, H=%.2f, F=%.2f) | OpenList=%d | Closed=%d",
            Node.G, Node.H, Node.F(), Open.Num(), Closed.Num());

        // Goal test
        if (Node.State.Satisfies(Goal))
        {
            OutPlan = Node.Path;
            GOAP_LOG_PLANNER(DebugLevel, EGOAPDebugLevel::Minimal,
                "[Planner] Plan found with %d steps (G=%.2f, H=%.2f, Iter=%d)",
                OutPlan.Num(), Node.G, Node.H, Iter);

            FString Seq;
            for (UGOAPAction* A : OutPlan)
            {
                Seq += FString::Printf(TEXT("%s -> "), A ? *A->GetName() : TEXT("null"));
            }
            GOAP_LOG_PLANNER(DebugLevel, EGOAPDebugLevel::Minimal, "[GOAPPlanner] Plan sequence: %s", *Seq);
            return true;
        }

        FString NodeKey = SerializeWorldState(Node.State);
        if (Closed.Contains(NodeKey))
        {
            continue;
        }
        Closed.Add(NodeKey);

        // Expand by actions
        for (UGOAPAction* Action : Actions)
        {
            if (!Action) continue;

            // Action preconditions must be satisfied in this node state
            if (!SatisfiesPreconditions(Node.State, Action->Preconditions))
            {
                GOAP_LOG_PLANNER(DebugLevel, EGOAPDebugLevel::Detailed,
                    "[Planner] Skipping %s (preconditions not met)", *Action->GetName());
                continue;
            }

            // Build child node: state after applying action effects
            FPlanNode Child;
            Child.State = Node.State;
            // Apply action effects into child.State
            for (const auto& EffectKv : Action->Effects)
            {
                Child.State.Bools.FindOrAdd(EffectKv.Key) = EffectKv.Value;
            }

            // If already visited this resulting state, skip
            FString ChildKey = SerializeWorldState(Child.State);
            if (Closed.Contains(ChildKey))
            {
                GOAP_LOG_PLANNER(DebugLevel, EGOAPDebugLevel::Detailed,
                    "[Planner] Skipping %s (already visited state)", *Action->GetName());
                continue;
            }

            // Update path and costs
            Child.Path = Node.Path;
            Child.Path.Add(Action);
            Child.G = Node.G + Action->Cost;
            Child.H = (float)UnsatisfiedGoalCount(Child.State, Goal);

            GOAP_LOG_PLANNER(DebugLevel, EGOAPDebugLevel::Detailed,
                "[Planner] Added child via %s (G=%.2f, H=%.2f, F=%.2f)",
                *Action->GetName(), Child.G, Child.H, Child.F());

            // Add to open list
            Open.Add(MoveTemp(Child));
        }
    }

    GOAP_LOG_PLANNER(DebugLevel, EGOAPDebugLevel::Minimal,
        "[Planner] No plan found after %d iterations (Open=%d, Closed=%d)", Iter, Open.Num(), Closed.Num());
    return false;
}
