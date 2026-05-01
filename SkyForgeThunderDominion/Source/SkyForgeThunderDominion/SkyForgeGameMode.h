#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SkyForgeGameMode.generated.h"

UCLASS()
class SKYFORGETHUNDERDOMINION_API ASkyForgeGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ASkyForgeGameMode();

    virtual void StartPlay() override;

private:
    void SpawnPrototypeUnitsIfNeeded(class ASkyForgePlayerController* PlayerController);
};
