#pragma once

#include "CoreMinimal.h"
#include "Actions/GOAPAction.h"
#include "AttackAction.generated.h"

UCLASS()
class GOAP_API UGOAPAttackAction : public UGOAPAction
{
    GENERATED_BODY()

public:
    UGOAPAttackAction();

    // The shooting animation montage
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* ShootMontage;

    virtual void Execute_Implementation(AGOAPAgent* Agent) override;

private:
    FTimerHandle TimerHandle;
};
