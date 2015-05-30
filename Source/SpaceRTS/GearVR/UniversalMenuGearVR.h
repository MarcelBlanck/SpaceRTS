#pragma once

#include "Components/ActorComponent.h"
#include "UniversalMenuGearVR.generated.h"


UCLASS( ClassGroup=(GearVR), meta=(BlueprintSpawnableComponent) )
class SPACERTS_API UUniversalMenuGearVR : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUniversalMenuProgressDelegate, float, Progress);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUniversalMenuAbortDelegate);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUniversalMenuTriggeredDelegate);

public:	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Trigger Timing")
	float UniversalMenuTriggerTimeStart;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Trigger Timing")
	float UniversalMenuTriggerTimeEnd;

	UUniversalMenuGearVR();

	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void BackPressed();

	virtual void BackReleased();

	/* Delegate to execute when the Gear VR back button is pressed longer than UniversalMenuTriggerTimeStart,
	 * but shorter than UniversalMenuTriggerTimeEnd. The time progress will be lineary mapped 
	 * from 0.0f to 1.0f between these to points in time. */
	UPROPERTY(BlueprintAssignable)
	FUniversalMenuProgressDelegate OnUniversalMenuProgressChanged;

	/* Delegate to execute when the Gear VR back button is released while in the middle between
	 * UniversalMenuTriggerTimeStart and UniversalMenuTriggerTimeEnd */
	UPROPERTY(BlueprintAssignable)
	FUniversalMenuAbortDelegate OnUniversalMenuAbort;

	/* Delegate to execute when the Gear VR back button is pressed longer than UniversalMenuTriggerTimeEnd. 
	 * This should start the Gear VR Universal Menu by executing the "OVRGLOBALMENU" console command */
	UPROPERTY(BlueprintAssignable)
	FUniversalMenuTriggeredDelegate OnUniversalMenuTriggered;

private:
	bool bBackIsPressed;
	bool bOnUniversalMenuTriggered;
	float BackKeyPressTime;
	float SecondsSinceBackKeyPressed;
};
