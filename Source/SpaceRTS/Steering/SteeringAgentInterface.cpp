#include "SpaceRTS.h"
#include "SteeringAgentInterface.h"


USteeringAgentInterface::USteeringAgentInterface(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void ISteeringAgentInterface::SetTargetPosition(const FVector& TargetPosition)
{
	// Base implementation neccessary, but should never be called
}

void ISteeringAgentInterface::CalculatePreferedVelocity()
{
	// Base implementation neccessary, but should never be called
}

void ISteeringAgentInterface::ComputeNewVelocity(UWorld* World, float DeltaTime)
{
	// Base implementation neccessary, but should never be called
}

const FVector& ISteeringAgentInterface::GetSteeringVelocity()
{
	// Base implementation neccessary, but should never be called
	static FVector Dummy(ForceInitToZero);
	return Dummy;
}

bool ISteeringAgentInterface::IsPrioritySignature()
{
	// Base implementation neccessary, but should never be called
	return false;
}

float ISteeringAgentInterface::GetSphereRadius()
{
	// Base implementation neccessary, but should never be called
	return 0.f;
}