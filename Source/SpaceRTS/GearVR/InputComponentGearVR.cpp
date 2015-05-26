// Pyrite Softworks 2015

#include "SpaceRTS.h"
#include "InputComponentGearVR.h"

#include "HAL/PlatformMisc.h"


UInputComponentGearVR::UInputComponentGearVR() :
	PlayerController(nullptr),
	TouchDownX(0.f),
	TouchDownY(0.f),
	TapHoldAreaMinX(0.f),
	TapHoldAreaMaxX(0.f),
	TapHoldAreaMinY(0.f),
	TapHoldAreaMaxY(0.f),
	VerticalSwipeCorridorMinX(0.f),
	VerticalSwipeCorridorMaxX(0.f),
	HorizontalSwipeCorridorMinY(0.f),
	HorizontalSwipeCorridorMaxY(0.f),
	TapHoldMovementToleranceExceeded(false),
	VerticalSwipeCorridorExceeded(false),
	HorizontalSwipeCorridorExceeded(false),
	HoldReported(false),
	bWasTouchDown(false),
	SecondsSinceTouchDown(-1.f),
	LastTapRealtimeSeconds(-1.f)
{
	PrimaryComponentTick.bCanEverTick = true;
	this->bWantsBeginPlay = true;
}

void UInputComponentGearVR::BeginPlay()
{
	APawn* Owner = Cast<APawn, AActor>(GetOwner());
	checkf(Owner != nullptr, TEXT("The Gesture Recognizer Gear VR Component can only be used on entitys inheriting from APawn."));
	
	PlayerController = Cast<APlayerController, AController>(Owner->GetController());
	
	if (PlayerController == nullptr)
	{
		UE_LOG(Generic, Warning, TEXT("The Gesture Recognizer Gear VR Component only works with a connected APlayerController."));
		return;
	}

#if PLATFORM_ANDROID == 1
	// Avoid starting with a volume popup
	LastVolume = FAndroidMisc::GetMusicStreamVolume();
	UE_LOG(Generic, Warning, TEXT("Volume initialized to %d"), LastVolume);
#endif
}

void UInputComponentGearVR::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if(PlayerController == nullptr)
	{
		UE_LOG(Generic, Warning, TEXT("The Gesture Recognizer Gear VR Component only works with a connected APlayerController."));
		return;
	}

	ReportBackKeyEvents(DeltaTime);
	ReportVolumeChanges();

	float X, Y;
	bool bIsTouchDown;
	PlayerController->GetInputTouchState(ETouchIndex::Type::Touch1, X, Y, bIsTouchDown);

	bool TouchUp = false;
	SecondsSinceTouchDown += DeltaTime;

	if (!bWasTouchDown && !bIsTouchDown)
	{
		// Nothing to do - return right here to speed things up
		return;
	}
	else if (!bWasTouchDown && bIsTouchDown)
	{
		OnTouchDown.Broadcast();
		bWasTouchDown = true;
		TouchDownX = X;
		TouchDownY = Y;
		TapHoldAreaMinX = X - TapHoldMovementTolerance / 2;
		TapHoldAreaMaxX = X + TapHoldMovementTolerance / 2;
		TapHoldAreaMinY = Y - TapHoldMovementTolerance / 2;
		TapHoldAreaMaxY = Y + TapHoldMovementTolerance / 2;
		VerticalSwipeCorridorMinX = X - VerticalSwipeCorridorWidth / 2;
		VerticalSwipeCorridorMaxX = X + VerticalSwipeCorridorWidth / 2;
		HorizontalSwipeCorridorMinY = Y - HorizontalSwipeCorridorWidth / 2;
		HorizontalSwipeCorridorMaxY = Y + HorizontalSwipeCorridorWidth / 2;
		TapHoldMovementToleranceExceeded = false;
		VerticalSwipeCorridorExceeded = false;
		HorizontalSwipeCorridorExceeded = false;
		HoldReported = false;
		SecondsSinceTouchDown = 0.f;
	}
	else if (bWasTouchDown && !bIsTouchDown)
	{
		TouchUp = true;
	}

	UpdateGestureMovementExceeds(X, Y);

	if (!TapHoldMovementToleranceExceeded && !HoldReported && SecondsSinceTouchDown > HoldTime)
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
		if (SecondsSinceTouchDown < TapMaxTime && !TapHoldMovementToleranceExceeded)
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


void UInputComponentGearVR::NotifyRelativeFingerMovement(float X, float Y)
{
	FVector2D RelativeFingerMovement (
		FMath::Clamp((TouchDownX - X) / TouchPadAverageCoordSize / 2, -1.f, 1.f),
		FMath::Clamp((TouchDownY - Y) / TouchPadAverageCoordSize / 2, -1.f, 1.f));

	OnRelativeFingerMovement.Broadcast(RelativeFingerMovement);
}

void UInputComponentGearVR::UpdateGestureMovementExceeds(float X, float Y)
{
	if (!TapHoldMovementToleranceExceeded
			&& (X < TapHoldAreaMinX || X > TapHoldAreaMaxX || Y < TapHoldAreaMinY || Y > TapHoldAreaMaxY))
	{
		TapHoldMovementToleranceExceeded = true;
	}

	if (!TapHoldMovementToleranceExceeded
			&& (X < VerticalSwipeCorridorMinX || X > VerticalSwipeCorridorMaxX))
	{
		VerticalSwipeCorridorExceeded = true;
	}

	if (!HorizontalSwipeCorridorExceeded
			&& (Y < HorizontalSwipeCorridorMinY || Y > HorizontalSwipeCorridorMaxY))
	{
		HorizontalSwipeCorridorExceeded = true;
	}
}

void UInputComponentGearVR::ReportBackKeyEvents(float DeltaSeconds)
{
#if PLATFORM_ANDROID == 1
	const bool bAndroidBackIsPressed= PlayerController->IsInputKeyDown(EKeys::Android_Back);
#else
	// For editor debugging
	const bool bAndroidBackIsPressed = PlayerController->IsInputKeyDown(EKeys::BackSpace);
#endif

	UE_LOG(Generic, Warning, TEXT("Android_Back pressed %s"), ((bAndroidBackIsPressed) ? TEXT("true") : TEXT("false")));
	
	SecondsSinceAndroidBackPressed += DeltaSeconds;

	if (!bAndroidBackWasPressed && !bAndroidBackIsPressed)
	{
		// Nothing to do - return right here to speed things up
		return;
	}
	else if (!bAndroidBackWasPressed && bAndroidBackIsPressed)
	{
		SecondsSinceAndroidBackPressed = 0.f;
		bAndroidBackWasPressed = true;
		bAndroidBackLongpressFired = false;
	}
	else if (bAndroidBackWasPressed && bAndroidBackIsPressed)
	{
		if (SecondsSinceAndroidBackPressed > BackClickMaxTime && SecondsSinceAndroidBackPressed < BackTimeToLongpress)
		{
			float LongPressProgress = FMath::Lerp(0.f, 1.f, (SecondsSinceAndroidBackPressed - BackClickMaxTime) / (BackTimeToLongpress - BackClickMaxTime));
			OnBackLongpressProgressChanged.Broadcast(LongPressProgress);
		}
		else if (!bAndroidBackLongpressFired && SecondsSinceAndroidBackPressed >= BackTimeToLongpress)
		{
			// This longpress is reported, release not neccesary
			OnBackLongpress.Broadcast();		
			bAndroidBackLongpressFired = true;
		}
	}
	else // Only one remaining case, the release: (bAndroidBackWasPressed && !bAndroidBackIsPressed)
	{
		if (SecondsSinceAndroidBackPressed <= BackClickMaxTime)
		{
			OnBackClicked.Broadcast();
		}
		else if (SecondsSinceAndroidBackPressed > BackClickMaxTime && SecondsSinceAndroidBackPressed < BackTimeToLongpress)
		{
			OnBackLongpressAbort.Broadcast();
		}
		else // Longpress happened
		{
			OnBackLongpressProgressChanged.Broadcast(0.f);
		}
		bAndroidBackWasPressed = false;
	}
}

void UInputComponentGearVR::ReportVolumeChanges()
{
#if PLATFORM_ANDROID == 1
	int CurrentVolume = FAndroidMisc::GetMusicStreamVolume();
	if (CurrentVolume != LastVolume)
	{
		LastVolume = CurrentVolume;
		OnVolumeChanged.Broadcast(CurrentVolume);
		UE_LOG(Generic, Warning, TEXT("Volume changed to %d"), LastVolume);
	}
#endif
}

void UInputComponentGearVR::ReportSwipeEvents(float X, float Y)
{
	if (SecondsSinceTouchDown < SwipeMaxTime)
	{
		if (!HorizontalSwipeCorridorExceeded)
		{
			float SwipedVectorX = TouchDownX - X;
			float SwipedDistanceX = FMath::Abs(SwipedVectorX);

			if (SwipedDistanceX > HorizontalSwipeMinDistance)
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
		}
		else if (!VerticalSwipeCorridorExceeded)
		{
			float SwipedVectorY = TouchDownY - Y;
			float SwipedDistanceY = FMath::Abs(SwipedVectorY);

			if (SwipedDistanceY > VerticalSwipeMinDistance)
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
}