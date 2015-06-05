#include "SpaceRTS.h"
#include "BackKeyGearVR.h"

UBackKeyGearVR::UBackKeyGearVR()
{
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UBackKeyGearVR::BeginPlay()
{
	Super::BeginPlay();

	APlayerController *PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	checkf(PlayerController != nullptr, TEXT("The Universal Menu Gear VR Component can not work without an existing PlayerController for player 0."));

	AActor* Owner = GetOwner();
	Owner->EnableInput(PlayerController);

#if PLATFORM_ANDROID == 1
	Owner->InputComponent->BindKey(EKeys::Android_Back, EInputEvent::IE_Pressed, this, &UBackKeyGearVR::BackPressed).bConsumeInput = false;
	Owner->InputComponent->BindKey(EKeys::Android_Back, EInputEvent::IE_Released, this, &UBackKeyGearVR::BackReleased).bConsumeInput = false;
#else
	// Allow debugging with Backspace key
	UE_LOG(Generic, Warning, TEXT("UBackKeyGearVR registering debug back key E"));
	Owner->InputComponent->BindKey(EKeys::E, EInputEvent::IE_Pressed, this, &UBackKeyGearVR::BackPressed).bConsumeInput = false;
	Owner->InputComponent->BindKey(EKeys::E, EInputEvent::IE_Released, this, &UBackKeyGearVR::BackReleased).bConsumeInput = false;
#endif

	SetComponentTickEnabled(false);
}

void UBackKeyGearVR::BackPressed()
{
	BackKeyPressTime = GEngine->GetWorldFromContextObject(this)->GetTimeSeconds();
}


void UBackKeyGearVR::BackReleased()
{
	const float BackKeyReleaseTime = GEngine->GetWorldFromContextObject(this)->GetTimeSeconds();
	if (BackKeyReleaseTime - BackKeyPressTime < MaxClickTime)
	{
		OnBackClicked.Broadcast();
	}
}
