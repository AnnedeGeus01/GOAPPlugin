#include "Actions/PickupWeaponAction.h"
#include "GOAPAgent.h"
#include "GOAPAnimInstance.h"

UGOAPPickupWeaponAction::UGOAPPickupWeaponAction()
{
    Cost = 1.5f;

    Preconditions.Add("HasWeapon", false);
    Effects.Add("HasWeapon", true);

    // Load the equip montage from the asset path
    static ConstructorHelpers::FObjectFinder<UAnimMontage> EquipMontageObj(
        TEXT("/Script/Engine.AnimMontage'/Game/Characters/Mannequins/Anims/Pistol/MM_Pistol_Equip_Montage.MM_Pistol_Equip_Montage'"));

    if (EquipMontageObj.Succeeded())
    {
        EquipMontage = EquipMontageObj.Object;
    }
    else
    {
        EquipMontage = nullptr;
        UE_LOG(LogTemp, Warning, TEXT("Failed to load EquipMontage! Check the asset path."));
    }
}

void UGOAPPickupWeaponAction::Execute_Implementation(AGOAPAgent* Agent)
{
    if (!Agent || !EquipMontage)
    {
        GOAP_ACTION_LOG(Agent, EGOAPDebugLevel::Minimal, "Cannot execute PickupWeaponAction: Agent or EquipMontage is null.");
        Finish(Agent, false);
        return;
    }

    GOAP_ACTION_LOG(Agent, EGOAPDebugLevel::Detailed, "%s: Picking up a weapon...", *Agent->GetName());

    // Play equip animation
    UAnimInstance* AnimInstance = Agent->GetMesh()->GetAnimInstance();
    if (AnimInstance)
    {
        AnimInstance->Montage_Play(EquipMontage);
    }

    float MontageDuration = EquipMontage ? EquipMontage->GetPlayLength() : 0.5f;

    Agent->GetWorldTimerManager().SetTimer(TimerHandle,
        [this, Agent]()
        {
            GOAP_ACTION_LOG(Agent, EGOAPDebugLevel::Detailed, "%s: Finished equipping weapon.", *Agent->GetName());
            Finish(Agent, true);
        },
        MontageDuration, false);
}

