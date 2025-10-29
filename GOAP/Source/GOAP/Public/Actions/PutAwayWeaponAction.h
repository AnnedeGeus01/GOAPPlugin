#pragma once

#include "CoreMinimal.h"
#include "Actions/GOAPAction.h"
#include "PutAwayWeaponAction.generated.h"

UCLASS()
class GOAP_API UGOAPPutAwayWeaponAction : public UGOAPAction
{
    GENERATED_BODY()

public:
    UGOAPPutAwayWeaponAction();

    virtual void Execute_Implementation(AGOAPAgent* Agent) override;

};