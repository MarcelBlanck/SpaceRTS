

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

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;	

private:
	TArray<class ASteeringObstacle*> Obstacles;
};
