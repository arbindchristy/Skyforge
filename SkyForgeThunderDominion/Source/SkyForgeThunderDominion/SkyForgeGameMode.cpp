#include "SkyForgeGameMode.h"

#include "SkyForgePlayerController.h"

ASkyForgeGameMode::ASkyForgeGameMode()
{
    DefaultPawnClass = nullptr;
    PlayerControllerClass = ASkyForgePlayerController::StaticClass();
}
