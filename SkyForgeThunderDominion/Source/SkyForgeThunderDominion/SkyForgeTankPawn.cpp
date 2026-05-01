#include "SkyForgeTankPawn.h"

#include "GameFramework/FloatingPawnMovement.h"

ASkyForgeTankPawn::ASkyForgeTankPawn()
{
    UnitClassTag = TEXT("Unit.Ground");
    MaxHealth = 3800.0f;
    Health = MaxHealth;
    FireCooldown = CannonReloadSeconds;
    MovementComponent->MaxSpeed = 1200.0f;
    MovementComponent->Acceleration = 2400.0f;
    MovementComponent->Deceleration = 3200.0f;
    MovementComponent->TurningBoost = 8.0f;
}

void ASkyForgeTankPawn::PrimaryFire()
{
    FireCooldown = CannonReloadSeconds;
    ASkyForgeUnitPawn::PrimaryFire();
}

FVector ASkyForgeTankPawn::GetProjectileSpawnLocation() const
{
    return GetActorLocation() + GetActorForwardVector() * 220.0f + FVector(0.0f, 0.0f, 70.0f);
}
