// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ShaderPluginDemo.h"
#include "ShaderPluginDemoGameMode.h"
#include "ShaderPluginDemoHUD.h"
#include "ShaderPluginDemoCharacter.h"

AShaderPluginDemoGameMode::AShaderPluginDemoGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AShaderPluginDemoHUD::StaticClass();
}
