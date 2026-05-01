#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SkyForgePlayerController.generated.h"

class ASkyForgeUnitPawn;
class USkyForgeSoulSwitchComponent;

UCLASS()
class SKYFORGETHUNDERDOMINION_API ASkyForgePlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ASkyForgePlayerController();

    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    UFUNCTION(BlueprintCallable, Category = "SkyForge|Units")
    void RegisterFriendlyUnit(ASkyForgeUnitPawn* Unit);

    UFUNCTION(BlueprintCallable, Category = "SkyForge|Units")
    void SetCurrentControlledUnit(ASkyForgeUnitPawn* Unit);

    UFUNCTION(BlueprintPure, Category = "SkyForge|Units")
    ASkyForgeUnitPawn* GetCurrentControlledUnit() const;

    UFUNCTION(BlueprintPure, Category = "SkyForge|Units")
    TArray<ASkyForgeUnitPawn*> GetFriendlyUnits() const;

    UFUNCTION(BlueprintCallable, Category = "SkyForge|Command")
    void SelectUnitUnderCursor();

    UFUNCTION(BlueprintCallable, Category = "SkyForge|Soul Switch")
    void SwitchToSelectedUnit();

    UFUNCTION(BlueprintCallable, Category = "SkyForge|Soul Switch")
    void SelectNextFriendlyUnit();

    UFUNCTION(BlueprintPure, Category = "SkyForge|Soul Switch")
    USkyForgeSoulSwitchComponent* GetSoulSwitchComponent() const;

private:
    void ToggleCommandView();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USkyForgeSoulSwitchComponent> SoulSwitchComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkyForge|Units", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ASkyForgeUnitPawn> CurrentControlledUnit;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkyForge|Units", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ASkyForgeUnitPawn> SelectedUnit;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkyForge|Units", meta = (AllowPrivateAccess = "true"))
    TArray<ASkyForgeUnitPawn*> FriendlyUnits;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkyForge|Command", meta = (AllowPrivateAccess = "true"))
    bool bInCommandView = false;
};
