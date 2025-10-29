#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GOAPAnimInstance.generated.h"

UCLASS()
class GOAP_API UGOAPAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    // This will drive whether the character holds a weapon
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon")
    bool bHasWeapon = false;
    // This will drive whether the character walks
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
    bool bIsWalking = false;
};
