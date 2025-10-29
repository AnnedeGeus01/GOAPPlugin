#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GOAPWorldStateComponent.h"
#include "GOAPPlanner.h"
#include "Goals/GOAPGoal.h"
#include "GOAPDebug.h"
#include "GOAPAgent.generated.h"

class UGOAPAction;

/**
 * @brief GOAP Agent responsible for managing goals, actions, and planning.
 *
 * This class represents an agent that uses the GOAP (Goal-Oriented Action Planning)
 * system to dynamically plan and execute sequences of actions that achieve its goals.
 * It maintains a knowledge of the world, evaluates goals, and reacts to world state changes.
 */
UCLASS()
class GOAP_API AGOAPAgent : public ACharacter
{
    GENERATED_BODY()

public:
    /** Default constructor */
    AGOAPAgent();

protected:
    /**
     * @brief Component that stores the agent's current perception of the world.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GOAP")
    UGOAPWorldStateComponent* WorldState;

    /**
     * @brief List of action classes available to this agent.
     *
     * Each action has Preconditions and Effects.
     * These are set in the editor and instantiated at runtime into @ref AvailableActions.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GOAP")
    TArray<TSubclassOf<UGOAPAction>> ActionClasses;

    /**
     * @brief Instantiated action objects created from @ref ActionClasses.
     */
    UPROPERTY()
    TArray<UGOAPAction*> AvailableActions;

    /**
     * @brief List of possible goal classes this agent can pursue.
     *
     * Each goal defines a desired world state and an associated priority.
     * These are set in the editor and instantiated at runtime into @ref AvailableGoals.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GOAP")
    TArray<TSubclassOf<UGOAPGoal>> GoalClasses;

    /**
     * @brief Instantiated goal objects derived from @ref GoalClasses.
     */
    UPROPERTY()
    TArray<UGOAPGoal*> AvailableGoals;

    /**
     * @brief Planner responsible for generating optimal action sequences.
     */
    UPROPERTY()
    UGOAPPlanner* Planner;

public:
    /** Called when the game starts or the actor is spawned. */
    virtual void BeginPlay() override;

    /** Called every frame. */
    virtual void Tick(float DeltaTime) override;

    /**
     * @brief Returns a string representation of the current world state.
     *
     * Useful for debugging and visualization in Blueprints.
     * @return String representing the agent's current world state.
     */
    UFUNCTION(BlueprintCallable, Category = "GOAP")
    FString GetWorldStateAsString() const;

    /**
     * @brief Provides access to the world state component.
     * @return Pointer to the @ref UGOAPWorldStateComponent owned by this agent.
     */
    UGOAPWorldStateComponent* GetWorldState() const { return WorldState; }

    /**
     * @brief Applies a set of effects (key-value pairs) to the agent’s world state.
     *
     * @param Effects The key-value pairs representing state changes to apply.
     */
    void ApplyEffects(const TMap<FName, bool>& Effects);

    /**
     * @brief The currently active plan (list of actions to execute in sequence).
     */
    UPROPERTY()
    TArray<UGOAPAction*> CurrentPlan;

    /**
     * @brief Requests the agent to reevaluate its goals and generate a new plan.
     */
    UFUNCTION(BlueprintCallable, Category = "GOAP")
    void RequestReplan();

    /**
     * @brief Uses the GOAP planner to create a new plan based on the current world state and goals.
     *
     * The resulting plan is stored in @ref CurrentPlan.
     */
    UFUNCTION(BlueprintCallable, Category = "GOAP")
    void PlanActions();

    /**
     * @brief Executes the current plan step by step.
     *
     * Each action is performed in order until the plan is complete or invalidated.
     */
    UFUNCTION(BlueprintCallable, Category = "GOAP")
    void ExecutePlan();

    /**
     * @brief The action currently being executed by the agent.
     */
    UPROPERTY()
    UGOAPAction* CurrentAction;

    /**
     * @brief The agent’s maximum stamina or energy capacity.
     *
     * A value of 100 represents fully rested.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
    float ExhaustionLimit = 100.f;

    /**
     * @brief How quickly the agent becomes tired while performing actions.
     *
     * This value can influence goals related to rest or recovery.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
    float ExhaustionDrainRate = 5.f;

    /**
     * @brief Updates the agent’s perception of whether an enemy is visible.
     *
     * @param bVisible True if the enemy is visible, false otherwise.
     */
    UFUNCTION(BlueprintCallable)
    void SetEnemyVisible(bool bVisible);

    /**
     * @brief The minimum delay between GOAP replans.
     *
     * Helps prevent excessive replanning and improves performance.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
    float MinReactionTime = 0.05f;

    /**
     * @brief The maximum random delay before the agent reacts to changes in the world state.
     *
     * Adds variability and realism to AI reactions and prevents synchronized replanning among agents.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
    float MaxReactionTime = 0.25f;

    /**
     * @brief Countdown timer controlling when the agent can plan again.
     */
    float ReactionTimer = 0.f;

    /**
     * @brief Whether a replan has been requested.
     */
    bool bRequestReplan = false;

    /**
     * @brief Controls how much debugging information is logged or displayed.
     *
     * Can be adjusted per agent for visibility or performance considerations.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP|Debug")
    EGOAPDebugLevel DebugLevel = EGOAPDebugLevel::Minimal;
};
