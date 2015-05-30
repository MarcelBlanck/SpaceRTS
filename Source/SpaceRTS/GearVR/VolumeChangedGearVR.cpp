

#include "SpaceRTS.h"
#include "VolumeChangedGearVR.h"
#include "HAL/PlatformMisc.h"

UVolumeChangedGearVR::UVolumeChangedGearVR()
{
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UVolumeChangedGearVR::BeginPlay()
{
	Super::BeginPlay();

#if PLATFORM_ANDROID == 1
	LastVolume = FAndroidMisc::GetMusicStreamVolume();
	UE_LOG(Generic, Warning, TEXT("Volume initialized to %d"), LastVolume);

	SetComponentTickEnabled(true);
#else
	APlayerController *PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	checkf(PlayerController != nullptr, TEXT("The Volume Changed Gear VR Component Debug can not work without an existing PlayerController for player 0."));

	AActor* Owner = GetOwner();
	Owner->EnableInput(PlayerController);

	Owner->InputComponent->BindKey(EKeys::Add, EInputEvent::IE_Pressed, this, &UVolumeChangedGearVR::Debug_VolumePlus).bConsumeInput = false;
	Owner->InputComponent->BindKey(EKeys::Subtract, EInputEvent::IE_Released, this, &UVolumeChangedGearVR::Debug_VolumeMinus).bConsumeInput = false;

	LastVolume = 10;
	UE_LOG(Generic, Warning, TEXT("Volume initialized to %d"), LastVolume);

	SetComponentTickEnabled(false);
#endif
}

void UVolumeChangedGearVR::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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

#if PLATFORM_ANDROID == 0
void UVolumeChangedGearVR::Debug_VolumePlus()
{
	int CurrentVolume = FMath::Clamp(LastVolume + 1, 0, 15);
	LastVolume = CurrentVolume;
	OnVolumeChanged.Broadcast(CurrentVolume);
	UE_LOG(Generic, Warning, TEXT("Volume changed to %d"), LastVolume);
}

void UVolumeChangedGearVR::Debug_VolumeMinus()
{
	int CurrentVolume = FMath::Clamp(LastVolume - 1, 0, 15);
	LastVolume = CurrentVolume;
	OnVolumeChanged.Broadcast(CurrentVolume);
	UE_LOG(Generic, Warning, TEXT("Volume changed to %d"), LastVolume);
}
#endif
