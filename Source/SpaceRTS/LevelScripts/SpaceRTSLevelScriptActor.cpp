

#include "SpaceRTS.h"
#include "SpaceRTSLevelScriptActor.h"



void ASpaceRTSLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

}

void ASpaceRTSLevelScriptActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASpaceRTSLevelScriptActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	FrameIndex++;
}
