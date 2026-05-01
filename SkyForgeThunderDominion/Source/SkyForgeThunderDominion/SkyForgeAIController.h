#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SkyForgeAIController.generated.h"

UCLASS()
class SKYFORGETHUNDERDOMINION_API ASkyForgeAIController : public AAIController
{
    GENERATED_BODY()

public:
    ASkyForgeAIController();
    virtual void Tick(float DeltaSeconds) override;

protected:
    virtual void OnPossess(APawn* InPawn) override;

private:
    void AcquireTarget();

    UPROPERTY()
    TObjectPtr<class ASkyForgeUnitPawn> ControlledUnit;

    UPROPERTY()
    TObjectPtr<AActor> CurrentTarget;

    float FireRange = 2800.0f;
};
