#pragma once

#include "CoreMinimal.h"
#include "GOAPDebug.generated.h"

/// \file GOAPDebug.h

/**
 * @brief Specifies the level of debugging for GOAP debug logging.
 *
 * Defines how much information should be logged or displayed during GOAP planning and execution. 
 */
UENUM(BlueprintType)
enum class EGOAPDebugLevel : uint8
{
    /** No logging or debug output. */
    None UMETA(DisplayName = "None"),

    /** Only essential or key events are logged (e.g., replans, action completions). */
    Minimal UMETA(DisplayName = "Minimal"),

    /** Detailed logs including planner steps, world state changes, and internal reasoning. */
    Detailed UMETA(DisplayName = "Detailed")
};
