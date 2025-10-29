#include "GOAPWorldStateComponent.h"
#include "GOAPDebug.h"
#include "GOAPAgent.h" 

// Set up for categorizing debug information
#define GOAP_WORLDSTATE_LOG(Component, Level, Format, ...) \
    if (Component && Component->OwningAgent && Component->OwningAgent->DebugLevel >= Level) \
    { \
        UE_LOG(LogTemp, Warning, TEXT(Format), ##__VA_ARGS__); \
    }

UGOAPWorldStateComponent::UGOAPWorldStateComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

FString UGOAPWorldStateComponent::GetStateAsString() const
{
    FString Out = "WorldState: ";
    for (const auto& Pair : CurrentState.Bools)
    {
        Out += FString::Printf(TEXT("%s=%s "),
            *Pair.Key.ToString(),
            Pair.Value ? TEXT("true") : TEXT("false"));
    }
    return Out;
}

void UGOAPWorldStateComponent::Apply(const TMap<FName, bool>& Effects)
{
    bool bChanged = false; // track if anything actually changed

    for (const auto& E : Effects)
    {
        bool* ExistingValue = CurrentState.Bools.Find(E.Key);

        if (ExistingValue)
        {
            if (*ExistingValue != E.Value)
            {
                GOAP_WORLDSTATE_LOG(this, EGOAPDebugLevel::Minimal, "WorldState changed: %s = %s",
                    *E.Key.ToString(), E.Value ? TEXT("true") : TEXT("false"));
                *ExistingValue = E.Value;
                bChanged = true;
            }
        }
        else
        {
            CurrentState.Bools.Add(E.Key, E.Value);
            GOAP_WORLDSTATE_LOG(this, EGOAPDebugLevel::Minimal, "WorldState added: %s = %s",
                *E.Key.ToString(), E.Value ? TEXT("true") : TEXT("false"));
            bChanged = true;
        }
    }

    if (bChanged)
    {
        OnWorldStateChanged.Broadcast(); // notify the agent that the state has changed
    }
}

