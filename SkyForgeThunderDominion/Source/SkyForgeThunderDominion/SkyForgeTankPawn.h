#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SkyForgeTankPawn.generated.h"

class UCapsuleComponent;
class UFloatingPawnMovement;
class UStaticMeshComponent;

UCLASS()
class SKYFORGETHUNDERDOMINION_API ASkyForgeTankPawn : public APawn
{
    GENERATED_BODY()

public:
    ASkyForgeTankPawn();
    virtual UPawnMovementComponent* GetMovementComponent() const override;

protected:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    void MoveForward(float Value);
    void TurnRight(float Value);

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UCapsuleComponent> CollisionComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> TankMeshComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UFloatingPawnMovement> MovementComponent;
};
