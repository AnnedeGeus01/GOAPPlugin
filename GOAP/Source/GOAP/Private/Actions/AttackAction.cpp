#include "Actions/AttackAction.h"
#include "GOAPAgent.h"

UGOAPAttackAction::UGOAPAttackAction()
{
    Cost = 3.0f;

    // Preconditions: need a weapon and an enemy
    Preconditions.Add("HasWeapon", true);
    Preconditions.Add("EnemyVisible", true);
    Preconditions.Add("HasBullets", true);

    // Effects: enemy is attacked
    Effects.Add("EnemyAlive", false);
    Effects.Add("EnemyVisible", false);
    Effects.Add("HasBullets", false);

    // Load the shoot montage from the asset path
    static ConstructorHelpers::FObjectFinder<UAnimMontage> ShootMontageObj(
        TEXT("/Script/Engine.AnimMontage'/Game/Characters/Mannequins/Anims/Pistol/MM_Pistol_DryFire_Montage.MM_Pistol_DryFire_Montage'"));

    if (ShootMontageObj.Succeeded())
    {
        ShootMontage = ShootMontageObj.Object;
    }
    else
    {
        ShootMontage = nullptr;
        UE_LOG(LogTemp, Warning, TEXT("Failed to load ShootMontage! Check the asset path."));
    }
}

void UGOAPAttackAction::Execute_Implementation(AGOAPAgent* Agent)
{
    if (!Agent || !ShootMontage) return;

    GOAP_ACTION_LOG(Agent, EGOAPDebugLevel::Detailed, "Attacking enemy...");

    // Play shooting animation
    if (UAnimInstance* AnimInstance = Agent->GetMesh()->GetAnimInstance())
    {
        AnimInstance->Montage_Play(ShootMontage);
    }

    // Apply effects and finish after the montage finishes
    float MontageDuration = ShootMontage ? ShootMontage->GetPlayLength() : 0.5f;

    Agent->GetWorldTimerManager().SetTimer(TimerHandle,
        [this, Agent]()
        {
            if (Agent)
            {
                GOAP_ACTION_LOG(Agent, EGOAPDebugLevel::Detailed, "Finished attacking enemy.");
                Finish(Agent, true);
            }
        },
        MontageDuration,
        false
    );
}

