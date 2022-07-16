// Copyright Epic Games, Inc. All Rights Reserved.

#include "StealthTopDownGameMode.h"
#include "StealthTopDownPlayerController.h"
#include "StealthTopDownCharacter.h"
#include "UObject/ConstructorHelpers.h"

AStealthTopDownGameMode::AStealthTopDownGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AStealthTopDownPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}