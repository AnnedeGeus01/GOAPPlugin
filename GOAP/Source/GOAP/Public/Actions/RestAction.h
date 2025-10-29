#pragma once

#include "CoreMinimal.h"
#include "Actions/GOAPAction.h"
#include "RestAction.generated.h"

UCLASS()
class GOAP_API UGOAPRestAction : public UGOAPAction
{
    GENERATED_BODY()

public:
    UGOAPRestAction();

    virtual void Execute_Implementation(AGOAPAgent* Agent) override;

    virtual void TickAction_Implementation(float DeltaTime, AGOAPAgent* Agent);
};
