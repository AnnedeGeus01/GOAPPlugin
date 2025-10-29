#pragma once
#include "CoreMinimal.h"
#include "Delegates/DelegateCombinations.h"
#include "Components/ActorComponent.h"
#include "GOAPTypes.h"
#include "GOAPWorldStateComponent.generated.h"

class AGOAPAgent;

/**
 * @brief Delegate called whenever the world state changes.
 *
 * Can be used to notify agents or other systems to replan or react.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWorldStateChanged);

/**
 * @brief Component that tracks the agent’s knowledge of the world in the system.
 *
 * This component stores the current state of the world as perceived by the agent.
 * It provides functions to apply effects from actions and notifies listeners when
 * the world state changes, allowing for reactive AI behavior.
 */
UCLASS(ClassGroup = (AI), meta = (BlueprintSpawnableComponent))
class GOAP_API UGOAPWorldStateComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    /** Default constructor */
    UGOAPWorldStateComponent();

    /**
     * @brief The agent’s current knowledge of the world.
     *
     * Stored as key-value pairs, where FName represents the state key (e.g. "HasWeapon")
     * and bool represents whether the condition is true or false.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
    FGOAPWorldState CurrentState;

    /**
     * @brief Returns a string representation of the current world state.
     *
     * Useful for debugging and logging.
     * @return String representation of the world state.
     */
    UFUNCTION(BlueprintCallable, Category = "GOAP")
    FString GetStateAsString() const;

    /**
     * @brief Reference to the agent that owns this world state component.
     *
     * Used for accessing debug information or context about the agent.
     */
    UPROPERTY(BlueprintReadOnly, Category = "GOAP")
    AGOAPAgent* OwningAgent;

    /**
     * @brief Event triggered whenever the world state changes.
     *
     * Agents can bind to this to automatically request replanning when relevant changes occur.
     */
    UPROPERTY(BlueprintAssignable, Category = "GOAP")
    FOnWorldStateChanged OnWorldStateChanged;

    /**
     * @brief Applies a set of effects to the current world state.
     *
     * If any state actually changes, triggers the OnWorldStateChanged delegate.
     *
     * @param Effects The key-value pairs representing state changes to apply.
     */
    void Apply(const TMap<FName, bool>& Effects);
};
