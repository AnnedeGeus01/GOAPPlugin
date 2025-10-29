#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GOAPAIController.generated.h"

/**
 * @brief AI Controller class for GOAP agents.
 *
 * This class integrates Unreal's AIController with the GOAP system.
 * It currently serves as a base controller for GOAP-driven characters.
 *
 * Being a Blueprintable and BlueprintType class, it can be extended in Blueprints
 * to add custom AI behavior.
 */
UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent))
class GOAP_API AGOAPAIController : public AAIController
{
    GENERATED_BODY()

public:
    /** Default constructor */
    AGOAPAIController();

protected:
    /**
     * @brief Called when this controller possesses a pawn.
     *
     * @param InPawn The pawn being possessed by this AI controller.
     */
    virtual void OnPossess(APawn* InPawn) override;
};
