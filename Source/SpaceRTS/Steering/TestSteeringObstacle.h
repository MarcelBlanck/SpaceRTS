

#pragma once

#include "Steering/SteeringObstacle.h"
#include "TestSteeringObstacle.generated.h"

/**
 * 
 */
UCLASS()
class SPACERTS_API ATestSteeringObstacle : public ASteeringObstacle
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* Mesh;

	FVector Color;

	ATestSteeringObstacle(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

private:
	class UMaterial* Material;
};
