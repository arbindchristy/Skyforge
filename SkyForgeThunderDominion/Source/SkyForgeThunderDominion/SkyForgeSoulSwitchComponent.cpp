#include "SkyForgeSoulSwitchComponent.h"

#include "Kismet/GameplayStatics.h"
#include "SkyForgePlayerController.h"
#include "SkyForgeUnitPawn.h"
#include "TimerManager.h"

USkyForgeSoulSwitchComponent::USkyForgeSoulSwitchComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void USkyForgeSoulSwitchComponent::BeginPlay()
{
    Super::BeginPlay();
    OwningPlayerController = Cast<ASkyForgePlayerController>(GetOwner());
}

void USkyForgeSoulSwitchComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bSwitchReady)
    {
        OnCooldownChanged.Broadcast(GetCooldownPercent());
    }
}

bool USkyForgeSoulSwitchComponent::RequestSoulSwitch(ASkyForgeUnitPawn* TargetUnit)
{
    if (!bSwitchReady || !TargetUnit || !TargetUnit->CanBeSoulSwitched())
    {
        return false;
    }

    if (!OwningPlayerController)
    {
        OwningPlayerController = Cast<ASkyForgePlayerController>(GetOwner());
    }

    if (!OwningPlayerController)
    {
        return false;
    }

    OldUnit = Cast<ASkyForgeUnitPawn>(OwningPlayerController->GetPawn());
    if (OldUnit == TargetUnit)
    {
        return false;
    }

    PendingTargetUnit = TargetUnit;
    bSwitchReady = false;

    UGameplayStatics::SetGlobalTimeDilation(this, DiveTimeDilation);
    OwningPlayerController->SetViewTargetWithBlend(TargetUnit, DiveDuration, VTBlend_Cubic);

    GetWorld()->GetTimerManager().SetTimer(
        SwitchFinishTimer,
        this,
        &USkyForgeSoulSwitchComponent::FinishPendingSwitch,
        FMath::Max(0.01f, DiveDuration * DiveTimeDilation),
        false);

    return true;
}

bool USkyForgeSoulSwitchComponent::IsSwitchReady() const
{
    return bSwitchReady;
}

float USkyForgeSoulSwitchComponent::GetCooldownPercent() const
{
    if (bSwitchReady)
    {
        return 1.0f;
    }

    const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : LastSwitchTimestamp;
    return FMath::Clamp((Now - LastSwitchTimestamp) / SwitchCooldown, 0.0f, 1.0f);
}

void USkyForgeSoulSwitchComponent::FinishPendingSwitch()
{
    UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);

    if (!OwningPlayerController || !PendingTargetUnit || !PendingTargetUnit->CanBeSoulSwitched())
    {
        ResetCooldown();
        return;
    }

    OwningPlayerController->Possess(PendingTargetUnit);

    if (OldUnit && OldUnit != PendingTargetUnit)
    {
        OldUnit->OnReleasedToAI();
    }

    PendingTargetUnit->OnPossessedByPlayer();
    OwningPlayerController->SetCurrentControlledUnit(PendingTargetUnit);
    OwningPlayerController->SetViewTarget(PendingTargetUnit);

    LastSwitchTimestamp = GetWorld()->GetTimeSeconds();
    OnSwitchCompleted.Broadcast(PendingTargetUnit);

    GetWorld()->GetTimerManager().SetTimer(
        CooldownTimer,
        this,
        &USkyForgeSoulSwitchComponent::ResetCooldown,
        SwitchCooldown,
        false);
}

void USkyForgeSoulSwitchComponent::ResetCooldown()
{
    bSwitchReady = true;
    PendingTargetUnit = nullptr;
    OldUnit = nullptr;
    OnCooldownChanged.Broadcast(1.0f);
    OnSwitchReady.Broadcast();
}
