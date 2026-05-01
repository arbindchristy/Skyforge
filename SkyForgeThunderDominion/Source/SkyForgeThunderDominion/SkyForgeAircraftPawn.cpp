#include "SkyForgeAircraftPawn.h"

#include "GameFramework/FloatingPawnMovement.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

ASkyForgeAircraftPawn::ASkyForgeAircraftPawn()
{
    UnitClassTag = TEXT("Unit.Air");
    MaxHealth = 1600.0f;
    Health = MaxHealth;
    FireCooldown = 1.2f;
    MovementComponent->MaxSpeed = CruiseSpeed;
    MovementComponent->Acceleration = 9000.0f;
    MovementComponent->Deceleration = 1200.0f;
    MovementComponent->TurningBoost = 2.0f;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMesh(TEXT("/Engine/BasicShapes/Cone.Cone"));
    if (ConeMesh.Succeeded())
    {
        UnitMeshComponent->SetStaticMesh(ConeMesh.Object);
        UnitMeshComponent->SetRelativeScale3D(FVector(1.0f, 2.2f, 0.35f));
    }
}

void ASkyForgeAircraftPawn::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (Controller && IsAlive())
    {
        AddMovementInput(GetActorForwardVector(), 0.35f);
    }
}

void ASkyForgeAircraftPawn::SecondaryAction()
{
    MovementComponent->MaxSpeed = CruiseSpeed * AfterburnerMultiplier;
    GetWorldTimerManager().SetTimer(
        AfterburnerTimer,
        this,
        &ASkyForgeAircraftPawn::EndAfterburner,
        AfterburnerDuration,
        false);
}

void ASkyForgeAircraftPawn::MoveForward(float Value)
{
    AddMovementInput(GetActorForwardVector(), FMath::Clamp(Value, -0.2f, 1.0f));
}

void ASkyForgeAircraftPawn::MoveRight(float Value)
{
    if (!FMath::IsNearlyZero(Value))
    {
        AddActorLocalRotation(FRotator(0.0f, Value * YawRate * GetWorld()->GetDeltaSeconds(), 0.0f));
    }
}

void ASkyForgeAircraftPawn::TurnRight(float Value)
{
    MoveRight(Value);
}

void ASkyForgeAircraftPawn::LookUp(float Value)
{
    if (!FMath::IsNearlyZero(Value))
    {
        AddActorLocalRotation(FRotator(Value * PitchRate * GetWorld()->GetDeltaSeconds(), 0.0f, 0.0f));
    }
}

void ASkyForgeAircraftPawn::EndAfterburner()
{
    MovementComponent->MaxSpeed = CruiseSpeed;
}
