// Marcel Blanck 2015 - All rights reserved.

#include "SpaceRTS.h"
#include "SpaceRTSPlayerCameraManager.h"

ASpaceRTSPlayerCameraManager::ASpaceRTSPlayerCameraManager() :
	Super()
{
	bUseClientSideCameraUpdates = false;
	bAlwaysApplyModifiers = true;
	bFollowHmdOrientation = true;
}



