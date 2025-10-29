#pragma once

#include "CoreMinimal.h"
#include "GOAPGoal.h"
#include "ReloadGoal.generated.h"

UCLASS()
class GOAP_API UGOAPReloadGoal : public UGOAPGoal
{
    GENERATED_BODY()

public:
    UGOAPReloadGoal();

    // Override from base goal
    virtual FString GetGoalName_Implementation() const override;
    virtual bool IsRelevant_Implementation(const FGOAPWorldState& WorldState) const override;
};