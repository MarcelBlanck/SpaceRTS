

#pragma once

#include "Components/ActorComponent.h"
#include "VolumeChangedGearVR.generated.h"


UCLASS( ClassGroup=(GearVR), meta=(BlueprintSpawnableComponent) )
class SPACERTS_API UVolumeChangedGearVR : public UActorComponent
{
	GENERATED_BODY()

	/* Volume changed event declaration */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVolumeChangedDelegate, int32, Volume);

public:	
	UVolumeChangedGearVR();

	virtual void BeginPlay() override;
	
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

#if PLATFORM_ANDROID == 0
	virtual void Debug_VolumePlus();

	virtual void Debug_VolumeMinus();
#endif

	/** Delegate to execute when the gear VR System Volume changes. This allows to show a popup as user feedback. */
	UPROPERTY(BlueprintAssignable)
	FVolumeChangedDelegate OnVolumeChanged;

private:
	int32 LastVolume;
};
