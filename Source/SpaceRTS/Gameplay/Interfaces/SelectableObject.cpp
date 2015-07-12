#include "SpaceRTS.h"
#include "SelectableObject.h"


USelectableObject::USelectableObject(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

ESelectableObjectType ISelectableObject::GetType()
{
	// Base implementation neccessary, but should never be called
	return ESelectableObjectType::Prop;
}

void ISelectableObject::Select()
{
	// Base implementation neccessary, but should never be called
}