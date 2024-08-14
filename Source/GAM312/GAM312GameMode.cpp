// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAM312GameMode.h"
#include "GAM312Character.h"
#include "UObject/ConstructorHelpers.h"

AGAM312GameMode::AGAM312GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
