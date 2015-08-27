
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

	GetWorld()->GetFirstPlayerController()->InputComponent->BindAction("Back", EInputEvent::IE_Pressed, this, &UUniversalMenuGearVR::BackPressed).bConsumeInput = false;
	GetWorld()->GetFirstPlayerController()->InputComponent->BindAction("Back", EInputEvent::IE_Released, this, &UUniversalMenuGearVR::BackReleased).bConsumeInput = false;

	SetComponentTickEnabled(false);
}

void UUniversalMenuGearVR::BackPressed()
{
	if (!bBackIsPressed)
	{
		bBackIsPressed = true;

		UWorld* World = GEngine->GetWorldFromContextObject(this);
		BackKeyPressTime = World->GetTimeSeconds();

		SecondsSinceBackKeyPressed = 0.f;
		bOnUniversalMenuTriggered = false;

		SetComponentTickEnabled(true); // Tick as long as the key is pressed
	}
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

	OnUniversalMenuAbort.Broadcast();

	SetComponentTickEnabled(false); // No reason to tick if the key is not pressed
}

