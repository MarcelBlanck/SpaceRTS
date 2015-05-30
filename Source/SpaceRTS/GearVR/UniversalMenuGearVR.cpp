
#include "SpaceRTS.h"
#include "UniversalMenuGearVR.h"

UUniversalMenuGearVR::UUniversalMenuGearVR() :
	UniversalMenuTriggerTimeStart(0.25f),
	UniversalMenuTriggerTimeEnd(1.0f)
{
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UUniversalMenuGearVR::BeginPlay()
{
	APlayerController *PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	checkf(PlayerController != nullptr, TEXT("The Universal Menu Gear VR Component can not work without an existing PlayerController for player 0."));

	AActor* Owner = GetOwner();
	Owner->EnableInput(PlayerController);

#if PLATFORM_ANDROID == 1
	Owner->InputComponent->BindKey(EKeys::Android_Back, EInputEvent::IE_Pressed, this, &UBackKeyGearVR::AndroidBackPressed);
	Owner->InputComponent->BindKey(EKeys::Android_Back, EInputEvent::IE_Released, this, &UBackKeyGearVR::AndroidBackReleased);
#else
	// Allow debugging with the BackSpace key
	Owner->InputComponent->BindKey(EKeys::BackSpace, EInputEvent::IE_Pressed, this, &UUniversalMenuGearVR::BackPressed).bConsumeInput = false;
	Owner->InputComponent->BindKey(EKeys::BackSpace, EInputEvent::IE_Released, this, &UUniversalMenuGearVR::BackReleased).bConsumeInput = false;
#endif

	SetComponentTickEnabled(false);
}

void UUniversalMenuGearVR::BackPressed()
{
	bBackIsPressed = true;

	UWorld* World = GEngine->GetWorldFromContextObject(this);
	BackKeyPressTime = World->GetTimeSeconds();

	SecondsSinceBackKeyPressed = 0.f;
	bOnUniversalMenuTriggered = false;

	SetComponentTickEnabled(true); // Tick as long as the key is pressed
}

void UUniversalMenuGearVR::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bBackIsPressed)
	{
		SecondsSinceBackKeyPressed += DeltaTime;

		if (SecondsSinceBackKeyPressed > UniversalMenuTriggerTimeStart && SecondsSinceBackKeyPressed < UniversalMenuTriggerTimeEnd)
		{
			float LongPressProgress = (SecondsSinceBackKeyPressed - UniversalMenuTriggerTimeStart) / (UniversalMenuTriggerTimeEnd - UniversalMenuTriggerTimeStart);
			OnUniversalMenuProgressChanged.Broadcast(LongPressProgress);
		}
		else if (!bOnUniversalMenuTriggered && SecondsSinceBackKeyPressed >= UniversalMenuTriggerTimeEnd)
		{
			// This longpress is reported, release not neccesary
			OnUniversalMenuTriggered.Broadcast();
			bOnUniversalMenuTriggered = true;
		}
	}
}

void UUniversalMenuGearVR::BackReleased()
{
	bBackIsPressed = false;

	UWorld* World = GEngine->GetWorldFromContextObject(this);
	float BackKeyReleaseTime = World->GetTimeSeconds();

	SecondsSinceBackKeyPressed = BackKeyReleaseTime - BackKeyPressTime;

	if (SecondsSinceBackKeyPressed > UniversalMenuTriggerTimeStart && !bOnUniversalMenuTriggered)
	{
		OnUniversalMenuAbort.Broadcast();
	}

	SetComponentTickEnabled(false); // No reason to tick if the key is not pressed
}

