#include "SkyForgeGameMode.h"

#include "SkyForgePlayerController.h"
#include "SkyForgeAircraftPawn.h"
#include "SkyForgeTankPawn.h"
#include "SkyForgeUnitPawn.h"
#include "Kismet/GameplayStatics.h"

ASkyForgeGameMode::ASkyForgeGameMode()
{
    DefaultPawnClass = ASkyForgeTankPawn::StaticClass();
    PlayerControllerClass = ASkyForgePlayerController::StaticClass();
}

void ASkyForgeGameMode::StartPlay()
{
    Super::StartPlay();

    ASkyForgePlayerController* PlayerController = Cast<ASkyForgePlayerController>(UGameplayStatics::GetPlayerController(this, 0));
    if (!PlayerController)
    {
        return;
    }

    SpawnPrototypeUnitsIfNeeded(PlayerController);

    TArray<AActor*> Units;
    UGameplayStatics::GetAllActorsOfClass(this, ASkyForgeUnitPawn::StaticClass(), Units);

    ASkyForgeUnitPawn* FirstFriendlyUnit = nullptr;
    for (AActor* Actor : Units)
    {
        ASkyForgeUnitPawn* Unit = Cast<ASkyForgeUnitPawn>(Actor);
        if (!Unit || !Unit->IsFriendly())
        {
            continue;
        }

        PlayerController->RegisterFriendlyUnit(Unit);
        if (!FirstFriendlyUnit)
        {
            FirstFriendlyUnit = Unit;
        }
    }

    ASkyForgeUnitPawn* CurrentPawn = Cast<ASkyForgeUnitPawn>(PlayerController->GetPawn());
    if (!CurrentPawn && FirstFriendlyUnit)
    {
        PlayerController->Possess(FirstFriendlyUnit);
        FirstFriendlyUnit->OnPossessedByPlayer();
        PlayerController->SetCurrentControlledUnit(FirstFriendlyUnit);
    }
    else if (CurrentPawn)
    {
        PlayerController->SetCurrentControlledUnit(CurrentPawn);
    }
}

void ASkyForgeGameMode::SpawnPrototypeUnitsIfNeeded(ASkyForgePlayerController* PlayerController)
{
    TArray<AActor*> ExistingUnits;
    UGameplayStatics::GetAllActorsOfClass(this, ASkyForgeUnitPawn::StaticClass(), ExistingUnits);

    int32 FriendlyCount = 0;
    int32 EnemyCount = 0;
    for (AActor* Actor : ExistingUnits)
    {
        ASkyForgeUnitPawn* Unit = Cast<ASkyForgeUnitPawn>(Actor);
        if (!Unit)
        {
            continue;
        }

        if (Unit->IsFriendly())
        {
            ++FriendlyCount;
        }
        else
        {
            ++EnemyCount;
        }
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    const FVector PlayerLocation = PlayerController && PlayerController->GetPawn()
        ? PlayerController->GetPawn()->GetActorLocation()
        : FVector::ZeroVector;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    if (FriendlyCount < 2)
    {
        World->SpawnActor<ASkyForgeAircraftPawn>(
            ASkyForgeAircraftPawn::StaticClass(),
            PlayerLocation + FVector(650.0f, 0.0f, 500.0f),
            FRotator::ZeroRotator,
            SpawnParams);
    }

    if (FriendlyCount < 3)
    {
        World->SpawnActor<ASkyForgeTankPawn>(
            ASkyForgeTankPawn::StaticClass(),
            PlayerLocation + FVector(0.0f, 550.0f, 60.0f),
            FRotator::ZeroRotator,
            SpawnParams);
    }

    if (EnemyCount < 1)
    {
        ASkyForgeTankPawn* EnemyTank = World->SpawnActor<ASkyForgeTankPawn>(
            ASkyForgeTankPawn::StaticClass(),
            PlayerLocation + FVector(2400.0f, 0.0f, 60.0f),
            FRotator(0.0f, 180.0f, 0.0f),
            SpawnParams);

        if (EnemyTank)
        {
            EnemyTank->SetFactionTag(TEXT("Faction.Enemy"));
            EnemyTank->OnReleasedToAI();
        }
    }
}
