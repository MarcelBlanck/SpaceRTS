// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SelectableObject.generated.h"

UENUM(BlueprintType)
enum class ESelectableObjectType : uint8
{
	Prop						UMETA(DisplayName = "Prop"),
	UI							UMETA(DisplayName = "UI"),
	Resource					UMETA(DisplayName = "Resource"),
	EnemySpaceship				UMETA(DisplayName = "EnemySpaceship"),
	PlayerControlledSpaceship	UMETA(DisplayName = "PlayerControlledSpaceship")
};

UINTERFACE(BlueprintType)
class USelectableObject : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class ISelectableObject
{
	GENERATED_IINTERFACE_BODY()

	virtual ESelectableObjectType GetType();

	virtual void Select();

	virtual void Deselect();

	virtual void GazeBegin();
	
	virtual void GazeEnd();
};
