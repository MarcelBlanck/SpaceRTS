

#include "SpaceRTS.h"
#include "GearVR_Volume.h"


UGearVR_Volume::UGearVR_Volume()
{
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}


void UGearVR_Volume::BeginPlay()
{
	Super::BeginPlay();

#if PLATFORM_ANDROID == 1
	LastVolume = FAndroidMisc::GetMusicStreamVolume();
	UE_LOG(Generic, Warning, TEXT("Volume initialized to %d"), LastVolume);
#endif
}

void UGearVR_Volume::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

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

