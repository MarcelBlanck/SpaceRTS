

#include "SpaceRTS.h"
#include "GameInstanceSpaceRTS.h"


UGameInstanceSpaceRTS::UGameInstanceSpaceRTS(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	ShowPlayerPawnBody(true),
	PlayerBodyPosition(EPlayerBodyPosition::NormalChair),
	InterpupillaryDistance(64.4f),
	GuiColor(0.0f, 1.0f, 0.0f, 1.0f),
	GuiHighlightColor(0.353f, 1.0f, 0.4275f, 1.0f)
{

}

