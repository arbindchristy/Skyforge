#include "SkyForgeAIController.h"

#include "Kismet/GameplayStatics.h"
#include "SkyForgeUnitPawn.h"

ASkyForgeAIController::ASkyForgeAIController()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASkyForgeAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    ControlledUnit = Cast<ASkyForgeUnitPawn>(InPawn);
    AcquireTarget();
}

void ASkyForgeAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!ControlledUnit || !ControlledUnit->IsAlive())
    {
        return;
    }

    if (!CurrentTarget || IsValid(CurrentTarget) == false)
    {
        AcquireTarget();
    }

    if (!CurrentTarget)
    {
        return;
    }

    const float Distance = FVector::Dist(ControlledUnit->GetActorLocation(), CurrentTarget->GetActorLocation());
    if (Distance > FireRange)
    {
        MoveToActor(CurrentTarget, FireRange * 0.65f);
    }
    else
    {
        StopMovement();
        const FVector Direction = (CurrentTarget->GetActorLocation() - ControlledUnit->GetActorLocation()).GetSafeNormal();
        ControlledUnit->SetActorRotation(Direction.Rotation());
        ControlledUnit->PrimaryFire();
    }
}

void ASkyForgeAIController::AcquireTarget()
{
    CurrentTarget = nullptr;

    TArray<AActor*> Units;
    UGameplayStatics::GetAllActorsOfClass(this, ASkyForgeUnitPawn::StaticClass(), Units);

    float BestDistanceSquared = TNumericLimits<float>::Max();
    for (AActor* Actor : Units)
    {
        ASkyForgeUnitPawn* Candidate = Cast<ASkyForgeUnitPawn>(Actor);
        if (!Candidate || Candidate == ControlledUnit || !Candidate->IsAlive())
        {
            continue;
        }

        if (ControlledUnit && Candidate->GetFactionTag() == ControlledUnit->GetFactionTag())
        {
            continue;
        }

        const float DistanceSquared = FVector::DistSquared(ControlledUnit->GetActorLocation(), Candidate->GetActorLocation());
        if (DistanceSquared < BestDistanceSquared)
        {
            BestDistanceSquared = DistanceSquared;
            CurrentTarget = Candidate;
        }
    }

    if (ControlledUnit)
    {
        ControlledUnit->SetAITarget(CurrentTarget);
    }
}
