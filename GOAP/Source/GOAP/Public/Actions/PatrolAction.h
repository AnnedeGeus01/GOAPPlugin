#pragma once

#include "CoreMinimal.h"
#include "Actions/GOAPAction.h"
#include "PatrolAction.generated.h"

UCLASS()
class GOAP_API UGOAPPatrolAction : public UGOAPAction
{
    GENERATED_BODY()

public:
    UGOAPPatrolAction();

    virtual void Execute_Implementation(AGOAPAgent* Agent) override;

    virtual void TickAction_Implementation(float DeltaTime, AGOAPAgent* agent);

    virtual void OnInterrupt_Implementation(AGOAPAgent* Agent);
};
