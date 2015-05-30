

#pragma once

#include "Components/ActorComponent.h"
#include "BackKeyGearVR.generated.h"


UCLASS( ClassGroup=(GearVR), meta=(BlueprintSpawnableComponent) )
class SPACERTS_API UBackKeyGearVR : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBackClickedDelegate);

public:	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Click Timing")
	float MaxClickTime;

	UBackKeyGearVR();

	void BeginPlay() override;

	virtual void BackPressed();

	virtual void BackReleased();

	/* Delegate to execute when the Gear VR back button is pressed and released before MaxClickTime is over */
	UPROPERTY(BlueprintAssignable)
	FBackClickedDelegate OnBackClicked;

private:
	float BackKeyPressTime;
};
