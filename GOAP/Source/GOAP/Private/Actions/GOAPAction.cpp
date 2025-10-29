#include "Actions/GOAPAction.h"
#include "GOAPAgent.h"

UGOAPAction::UGOAPAction()
{
    Cost = 1.0f; // Default cost
}

bool UGOAPAction::CanExecute_Implementation(const TMap<FName, bool>& WorldState) const
{
    //GOAP_ACTION_LOG(nullptr, EGOAPDebugLevel::Minimal, "Checking action: %s", *GetName());

    for (const auto& Pair : Preconditions)
    {
        const bool* Found = WorldState.Find(Pair.Key);
        if (!Found || *Found != Pair.Value)
        {
            return false;
        }
    }

    //GOAP_ACTION_LOG(nullptr, EGOAPDebugLevel::Minimal, "All preconditions satisfied for action: %s", *GetName());
    return true;
}

void UGOAPAction::Execute_Implementation(AGOAPAgent* Agent)
{
    GOAP_ACTION_LOG(Agent, EGOAPDebugLevel::Minimal, "Executing action: %s", *GetClass()->GetName());
    Finish(Agent, true);
}

void UGOAPAction::TickAction_Implementation(float DeltaTime, AGOAPAgent* Agent)
{
    // Default does nothing
}

void UGOAPAction::Finish_Implementation(AGOAPAgent* Agent, bool bSuccess)
{
    bIsRunning = false;
    GOAP_ACTION_LOG(Agent, EGOAPDebugLevel::Minimal, "Action %s finished: %s",
        *GetClass()->GetName(), bSuccess ? TEXT("SUCCESS") : TEXT("FAIL"));

    if (bSuccess && Agent)
    {
        Agent->ApplyEffects(Effects);
        // Remove the finished action from the plan and continue
        if (Agent->CurrentPlan.Num() > 0)
        {
            Agent->CurrentPlan.RemoveAt(0);
            Agent->ExecutePlan();
        }
    }
}

void UGOAPAction::OnInterrupt_Implementation(AGOAPAgent* Agent)
{
    // Default does nothing
}