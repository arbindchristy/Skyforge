#include "SkyForgeTankPawn.h"

#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/FloatingPawnMovement.h"

ASkyForgeTankPawn::ASkyForgeTankPawn()
{
    PrimaryActorTick.bCanEverTick = false;
    AutoPossessPlayer = EAutoReceiveInput::Player0;

    CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));
    CollisionComponent->InitCapsuleSize(60.0f, 40.0f);
    CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
    RootComponent = CollisionComponent;

    TankMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TankMeshComponent"));
    TankMeshComponent->SetupAttachment(RootComponent);
    TankMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
    MovementComponent->UpdatedComponent = RootComponent;
    MovementComponent->MaxSpeed = 1200.0f;
    MovementComponent->Acceleration = 2400.0f;
    MovementComponent->Deceleration = 3200.0f;
    MovementComponent->TurningBoost = 8.0f;

    bUseControllerRotationYaw = true;
}

UPawnMovementComponent* ASkyForgeTankPawn::GetMovementComponent() const
{
    return MovementComponent;
}

void ASkyForgeTankPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    check(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ASkyForgeTankPawn::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("TurnRight"), this, &ASkyForgeTankPawn::TurnRight);
}

void ASkyForgeTankPawn::MoveForward(float Value)
{
    if (FMath::IsNearlyZero(Value))
    {
        return;
    }

    AddMovementInput(GetActorForwardVector(), Value);
}

void ASkyForgeTankPawn::TurnRight(float Value)
{
    if (FMath::IsNearlyZero(Value))
    {
        return;
    }

    AddControllerYawInput(Value);
}
