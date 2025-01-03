// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetDevWorldGameMode.h"
#include "AssetDevWorldCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAssetDevWorldGameMode::AAssetDevWorldGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
