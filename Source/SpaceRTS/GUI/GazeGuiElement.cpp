

#include "SpaceRTS.h"
#include "GazeGuiElement.h"

AGazeGuiElement::AGazeGuiElement(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void AGazeGuiElement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

ESelectableObjectType AGazeGuiElement::GetType()
{
	return ESelectableObjectType::UI;
}

void AGazeGuiElement::Select()
{
	if (OnSelect.IsBound())
	{
		OnSelect.Broadcast();
	}
}

void AGazeGuiElement::Deselect()
{
	if (OnDeselect.IsBound())
	{
		OnDeselect.Broadcast();
	}
}

void AGazeGuiElement::GazeBegin()
{
	if (OnGazeBegin.IsBound())
	{
		OnGazeBegin.Broadcast();
	}
}

void AGazeGuiElement::GazeEnd()
{
	if (OnGazeEnd.IsBound())
	{
		OnGazeEnd.Broadcast();
	}
}