#include "SpaceRTS.h"
#include "BackKeyGearVR.h"

UBackKeyGearVR::UBackKeyGearVR() :
	Super(),
	MaxClickTime(0.25f)
{
	bWantsBeginPlay = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bTickEvenWhenPaused = false;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void UBackKeyGearVR::BeginPlay()
{
	Super::BeginPlay();

	APawn* Owner = Cast<APawn>(GetOwner());
	checkf(Owner != nullptr, TEXT("The Gesture Recognizer Gear VR Component can only be used on entitys inheriting from APawn."));

	Owner->EnableInput(GetWorld()->GetFirstPlayerController());

#if PLATFORM_ANDROID == 1
	GetWorld()->GetFirstPlayerController()->InputComponent->BindAction("Back", EInputEvent::IE_Pressed, this, &UBackKeyGearVR::BackPressed).bConsumeInput = false;
	GetWorld()->GetFirstPlayerController()->InputComponent->BindAction("Back", EInputEvent::IE_Released, this, &UBackKeyGearVR::BackReleased).bConsumeInput = false;
#else
	// Allow debugging with Backspace key
	UE_LOG(Generic, Warning, TEXT("UBackKeyGearVR registering debug back key BackSpace"));
	GetWorld()->GetFirstPlayerController()->InputComponent->BindAction("Back", EInputEvent::IE_Pressed, this, &UBackKeyGearVR::BackPressed).bConsumeInput = false;
	GetWorld()->GetFirstPlayerController()->InputComponent->BindAction("Back", EInputEvent::IE_Released, this, &UBackKeyGearVR::BackReleased).bConsumeInput = false;
#endif
}

void UBackKeyGearVR::BackPressed()
{
	BackKeyPressTime = GetWorld()->GetTimeSeconds();
}


void UBackKeyGearVR::BackReleased()
{
	const float BackKeyReleaseTime = GetWorld()->GetTimeSeconds();
	if (BackKeyReleaseTime - BackKeyPressTime < MaxClickTime)
	{
		OnBackClicked.Broadcast();
	}
}
