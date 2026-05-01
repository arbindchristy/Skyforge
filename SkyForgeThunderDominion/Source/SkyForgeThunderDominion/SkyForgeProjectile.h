#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkyForgeProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class SKYFORGETHUNDERDOMINION_API ASkyForgeProjectile : public AActor
{
    GENERATED_BODY()

public:
    ASkyForgeProjectile();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void HandleImpact(
        UPrimitiveComponent* HitComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        FVector NormalImpulse,
        const FHitResult& Hit);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USphereComponent> CollisionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkyForge|Combat")
    float Damage = 250.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkyForge|Combat")
    float BlastRadius = 250.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkyForge|Combat")
    float LifeSeconds = 8.0f;
};
