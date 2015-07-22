// Marcel Blanck 2015 - All rights reserved.

#include "SpaceRTS.h"
#include "ExclusivSwitchArray.h"


AExclusivSwitchArray::AExclusivSwitchArray()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AExclusivSwitchArray::BeginPlay()
{
	Super::BeginPlay();

	OnSwitchedDelegate.BindUFunction(this, TEXT("OnSwitched"));
	for (AGazeGuiElement* Switch : Switches)
	{
		if (Switch != nullptr)
		{
			Switch->GazeGuiElementType = EGazeGuiElementType::Switch;
			Switch->OnSwitched.AddUnique(OnSwitchedDelegate);
		}
	}
}

void AExclusivSwitchArray::OnSwitched(AGazeGuiElement* GazeGuiElement, bool Active)
{
	if (Active)
	{
		// Deactivate all other switches
		for (AGazeGuiElement* Switch : Switches)
		{
			if (Switch != GazeGuiElement)
			{
				Switch->Deselect();
			}
		}
	}
}

AGazeGuiElement* AExclusivSwitchArray::GetCurrentlyActiveSwitch()
{
	for (AGazeGuiElement* Switch : Switches)
	{
		if (Switch->IsSwitchActive())
		{
			return Switch;
		}
	}
	return nullptr;
}

int32 AExclusivSwitchArray::GetCurrentlyActiveSwitchIndex()
{
	int32 Index = -1;

	int32 SwitchCount = Switches.Num();
	for (int32 i = 0; i < SwitchCount; ++i)
	{
		if (Switches[i]->IsSwitchActive())
		{
			Index = i;
			break;
		}
		
	}
	return Index;
}