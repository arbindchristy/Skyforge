#include "SkyForgeGameMode.h"

#include "SkyForgePlayerController.h"
#include "SkyForgeTankPawn.h"

ASkyForgeGameMode::ASkyForgeGameMode()
{
    DefaultPawnClass = ASkyForgeTankPawn::StaticClass();
    PlayerControllerClass = ASkyForgePlayerController::StaticClass();
}
