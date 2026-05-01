#include "SkyForgePlayerController.h"

#include "SkyForgeSoulSwitchComponent.h"
#include "SkyForgeUnitPawn.h"

ASkyForgePlayerController::ASkyForgePlayerController()
{
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Crosshairs;
    SoulSwitchComponent = CreateDefaultSubobject<USkyForgeSoulSwitchComponent>(TEXT("SoulSwitchComponent"));
}

void ASkyForgePlayerController::BeginPlay()
{
    Super::BeginPlay();

    CurrentControlledUnit = Cast<ASkyForgeUnitPawn>(GetPawn());
    if (CurrentControlledUnit)
    {
        CurrentControlledUnit->OnPossessedByPlayer();
        RegisterFriendlyUnit(CurrentControlledUnit);
    }
}

void ASkyForgePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    check(InputComponent);
    InputComponent->BindAction(TEXT("SelectUnit"), IE_Pressed, this, &ASkyForgePlayerController::SelectUnitUnderCursor);
    InputComponent->BindAction(TEXT("SoulSwitch"), IE_Pressed, this, &ASkyForgePlayerController::SwitchToSelectedUnit);
    InputComponent->BindAction(TEXT("SelectNextUnit"), IE_Pressed, this, &ASkyForgePlayerController::SelectNextFriendlyUnit);
    InputComponent->BindAction(TEXT("ToggleCommandView"), IE_Pressed, this, &ASkyForgePlayerController::ToggleCommandView);
}

void ASkyForgePlayerController::RegisterFriendlyUnit(ASkyForgeUnitPawn* Unit)
{
    if (Unit && Unit->IsFriendly())
    {
        FriendlyUnits.AddUnique(Unit);
    }
}

void ASkyForgePlayerController::SetCurrentControlledUnit(ASkyForgeUnitPawn* Unit)
{
    CurrentControlledUnit = Unit;
    if (Unit)
    {
        RegisterFriendlyUnit(Unit);
    }
}

ASkyForgeUnitPawn* ASkyForgePlayerController::GetCurrentControlledUnit() const
{
    return CurrentControlledUnit;
}

TArray<ASkyForgeUnitPawn*> ASkyForgePlayerController::GetFriendlyUnits() const
{
    return FriendlyUnits;
}

void ASkyForgePlayerController::SelectUnitUnderCursor()
{
    FHitResult Hit;
    if (!GetHitResultUnderCursor(ECC_Visibility, false, Hit))
    {
        return;
    }

    ASkyForgeUnitPawn* HitUnit = Cast<ASkyForgeUnitPawn>(Hit.GetActor());
    if (HitUnit && HitUnit->CanBeSoulSwitched())
    {
        SelectedUnit = HitUnit;
        RegisterFriendlyUnit(HitUnit);
    }
}

void ASkyForgePlayerController::SwitchToSelectedUnit()
{
    if (!SelectedUnit && FriendlyUnits.Num() > 0)
    {
        SelectNextFriendlyUnit();
    }

    if (SoulSwitchComponent)
    {
        SoulSwitchComponent->RequestSoulSwitch(SelectedUnit);
    }
}

void ASkyForgePlayerController::SelectNextFriendlyUnit()
{
    FriendlyUnits.RemoveAll([](ASkyForgeUnitPawn* Unit)
    {
        return !Unit || !Unit->CanBeSoulSwitched();
    });

    if (FriendlyUnits.Num() == 0)
    {
        SelectedUnit = nullptr;
        return;
    }

    const int32 CurrentIndex = FriendlyUnits.IndexOfByKey(CurrentControlledUnit);
    const int32 StartIndex = CurrentIndex == INDEX_NONE ? 0 : CurrentIndex + 1;

    for (int32 Offset = 0; Offset < FriendlyUnits.Num(); ++Offset)
    {
        const int32 CandidateIndex = (StartIndex + Offset) % FriendlyUnits.Num();
        if (FriendlyUnits[CandidateIndex] != CurrentControlledUnit)
        {
            SelectedUnit = FriendlyUnits[CandidateIndex];
            return;
        }
    }

    SelectedUnit = CurrentControlledUnit;
}

USkyForgeSoulSwitchComponent* ASkyForgePlayerController::GetSoulSwitchComponent() const
{
    return SoulSwitchComponent;
}

void ASkyForgePlayerController::ToggleCommandView()
{
    bInCommandView = !bInCommandView;
    bShowMouseCursor = bInCommandView;
}
