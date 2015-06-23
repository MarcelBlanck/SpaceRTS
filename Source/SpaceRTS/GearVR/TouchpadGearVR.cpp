

#include "SpaceRTS.h"
#include "TouchpadGearVR.h"

UTouchpadGearVR::UTouchpadGearVR() :
	TouchPadAverageCoordSize(100.0f),
	TapMaxTime(0.25f),
	DoubleTapTime(0.75f),
	HoldTime(0.25f),
	bNotifyRelativeFingerMovement(false),
	SwipeMaxTime(0.8f),
	VerticalSwipeMinDistance(80.0f),
	HorizontalSwipeMinDistance(80.0f),
	PlayerController(nullptr),
	TouchDownX(0.f),
	TouchDownY(0.f),
	HoldReported(false),
	bWasTouchDown(false),
	SecondsSinceTouchDown(-1.f),
	LastTapRealtimeSeconds(-1.f)
{
	PrimaryComponentTick.bCanEverTick = true;
	this->bWantsBeginPlay = true;
}

void UTouchpadGearVR::BeginPlay()
{
	APawn* Owner = Cast<APawn, AActor>(GetOwner());
	checkf(Owner != nullptr, TEXT("The Gesture Recognizer Gear VR Component can only be used on entitys inheriting from APawn."));

	PlayerController = Cast<APlayerController, AController>(Owner->GetController());

	if (PlayerController == nullptr)
	{
		UE_LOG(Generic, Warning, TEXT("The Gesture Recognizer Gear VR Component only works with a connected APlayerController."));
		return;
	}
}

void UTouchpadGearVR::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PlayerController == nullptr)
	{
		UE_LOG(Generic, Warning, TEXT("The Gesture Recognizer Gear VR Component only works with a connected APlayerController."));
		return;
	}

	float X, Y;
	bool bIsTouchDown;
	PlayerController->GetInputTouchState(ETouchIndex::Type::Touch1, X, Y, bIsTouchDown);

	if (!bWasTouchDown && !bIsTouchDown)
	{
		// Nothing to do - return right here to speed things up
		return;
	}

	bool TouchUp = false;
	SecondsSinceTouchDown += DeltaTime;
	
	if (!bWasTouchDown && bIsTouchDown)
	{
		OnTouchDown.Broadcast();
		bWasTouchDown = true;
		TouchDownX = X;
		TouchDownY = Y;
		TouchStationary = true;
		HoldReported = false;
		SecondsSinceTouchDown = 0.f;
	}
	else if (bWasTouchDown && !bIsTouchDown)
	{
		TouchUp = true;
	}

	if (TouchStationary && (X != TouchDownX || Y != TouchDownY))
	{
		TouchStationary = false;
	}

	if (TouchStationary && !HoldReported && SecondsSinceTouchDown > HoldTime)
	{
		OnHold.Broadcast();
		HoldReported = true;
	}

	if (bNotifyRelativeFingerMovement)
	{
		NotifyRelativeFingerMovement(X, Y);
	}

	if (TouchUp)
	{
		if (TouchStationary && SecondsSinceTouchDown < TapMaxTime)
		{
			OnSingleTap.Broadcast();
			float TapRealtimeSeconds = GetWorld()->GetRealTimeSeconds();
			if (TapRealtimeSeconds - LastTapRealtimeSeconds < DoubleTapTime)
			{
				OnDoubleTap.Broadcast();
				LastTapRealtimeSeconds = -1.f; // Prevent reporting trippletaps
			}
			LastTapRealtimeSeconds = TapRealtimeSeconds;
		}
		else
		{
			LastTapRealtimeSeconds = -1.f; // Generic TouchUp is no tap, so double tap recognition should restart

			ReportSwipeEvents(X, Y);
		}

		OnTouchUp.Broadcast();
		bWasTouchDown = false;
	}
	else
	{
		bWasTouchDown = true;
	}
}

void UTouchpadGearVR::NotifyRelativeFingerMovement(float X, float Y)
{
	FVector2D RelativeFingerMovement(
		FMath::Clamp((TouchDownX - X) / TouchPadAverageCoordSize / 2, -1.f, 1.f),
		FMath::Clamp((TouchDownY - Y) / TouchPadAverageCoordSize / 2, -1.f, 1.f));

	OnRelativeFingerMovement.Broadcast(RelativeFingerMovement);
}

void UTouchpadGearVR::ReportSwipeEvents(float X, float Y)
{
	if (!TouchStationary && SecondsSinceTouchDown < SwipeMaxTime)
	{
		float SwipedVectorX = TouchDownX - X;
		float SwipedVectorY = TouchDownY - Y;

		float SwipedDistanceX = FMath::Abs(SwipedVectorX);
		float SwipedDistanceY = FMath::Abs(SwipedVectorY);

		if (SwipedDistanceX > HorizontalSwipeMinDistance && SwipedDistanceX > SwipedDistanceY)
		{
			if (SwipedVectorX > 0)
			{
				OnSwipeBackward.Broadcast();
			}
			else
			{
				OnSwipeForward.Broadcast();
			}
		}
		else if (SwipedDistanceY > VerticalSwipeMinDistance && SwipedDistanceY > SwipedDistanceX)
		{
			if (SwipedVectorY > 0)
			{
				OnSwipeUp.Broadcast();
			}
			else
			{
				OnSwipeDown.Broadcast();
			}
		}
	}
}

