

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Level Data")
	FName LevelName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelData")
	int32 FrameIndex;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Obstacle Avoidance")
	int32 ComputedSteeringAgentsPerFrame;

	ASteeringLevelScriptActor(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void RegisterSteeringAgent(ISteeringAgentInterface* SteeringAgent);

	virtual void UnregisterSteeringAgent(ISteeringAgentInterface* SteeringAgent);

private:
	TArray<ISteeringAgentInterface*> SteeringAgentList;

	int32 SteeringAgentCount;

	int32 SteeringComputationGroups;

	void UpdateSteeringAgents(float DeltaSeconds);
};
