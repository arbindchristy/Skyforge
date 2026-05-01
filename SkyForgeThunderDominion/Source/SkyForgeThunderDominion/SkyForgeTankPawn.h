#pragma once

#include "CoreMinimal.h"
#include "SkyForgeUnitPawn.h"
#include "SkyForgeTankPawn.generated.h"

UCLASS()
class SKYFORGETHUNDERDOMINION_API ASkyForgeTankPawn : public ASkyForgeUnitPawn
{
    GENERATED_BODY()

public:
    ASkyForgeTankPawn();
    virtual void PrimaryFire() override;
    virtual FVector GetProjectileSpawnLocation() const override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkyForge|Tank")
    float CannonReloadSeconds = 8.0f;
};
