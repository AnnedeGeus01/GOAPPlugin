#pragma once

#include "CoreMinimal.h"
#include "Actions/GOAPAction.h"
#include "TimerManager.h"
#include "PickupWeaponAction.generated.h"

UCLASS()
class GOAP_API UGOAPPickupWeaponAction : public UGOAPAction
{
    GENERATED_BODY()

public:
    UGOAPPickupWeaponAction();

    virtual void Execute_Implementation(AGOAPAgent* Agent) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP|Visuals")
    UAnimMontage* EquipMontage;

    FTimerHandle TimerHandle;
};
