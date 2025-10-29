#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/NoExportTypes.h"
#include "GOAPTypes.h"
#include "GOAPGoal.generated.h"

/**
 * @brief Represents a GOAP Goal: a desired world state the agent aims to achieve.
 *
 * Each goal defines a target world state and a priority level.
 * The GOAP system evaluates all available goals and plans a sequence of actions
 * that best satisfies the most relevant and highest-priority goal.
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class GOAP_API UGOAPGoal : public UObject
{
    GENERATED_BODY()

public:
    /** Default constructor */
    UGOAPGoal();

    /**
     * @brief The desired world state this goal represents.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
    FGOAPWorldState DesiredState;

    /**
     * @brief The goal's priority level.
     *
     * Higher values indicate more important goals.
     * The planner uses this to decide which goal to do first when multiple are valid.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
    float Priority;

    /**
     * @brief Returns the display name of the goal.
     *
     * Useful for debugging, visualization, and logging purposes.
     *
     * @return The name of this goal.
     */
    UFUNCTION(BlueprintNativeEvent, Category = "GOAP")
    FString GetGoalName() const;

    /**
     * @brief Determines whether this goal is relevant given the current world state.
     *
     * Override this in derived goals to add custom logic for when a goal should be considered.
     * For example, a "Rest" goal might only be relevant if the agent’s energy is below a threshold.
     *
     * @param WorldState The current world state as perceived by the agent.
     * @return True if the goal should be considered, false otherwise.
     */
    UFUNCTION(BlueprintNativeEvent, Category = "GOAP")
    bool IsRelevant(const FGOAPWorldState& WorldState) const;
};
