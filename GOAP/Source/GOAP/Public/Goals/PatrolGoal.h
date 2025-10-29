#pragma once

#include "CoreMinimal.h"
#include "GOAPGoal.h"
#include "PatrolGoal.generated.h"

UCLASS()
class GOAP_API UGOAPPatrolGoal : public UGOAPGoal
{
    GENERATED_BODY()

public:
    UGOAPPatrolGoal();

    // Override from base goal
    virtual FString GetGoalName_Implementation() const override;
    virtual bool IsRelevant_Implementation(const FGOAPWorldState& WorldState) const override;
};