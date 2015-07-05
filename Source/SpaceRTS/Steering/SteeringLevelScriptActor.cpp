

#include "SpaceRTS.h"
#include "SteeringLevelScriptActor.h"

ASteeringLevelScriptActor::ASteeringLevelScriptActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	SteeringAgentCount(0U),
	SteeringComputationGroups(1U),
	FrameIndex(0U),
	ComputedSteeringAgentsPerFrame(5U)
{

}

void ASteeringLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

}

void ASteeringLevelScriptActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
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
	SteeringAgentList.RemoveSingle(SteeringAgent);

	SteeringAgentCount = SteeringAgentList.Num();
	SteeringComputationGroups = FMath::Max(SteeringAgentCount / ComputedSteeringAgentsPerFrame, 1);
}

void ASteeringLevelScriptActor::UpdateSteeringAgents(float DeltaSeconds)
{
	UWorld* World = GetWorld();
	if (World == nullptr)
		return;

	for (ISteeringAgentInterface* SteeringAgent : SteeringAgentList)
	{
		SteeringAgent->CalculatePreferedVelocity();
	}

	int32 CurrentComputationGroup = FrameIndex % SteeringComputationGroups;
	int32 ComputationGroupOffset = CurrentComputationGroup * ComputedSteeringAgentsPerFrame;
	
	SteeringAgentCount = SteeringAgentList.Num();
	for (int32 i = ComputationGroupOffset; i < SteeringAgentCount ; ++i)
	{
		SteeringAgentList[i]->GetSteeringAgentComponent()->ComputeNewVelocity(World, DeltaSeconds);
	}
}