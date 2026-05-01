#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SkyForgeUnitPawn.generated.h"

class UCapsuleComponent;
class UFloatingPawnMovement;
class UStaticMeshComponent;
class UWidgetComponent;
class AAIController;

UCLASS()
class SKYFORGETHUNDERDOMINION_API ASkyForgeUnitPawn : public APawn
{
    GENERATED_BODY()

public:
    ASkyForgeUnitPawn();

    virtual void Tick(float DeltaSeconds) override;
    virtual UPawnMovementComponent* GetMovementComponent() const override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    UFUNCTION(BlueprintCallable, Category = "SkyForge|Soul Switch")
    bool CanBeSoulSwitched() const;

    UFUNCTION(BlueprintCallable, Category = "SkyForge|Soul Switch")
    virtual void OnPossessedByPlayer();

    UFUNCTION(BlueprintCallable, Category = "SkyForge|Soul Switch")
    virtual void OnReleasedToAI();

    UFUNCTION(BlueprintCallable, Category = "SkyForge|Combat")
    virtual void ApplySkyForgeDamage(float DamageAmount);

    UFUNCTION(BlueprintCallable, Category = "SkyForge|Combat")
    bool IsAlive() const;

    UFUNCTION(BlueprintCallable, Category = "SkyForge|Combat")
    bool IsFriendly() const;

    UFUNCTION(BlueprintCallable, Category = "SkyForge|Combat")
    virtual FVector GetProjectileSpawnLocation() const;

    UFUNCTION(BlueprintCallable, Category = "SkyForge|Combat")
    virtual void PrimaryFire();

    UFUNCTION(BlueprintCallable, Category = "SkyForge|Combat")
    virtual void SecondaryAction();

    UFUNCTION(BlueprintCallable, Category = "SkyForge|AI")
    void SetAITarget(AActor* NewTarget);

    UFUNCTION(BlueprintPure, Category = "SkyForge|AI")
    AActor* GetAITarget() const;

    UFUNCTION(BlueprintPure, Category = "SkyForge|Unit")
    FName GetFactionTag() const;

    UFUNCTION(BlueprintCallable, Category = "SkyForge|Unit")
    void SetFactionTag(FName NewFactionTag);

    UFUNCTION(BlueprintPure, Category = "SkyForge|Unit")
    FName GetUnitClassTag() const;

protected:
    virtual void BeginPlay() override;

    virtual void MoveForward(float Value);
    virtual void MoveRight(float Value);
    virtual void TurnRight(float Value);
    virtual void LookUp(float Value);

    UFUNCTION()
    void ClearInvulnerability();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UCapsuleComponent> CollisionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> UnitMeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UFloatingPawnMovement> MovementComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UWidgetComponent> WorldBadgeComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkyForge|Unit")
    FName FactionTag = TEXT("Faction.Friendly");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkyForge|Unit")
    FName UnitClassTag = TEXT("Unit.Ground");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkyForge|Combat", meta = (ClampMin = "1.0"))
    float MaxHealth = 1000.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkyForge|Combat")
    float Health = 1000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkyForge|Combat")
    TSubclassOf<AActor> ProjectileClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkyForge|Combat")
    float FireCooldown = 1.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkyForge|Combat")
    bool bCanFire = true;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkyForge|Soul Switch")
    bool bIsPlayerControlled = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkyForge|Soul Switch")
    bool bInvulnerable = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkyForge|Soul Switch")
    float SwitchInvulnerabilitySeconds = 0.4f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkyForge|AI")
    TSubclassOf<AAIController> AssignedAIControllerClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkyForge|AI")
    TObjectPtr<AActor> AITarget;

private:
    FTimerHandle FireCooldownTimer;
    FTimerHandle InvulnerabilityTimer;
};
