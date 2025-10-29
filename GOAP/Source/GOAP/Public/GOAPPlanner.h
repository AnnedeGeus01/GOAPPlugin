#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GOAPTypes.h"
#include "GOAPDebug.h"
#include "GOAPPlanner.generated.h"

class UGOAPAction;

/**
 * @brief GOAP planner that computes an action sequence to achieve a desired goal.
 *
 * The planner evaluates actions based on their preconditions, effects, and cost
 * to generate efficient plans that can dynamically adapt to changing world conditions.
 */
UCLASS(BlueprintType)
class GOAP_API UGOAPPlanner : public UObject
{
    GENERATED_BODY()

public:
    /**
     * @brief Attempts to generate a plan (sequence of actions) to reach the goal state.
     *
     * This function searches through the provided set of actions, building a sequence
     * that satisfies the goal from the current world state. The resulting plan
     * is ordered by execution (first to last).
     *
     * @param Current The current world state as perceived by the agent.
     * @param Goal The desired goal state to achieve.
     * @param Actions The list of available actions that can be used to plan.
     * @param OutPlan Output array that will contain the resulting ordered plan.
     * @param DebugLevel Optional debug verbosity level for logging planner details.
     * @return True if a valid plan was found, false otherwise.
     */
    UFUNCTION(BlueprintCallable, Category = "GOAP")
    bool Plan(
        const FGOAPWorldState& Current,
        const FGOAPWorldState& Goal,
        const TArray<UGOAPAction*>& Actions,
        TArray<UGOAPAction*>& OutPlan,
        EGOAPDebugLevel DebugLevel = EGOAPDebugLevel::None
    );
};
