#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkyForgeSoulSwitchComponent.generated.h"

class ASkyForgePlayerController;
class ASkyForgeUnitPawn;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSkyForgeSoulSwitchCooldownChanged, float, CooldownPercent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSkyForgeSoulSwitchReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSkyForgeSoulSwitchCompleted, ASkyForgeUnitPawn*, NewUnit);

UCLASS(ClassGroup = (SkyForge), meta = (BlueprintSpawnableComponent))
class SKYFORGETHUNDERDOMINION_API USkyForgeSoulSwitchComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USkyForgeSoulSwitchComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "SkyForge|Soul Switch")
    bool RequestSoulSwitch(ASkyForgeUnitPawn* TargetUnit);

    UFUNCTION(BlueprintPure, Category = "SkyForge|Soul Switch")
    bool IsSwitchReady() const;

    UFUNCTION(BlueprintPure, Category = "SkyForge|Soul Switch")
    float GetCooldownPercent() const;

    UPROPERTY(BlueprintAssignable, Category = "SkyForge|Soul Switch")
    FSkyForgeSoulSwitchCooldownChanged OnCooldownChanged;

    UPROPERTY(BlueprintAssignable, Category = "SkyForge|Soul Switch")
    FSkyForgeSoulSwitchReady OnSwitchReady;

    UPROPERTY(BlueprintAssignable, Category = "SkyForge|Soul Switch")
    FSkyForgeSoulSwitchCompleted OnSwitchCompleted;

private:
    UFUNCTION()
    void FinishPendingSwitch();

    UFUNCTION()
    void ResetCooldown();

    UPROPERTY(EditDefaultsOnly, Category = "SkyForge|Soul Switch")
    float SwitchCooldown = 8.0f;

    UPROPERTY(EditDefaultsOnly, Category = "SkyForge|Soul Switch")
    float DiveDuration = 0.45f;

    UPROPERTY(EditDefaultsOnly, Category = "SkyForge|Soul Switch")
    float DiveTimeDilation = 0.35f;

    UPROPERTY(VisibleAnywhere, Category = "SkyForge|Soul Switch")
    bool bSwitchReady = true;

    UPROPERTY(VisibleAnywhere, Category = "SkyForge|Soul Switch")
    float LastSwitchTimestamp = -1000.0f;

    UPROPERTY()
    TObjectPtr<ASkyForgePlayerController> OwningPlayerController;

    UPROPERTY()
    TObjectPtr<ASkyForgeUnitPawn> OldUnit;

    UPROPERTY()
    TObjectPtr<ASkyForgeUnitPawn> PendingTargetUnit;

    FTimerHandle SwitchFinishTimer;
    FTimerHandle CooldownTimer;
};
