// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SelectableObject.h"
#include "PlayerControlledSpaceship.generated.h"

UINTERFACE(BlueprintType)
class UPlayerControlledSpaceship : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

UCLASS(BlueprintType)
class UPlayerControlledSpaceshipBPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = "Player Controlled Spaceship")
	static void AttackObject(AActor *Actor, const TScriptInterface<ISelectableObject>& Object);

	UFUNCTION(BlueprintCallable, Category = "Player Controlled Spaceship")
	static void InteractWithObject(AActor *Actor, const TScriptInterface<ISelectableObject>& Object);

	UFUNCTION(BlueprintCallable, Category = "Player Controlled Spaceship")
	static void SteerToLocation(AActor *Actor, FVector Location);
};

class IPlayerControlledSpaceship 
{
	GENERATED_IINTERFACE_BODY()

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Attack Object"), Category = "Player Controlled Spaceship")
	void AttackObject(const TScriptInterface<ISelectableObject>& Object);
	virtual void ExecuteNative_AttackObject(const TScriptInterface<ISelectableObject>& Object);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Interact With Object"), Category = "Player Controlled Spaceship")
	void InteractWithObject(const TScriptInterface<ISelectableObject>& Object);
	virtual void ExecuteNative_InteractWithObject(const TScriptInterface<ISelectableObject>& Object);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Steer To Location"), Category = "Player Controlled Spaceship")
	void SteerToLocation(FVector &Location);
	virtual void ExecuteNative_SteerToLocation(FVector Location);
};