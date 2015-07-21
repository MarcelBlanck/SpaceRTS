// Marcel Blanck 2015 - All rights reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GazeGuiElement.h"
#include "ExclusivSwitchArray.generated.h"

UCLASS()
class SPACERTS_API AExclusivSwitchArray : public AActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ExclusiveSwitch")
	TArray<AGazeGuiElement*> Switches;

	UFUNCTION(BlueprintCallable, Category = Updating)
	AGazeGuiElement* GetCurrentlyActiveSwitch();

	UFUNCTION(BlueprintCallable, Category = Updating)
	int32 GetCurrentlyActiveSwitchIndex();

	AExclusivSwitchArray();

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSwitched(AGazeGuiElement* GazeGuiElement, bool Active);

private:
	TScriptDelegate<FWeakObjectPtr> OnSwitchedDelegate;
};
