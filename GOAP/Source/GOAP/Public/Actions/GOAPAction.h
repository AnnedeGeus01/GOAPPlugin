#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GOAPAction.generated.h"

class AGOAPAgent;

/**
 * @brief Macro for categorized GOAP action logging.
 *
 * This macro conditionally logs messages based on the agent’s current @ref EGOAPDebugLevel.
 *
 * @param Agent The agent whose debug level determines if logging occurs.
 * @param Level The required debug level to display this message.
 * @param Format The log format string (uses UE_LOG syntax).
 * @param ... Additional arguments for the format string.
 */
#define GOAP_ACTION_LOG(Agent, Level, Format, ...) \
{ \
    EGOAPDebugLevel CurrentLevel = (Agent) ? Agent->DebugLevel : EGOAPDebugLevel::Minimal; \
    if (CurrentLevel >= Level) \
    { \
        UE_LOG(LogTemp, Warning, TEXT(Format), ##__VA_ARGS__); \
    } \
}

 /**
  * @brief Base class for all GOAP actions.
  *
  * This abstract class defines the structure and behavior of actions that can be
  * planned and executed by a GOAP agent. Each action specifies preconditions and
  * effects that describe how it changes the world state, and optionally implements
  * continuous behavior through @ref TickAction.
  */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class GOAP_API UGOAPAction : public UObject
{
    GENERATED_BODY()

public:
    /** Default constructor */
    UGOAPAction();

    /**
     * @brief Preconditions that must be satisfied before this action can execute.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
    TMap<FName, bool> Preconditions;

    /**
     * @brief Effects produced by this action once it finishes.
     *
     * How the world state changes after the action.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
    TMap<FName, bool> Effects;

    /**
     * @brief The action’s cost value.
     *
     * Used by the planner to evaluate plan efficiency, lower cost actions are more desirable.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
    float Cost;

    /**
     * @brief Indicates whether this action is currently executing.
     *
     * Used for long-running or continuous actions (e.g. patrol).
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GOAP")
    bool bIsRunning;

    /**
     * @brief Determines if this action can execute under the given world state.
     *
     * Override to implement custom logic for availability checks.
     *
     * @param WorldState The agent’s current knowledge of the world.
     * @return True if the action can execute under this world state, false otherwise.
     */
    UFUNCTION(BlueprintNativeEvent, Category = "GOAP")
    bool CanExecute(const TMap<FName, bool>& WorldState) const;

    /**
     * @brief Executes the action.
     *
     * Called when the action begins execution. Override in derived actions to define custom behavior.
     *
     * @param Agent The agent executing this action.
     */
    UFUNCTION(BlueprintNativeEvent, Category = "GOAP")
    void Execute(AGOAPAgent* Agent);

    /**
     * @brief Ticks a continuous action each frame.
     *
     * This function is called while @ref bIsRunning is true. Override for long-running actions.
     *
     * @param DeltaTime The time since the last frame.
     * @param Agent The agent performing the action.
     */
    UFUNCTION(BlueprintNativeEvent, Category = "GOAP")
    void TickAction(float DeltaTime, AGOAPAgent* Agent);

    /**
     * @brief Called when the action finishes (successfully or unsuccessfully).
     *
     * Use this to clean up or trigger effects that occur at the end of an action.
     *
     * @param Agent The agent that executed the action.
     * @param bSuccess True if the action completed successfully, false if it failed.
     */
    UFUNCTION(BlueprintNativeEvent, Category = "GOAP")
    void Finish(AGOAPAgent* Agent, bool bSuccess);

    /**
     * @brief Called when the action is interrupted before completion.
     *
     * Override this to handle cleanup if an action is canceled mid-execution.
     *
     * @param Agent The agent that was performing the action.
     */
    UFUNCTION(BlueprintNativeEvent, Category = "GOAP")
    void OnInterrupt(AGOAPAgent* Agent);
};
