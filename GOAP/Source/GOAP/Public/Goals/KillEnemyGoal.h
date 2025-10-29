#pragma once

#include "CoreMinimal.h"
#include "GOAPGoal.h"
#include "KillEnemyGoal.generated.h"

UCLASS()
class GOAP_API UGOAPKillEnemyGoal : public UGOAPGoal
{
    GENERATED_BODY()

public:
    UGOAPKillEnemyGoal();

    // Override from base goal
    virtual FString GetGoalName_Implementation() const override;
    virtual bool IsRelevant_Implementation(const FGOAPWorldState& WorldState) const override;
};
