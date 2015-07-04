// Pyrite Softworks 2015

#pragma once

#include "SteeringObstacle.h"
#include "MoveableSteeringObstacle.generated.h"

UCLASS()
class SPACERTS_API AMoveableSteeringObstacle : public ASteeringObstacle
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Steering")
	class USteering3D* Steering;

	AMoveableSteeringObstacle(const FObjectInitializer& ObjectInitializer);
};
