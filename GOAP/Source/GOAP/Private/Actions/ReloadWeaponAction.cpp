#include "Actions/ReloadWeaponAction.h"
#include "GOAPAgent.h"
#include "GOAPAnimInstance.h"

UGOAPReloadWeaponWeaponAction::UGOAPReloadWeaponWeaponAction()
{
	Cost = 3.f;

    // Preconditions
    Preconditions.Add("HasWeapon", true);
    Preconditions.Add("HasBullets", false);

    // Effects
    Effects.Add("HasBullets", true);

    // Load the reload montage from the asset path
    static ConstructorHelpers::FObjectFinder<UAnimMontage> ReloadMontageObj(
        TEXT("/Script/Engine.AnimMontage'/Game/Characters/Mannequins/Anims/Pistol/MM_Pistol_Reload_Montage.MM_Pistol_Reload_Montage'"));

    if (ReloadMontageObj.Succeeded())
    {
        ReloadMontage = ReloadMontageObj.Object;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load ReloadMontage! Check the asset path."));
        ReloadMontage = nullptr;
    }
}

void UGOAPReloadWeaponWeaponAction::Execute_Implementation(AGOAPAgent* Agent)
{
    if (!Agent || !ReloadMontage)
    {
        GOAP_ACTION_LOG(Agent, EGOAPDebugLevel::Minimal, "Cannot execute ReloadWeaponAction: Agent or ReloadMontage is null.");
        Finish(Agent, false);
        return;
    }

    GOAP_ACTION_LOG(Agent, EGOAPDebugLevel::Detailed, "Reload weapon...");

    // Play reload animation
    UAnimInstance* AnimInstance = Agent->GetMesh()->GetAnimInstance();
    if (AnimInstance)
    {
        AnimInstance->Montage_Play(ReloadMontage);
    }

    float MontageDuration = ReloadMontage ? ReloadMontage->GetPlayLength() : 0.5f;

    Agent->GetWorldTimerManager().SetTimer(TimerHandle,
        [this, Agent]()
        {
            GOAP_ACTION_LOG(Agent, EGOAPDebugLevel::Detailed, "Finished reloading weapon.");
            Finish(Agent, true);
        },
        MontageDuration, false);
}
