// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SpaceshipAITypes.generated.h"

UENUM(BlueprintType)
enum class ESpaceshipAICommandMode : uint8
{
	Idle					UMETA(DisplayName = "Idle"),
	Move					UMETA(DisplayName = "Move"),
	Attack					UMETA(DisplayName = "Attack"),
	Defend					UMETA(DisplayName = "Defend"),
	GatherRessource			UMETA(DisplayName = "GatherRessource"),
	ReturnRessource			UMETA(DisplayName = "ReturnRessource")
};