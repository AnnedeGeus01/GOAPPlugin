#include "GOAPAIController.h"
#include "GOAPWorldStateComponent.h"
#include "GameFramework/Pawn.h"

AGOAPAIController::AGOAPAIController()
{
    
}

void AGOAPAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (UGOAPWorldStateComponent* WS = InPawn->FindComponentByClass<UGOAPWorldStateComponent>())
    {
        UE_LOG(LogTemp, Warning, TEXT("%s"), *WS->GetStateAsString());
    }
}
