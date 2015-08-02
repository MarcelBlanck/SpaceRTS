

#pragma once

#include "Engine/LevelScriptActor.h"
#include "SteeringAgentInterface.h"
#include "SteeringLevelScriptActor.generated.h"

/**
 * This levelscript provides the backand for any Steering
 */
UCLASS()
class SPACERTS_API ASteeringLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()
	
public:
	void RegisterSteeringAgent(ISteeringAgentInterface* SteeringAgent);

	void UnregisterSteeringAgent(ISteeringAgentInterface* SteeringAgent);

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Obstacle Avoidance")
	int32 ComputedSteeringAgentsPerFrame;

	ASteeringLevelScriptActor(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaSeconds) override;

private:
	int32 FrameIndex;

	TArray<ISteeringAgentInterface*> SteeringAgentList;

	int32 SteeringAgentCount;

	int32 SteeringComputationGroups;

	void UpdateSteeringAgents(float DeltaSeconds);
};
