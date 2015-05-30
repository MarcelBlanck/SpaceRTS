
#pragma once

#include "Components/ActorComponent.h"
#include "GearVR_Volume.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACERTS_API UGearVR_Volume : public UActorComponent
{
	GENERATED_BODY()

	/* Volume changed event declaration */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVolumeChangedDelegate, int32, Volume);

public:	
	UGearVR_Volume();

	virtual void BeginPlay() override;
	
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;



private:
	int LastVolume;
};
