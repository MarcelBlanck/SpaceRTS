

#include "SpaceRTS.h"
#include "SteeringLevelScriptActor.h"

ASteeringLevelScriptActor::ASteeringLevelScriptActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	ComputedSteeringAgentsPerFrame(5U),
	FrameIndex(0U),
	SteeringAgentList(),
	SteeringAgentCount(0U),
	SteeringComputationGroups(1U)
{

}

void ASteeringLevelScriptActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	FrameIndex++;

	UpdateSteeringAgents(DeltaSeconds);
}

void ASteeringLevelScriptActor::RegisterSteeringAgent(ISteeringAgentInterface* SteeringAgent)
{
	SteeringAgentList.AddUnique(SteeringAgent);

	SteeringAgentCount = SteeringAgentList.Num();
	SteeringComputationGroups = FMath::Max(SteeringAgentCount / ComputedSteeringAgentsPerFrame, 1);
}

void ASteeringLevelScriptActor::UnregisterSteeringAgent(ISteeringAgentInterface* SteeringAgent)
{
	if (SteeringAgentList.RemoveSingle(SteeringAgent) > 0)
	{
		SteeringAgentCount = SteeringAgentList.Num();
		SteeringComputationGroups = FMath::Max(SteeringAgentCount / ComputedSteeringAgentsPerFrame, 1);
	}
}

void ASteeringLevelScriptActor::UpdateSteeringAgents(float DeltaSeconds)
{
	UWorld* World = GetWorld();
	if (World == nullptr)
		return;

	int32 CurrentComputationGroup = FrameIndex % SteeringComputationGroups;
	int32 ComputationGroupOffset = CurrentComputationGroup * ComputedSteeringAgentsPerFrame;

	for (int32 i = ComputationGroupOffset; i < SteeringAgentCount ; ++i)
	{
		SteeringAgentList[i]->CalculatePreferedVelocity();
		SteeringAgentList[i]->ComputeNewVelocity(World, DeltaSeconds);
	}
}