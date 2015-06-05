

#pragma once

#include "Components/ActorComponent.h"
#include "TouchpadGearVR.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACERTS_API UTouchpadGearVR : public UActorComponent
{
	GENERATED_BODY()

	/* Low Level touch input events */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTouchDownDelegate);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTouchUpDelegate);

	/* If bNotifyRelativeFingerMovement is true, the touch start to current vector is continuously reported.
	* The vector coordinates are normalized to -1.0 and 1.0, while 0.0 is the touch start position. */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRelativeFingerMovementDelegate, FVector2D const &, RelativeFingerMovement);

	/* Tap events */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSingleTapGestureDelegate);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDoubleTapGestureDelegate);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHoldGestureDelegate);

	/* Swipe Events */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSwipeUpDelegate);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSwipeDownDelegate);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSwipeBackwardDelegate);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSwipeForwardDelegate);

public:
	UTouchpadGearVR();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GearVR Input")
	float TouchPadAverageCoordSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GearVR Input")
	float TapMaxTime;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GearVR Input")
	float DoubleTapTime;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GearVR Input")
	float HoldTime;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GearVR Input")
	bool bNotifyRelativeFingerMovement;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GearVR Swipe Gestures")
	float SwipeMaxTime;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GearVR Swipe Gestures")
	float VerticalSwipeMinDistance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GearVR Swipe Gestures")
	float HorizontalSwipeMinDistance;


	/* Delegate to execute when the Gear VR touch pad is touched */
	UPROPERTY(BlueprintAssignable)
	FTouchDownDelegate OnTouchDown;

	/* Delegate to execute when the Gear VR touch pad is no longer touched */
	UPROPERTY(BlueprintAssignable)
	FTouchUpDelegate OnTouchUp;

	/* Delegate to execute when the touch moves on the Gear VR touch pad
	* Only executes if bNotifyRelativeFingerMovement is set to true */
	UPROPERTY(BlueprintAssignable)
	FRelativeFingerMovementDelegate OnRelativeFingerMovement;

	/** Delegate to execute when */
	UPROPERTY(BlueprintAssignable)
	FSingleTapGestureDelegate OnSingleTap;

	/** Delegate to execute when */
	UPROPERTY(BlueprintAssignable)
	FDoubleTapGestureDelegate OnDoubleTap;

	/** Delegate to execute when */
	UPROPERTY(BlueprintAssignable)
	FHoldGestureDelegate OnHold;

	/** Delegate to execute when */
	UPROPERTY(BlueprintAssignable)
	FSwipeUpDelegate OnSwipeUp;

	/** Delegate to execute when */
	UPROPERTY(BlueprintAssignable)
	FSwipeDownDelegate OnSwipeDown;

	/** Delegate to execute when */
	UPROPERTY(BlueprintAssignable)
	FSwipeBackwardDelegate OnSwipeBackward;

	/** Delegate to execute when */
	UPROPERTY(BlueprintAssignable)
	FSwipeForwardDelegate OnSwipeForward;

private:
	APlayerController* PlayerController;

	float TouchDownX;
	float TouchDownY;
	bool TouchStationary;

	bool HoldReported;

	bool bWasTouchDown;
	float SecondsSinceTouchDown;

	float LastTapRealtimeSeconds;

	void NotifyRelativeFingerMovement(float X, float Y);
	void ReportSwipeEvents(float X, float Y);
};
