#pragma once
#include "CoreMinimal.h"
#include "GOAPTypes.generated.h"

/// \file GOAPTypes.h

/**
 * @brief Represents a set of world state facts for the GOAP system.
 *
 * This struct stores boolean key-value pairs that describe the current known state of the world.
 * It supports operations to compare and merge world states.
 */
USTRUCT(BlueprintType)
struct FGOAPWorldState
{
    GENERATED_BODY()

    /**
     * @brief Stores boolean facts about the world.
     *
     * Each key represents a fact or condition (e.g. "HasWeapon"), and the value indicates
     * whether that condition is currently true or false.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
    TMap<FName, bool> Bools;

    /**
     * @brief Checks if this state satisfies another desired state.
     *
     * This function returns true if all facts in the desired state are present
     * and have matching values in this state.
     *
     * @param Desired The state that we want to verify against this one.
     * @return True if this state satisfies all conditions in the desired state, false otherwise.
     */
    bool Satisfies(const FGOAPWorldState& Desired) const
    {
        for (const auto& Pair : Desired.Bools)
        {
            const bool* Found = Bools.Find(Pair.Key);
            if (!Found || *Found != Pair.Value)
            {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Merges another state's values into this one.
     *
     * Each key-value pair from the provided state is applied to this one,
     * overwriting existing entries or adding new ones.
     *
     * @param Effects The state whose facts should be applied to this state.
     */
    void Apply(const FGOAPWorldState& Effects)
    {
        for (const auto& Pair : Effects.Bools)
        {
            Bools.FindOrAdd(Pair.Key) = Pair.Value;
        }
    }
};
