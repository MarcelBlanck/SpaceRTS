

#pragma once

#include "../Steering/SteeringLevelScriptActor.h"
#include "TestObstacleAvoidanceLevelScript.generated.h"

/**
 * 
 */
UCLASS()
class SPACERTS_API ATestObstacleAvoidanceLevelScript : public ASteeringLevelScriptActor
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Testcase Preparation")
	void StartWallTest(FVector Center, float Distance, int32 Rows, float RowDistance, int32 Columns, float ColumnDistance);

	UFUNCTION(BlueprintCallable, Category = "Testcase Preparation")
	void StartCircularTest(FVector Center, float Radius, int32 Segments);

	UFUNCTION(BlueprintCallable, Category = "Testcase Preparation")
	void StartSphereTest(FVector Center, float Radius, int32 Segments);

private:
	TArray<AActor*> SpawnedActors;

	void DestroyAllSpawnedActors();
};
