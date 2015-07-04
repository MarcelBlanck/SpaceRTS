

#pragma once

#include "LevelScripts/SpaceRTSLevelScriptActor.h"
#include "TestObstacleAvoidanceLevelScript.generated.h"

/**
 * 
 */
UCLASS()
class SPACERTS_API ATestObstacleAvoidanceLevelScript : public ASpaceRTSLevelScriptActor
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Level Logic")
	void StartCircularTest(FVector Center, float Radius, int32 ShipCount);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Obstacle Avoidance")
	int32 ComputedObstaclesPerFrame;

	ATestObstacleAvoidanceLevelScript();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;	

private:
	TArray<class AMoveableSteeringObstacle*> Obstacles;
	uint32 FrameSlices;
};
