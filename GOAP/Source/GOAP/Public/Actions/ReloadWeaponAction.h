#pragma once

#include "CoreMinimal.h"
#include "Actions/GOAPAction.h"
#include "ReloadWeaponAction.generated.h"

UCLASS()
class GOAP_API UGOAPReloadWeaponWeaponAction : public UGOAPAction
{
    GENERATED_BODY()

public:
    UGOAPReloadWeaponWeaponAction();

    virtual void Execute_Implementation(AGOAPAgent* Agent) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP|Visuals")
    UAnimMontage* ReloadMontage;

    FTimerHandle TimerHandle;
};
