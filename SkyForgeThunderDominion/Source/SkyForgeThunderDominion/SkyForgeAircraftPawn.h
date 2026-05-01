#pragma once

#include "CoreMinimal.h"
#include "SkyForgeUnitPawn.h"
#include "SkyForgeAircraftPawn.generated.h"

UCLASS()
class SKYFORGETHUNDERDOMINION_API ASkyForgeAircraftPawn : public ASkyForgeUnitPawn
{
    GENERATED_BODY()

public:
    ASkyForgeAircraftPawn();
    virtual void Tick(float DeltaSeconds) override;
    virtual void SecondaryAction() override;

protected:
    virtual void MoveForward(float Value) override;
    virtual void MoveRight(float Value) override;
    virtual void TurnRight(float Value) override;
    virtual void LookUp(float Value) override;

    UFUNCTION()
    void EndAfterburner();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkyForge|Aircraft")
    float CruiseSpeed = 4200.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkyForge|Aircraft")
    float AfterburnerMultiplier = 1.8f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkyForge|Aircraft")
    float AfterburnerDuration = 2.5f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkyForge|Aircraft")
    float PitchRate = 60.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkyForge|Aircraft")
    float YawRate = 75.0f;

private:
    FTimerHandle AfterburnerTimer;
};
