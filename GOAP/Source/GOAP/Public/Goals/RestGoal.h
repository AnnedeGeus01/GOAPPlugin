#pragma once

#include "CoreMinimal.h"
#include "GOAPGoal.h"
#include "RestGoal.generated.h"

UCLASS()
class GOAP_API UGOAPRestGoal : public UGOAPGoal
{
    GENERATED_BODY()

public:
    UGOAPRestGoal();

    // Override from base goal
    virtual FString GetGoalName_Implementation() const override;
    virtual bool IsRelevant_Implementation(const FGOAPWorldState& WorldState) const override;
};