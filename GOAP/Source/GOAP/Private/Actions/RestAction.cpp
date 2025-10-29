#include "Actions/RestAction.h"
#include "GOAPAgent.h"
#include "GOAPAnimInstance.h"

UGOAPRestAction::UGOAPRestAction()
{
    Cost = 1.0f;
    
    Preconditions.Add("IsExhausted", true);
    Effects.Add("IsExhausted", false);
}

void UGOAPRestAction::Execute_Implementation(AGOAPAgent* Agent)
{
    if (!Agent)
    {
        Finish(Agent, false);
        return;
    }

    GOAP_ACTION_LOG(Agent, EGOAPDebugLevel::Detailed, "%s started resting...", *Agent->GetName());
    bIsRunning = true;

    if (UGOAPAnimInstance* GOAPAnim = Cast<UGOAPAnimInstance>(Agent->GetMesh()->GetAnimInstance()))
    {
        GOAPAnim->bIsWalking = false;
    }
}

void UGOAPRestAction::TickAction_Implementation(float DeltaTime, AGOAPAgent* Agent)
{
    if (!bIsRunning || !Agent) return;

    // Recharge stamina slowly
    Agent->ExhaustionLimit += DeltaTime * 20.f;  // tune rate

    if (Agent->ExhaustionLimit >= 100.f) // full recovery
    {
        Agent->ExhaustionLimit = 100.f;

        GOAP_ACTION_LOG(Agent, EGOAPDebugLevel::Detailed, "%s finished resting. Fully recovered!", *Agent->GetName());

        Finish(Agent, true);
    }
}
