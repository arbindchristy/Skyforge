#include "SkyForgeUnitPawn.h"

#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "SkyForgeProjectile.h"
#include "SkyForgeAIController.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

ASkyForgeUnitPawn::ASkyForgeUnitPawn()
{
    PrimaryActorTick.bCanEverTick = true;
    AutoPossessPlayer = EAutoReceiveInput::Disabled;
    AutoPossessAI = EAutoPossessAI::Disabled;

    CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));
    CollisionComponent->InitCapsuleSize(80.0f, 80.0f);
    CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
    CollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    RootComponent = CollisionComponent;

    UnitMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UnitMeshComponent"));
    UnitMeshComponent->SetupAttachment(RootComponent);
    UnitMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        UnitMeshComponent->SetStaticMesh(CubeMesh.Object);
        UnitMeshComponent->SetRelativeScale3D(FVector(1.4f, 2.0f, 0.45f));
    }

    MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
    MovementComponent->UpdatedComponent = RootComponent;
    MovementComponent->MaxSpeed = 1200.0f;
    MovementComponent->Acceleration = 2400.0f;
    MovementComponent->Deceleration = 3200.0f;
    MovementComponent->TurningBoost = 8.0f;

    WorldBadgeComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WorldBadgeComponent"));
    WorldBadgeComponent->SetupAttachment(RootComponent);
    WorldBadgeComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
    WorldBadgeComponent->SetDrawSize(FVector2D(96.0f, 32.0f));
    WorldBadgeComponent->SetWidgetSpace(EWidgetSpace::Screen);

    bUseControllerRotationYaw = true;
    AssignedAIControllerClass = ASkyForgeAIController::StaticClass();
    AIControllerClass = ASkyForgeAIController::StaticClass();
    ProjectileClass = ASkyForgeProjectile::StaticClass();
}

void ASkyForgeUnitPawn::BeginPlay()
{
    Super::BeginPlay();
    Health = MaxHealth;
}

void ASkyForgeUnitPawn::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

UPawnMovementComponent* ASkyForgeUnitPawn::GetMovementComponent() const
{
    return MovementComponent;
}

void ASkyForgeUnitPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    check(PlayerInputComponent);
    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ASkyForgeUnitPawn::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ASkyForgeUnitPawn::MoveRight);
    PlayerInputComponent->BindAxis(TEXT("TurnRight"), this, &ASkyForgeUnitPawn::TurnRight);
    PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ASkyForgeUnitPawn::LookUp);
    PlayerInputComponent->BindAction(TEXT("PrimaryFire"), IE_Pressed, this, &ASkyForgeUnitPawn::PrimaryFire);
    PlayerInputComponent->BindAction(TEXT("SecondaryAction"), IE_Pressed, this, &ASkyForgeUnitPawn::SecondaryAction);
}

float ASkyForgeUnitPawn::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    const float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    ApplySkyForgeDamage(AppliedDamage > 0.0f ? AppliedDamage : DamageAmount);
    return AppliedDamage;
}

bool ASkyForgeUnitPawn::CanBeSoulSwitched() const
{
    return IsAlive() && IsFriendly() && !IsHidden();
}

void ASkyForgeUnitPawn::OnPossessedByPlayer()
{
    bIsPlayerControlled = true;
    bInvulnerable = true;

    GetWorldTimerManager().ClearTimer(InvulnerabilityTimer);
    GetWorldTimerManager().SetTimer(
        InvulnerabilityTimer,
        this,
        &ASkyForgeUnitPawn::ClearInvulnerability,
        SwitchInvulnerabilitySeconds,
        false);
}

void ASkyForgeUnitPawn::OnReleasedToAI()
{
    bIsPlayerControlled = false;

    if (!IsAlive())
    {
        return;
    }

    if (Controller && Controller->IsPlayerController())
    {
        Controller->UnPossess();
    }

    if (!Controller)
    {
        AIControllerClass = AssignedAIControllerClass.Get() ? AssignedAIControllerClass.Get() : AIControllerClass.Get();
        SpawnDefaultController();
    }
}

void ASkyForgeUnitPawn::ApplySkyForgeDamage(float DamageAmount)
{
    if (bInvulnerable || DamageAmount <= 0.0f || !IsAlive())
    {
        return;
    }

    Health = FMath::Max(0.0f, Health - DamageAmount);
    if (!IsAlive())
    {
        SetActorEnableCollision(false);
        SetActorTickEnabled(false);
        if (Controller)
        {
            Controller->UnPossess();
        }
    }
}

bool ASkyForgeUnitPawn::IsAlive() const
{
    return Health > 0.0f;
}

bool ASkyForgeUnitPawn::IsFriendly() const
{
    return FactionTag == TEXT("Faction.Friendly");
}

FVector ASkyForgeUnitPawn::GetProjectileSpawnLocation() const
{
    return GetActorLocation() + GetActorForwardVector() * 160.0f + FVector(0.0f, 0.0f, 45.0f);
}

void ASkyForgeUnitPawn::PrimaryFire()
{
    if (!bCanFire || !ProjectileClass || !IsAlive())
    {
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = this;

    GetWorld()->SpawnActor<AActor>(
        ProjectileClass,
        GetProjectileSpawnLocation(),
        GetActorRotation(),
        SpawnParams);

    bCanFire = false;
    GetWorldTimerManager().SetTimer(
        FireCooldownTimer,
        FTimerDelegate::CreateLambda([this]()
        {
            bCanFire = true;
        }),
        FireCooldown,
        false);
}

void ASkyForgeUnitPawn::SecondaryAction()
{
}

void ASkyForgeUnitPawn::SetAITarget(AActor* NewTarget)
{
    AITarget = NewTarget;
}

AActor* ASkyForgeUnitPawn::GetAITarget() const
{
    return AITarget;
}

FName ASkyForgeUnitPawn::GetFactionTag() const
{
    return FactionTag;
}

void ASkyForgeUnitPawn::SetFactionTag(FName NewFactionTag)
{
    FactionTag = NewFactionTag;
}

FName ASkyForgeUnitPawn::GetUnitClassTag() const
{
    return UnitClassTag;
}

void ASkyForgeUnitPawn::MoveForward(float Value)
{
    if (!FMath::IsNearlyZero(Value))
    {
        AddMovementInput(GetActorForwardVector(), Value);
    }
}

void ASkyForgeUnitPawn::MoveRight(float Value)
{
    if (!FMath::IsNearlyZero(Value))
    {
        AddMovementInput(GetActorRightVector(), Value);
    }
}

void ASkyForgeUnitPawn::TurnRight(float Value)
{
    if (!FMath::IsNearlyZero(Value))
    {
        AddControllerYawInput(Value);
    }
}

void ASkyForgeUnitPawn::LookUp(float Value)
{
    if (!FMath::IsNearlyZero(Value))
    {
        AddControllerPitchInput(Value);
    }
}

void ASkyForgeUnitPawn::ClearInvulnerability()
{
    bInvulnerable = false;
}
