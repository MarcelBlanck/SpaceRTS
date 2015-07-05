#include "SpaceRTS.h"
#include "SteeringAgentInterface.h"


USteeringAgentInterface::USteeringAgentInterface(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

USteeringAgentComponent* ISteeringAgentInterface::GetSteeringAgentComponent()
{
	// Base implementation neccessary, but should never be called
	return nullptr;
}

void ISteeringAgentInterface::CalculatePreferedVelocity()
{
	// Base implementation neccessary, but should never be called
}