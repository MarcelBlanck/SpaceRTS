// Pyrite Softworks 2015

#pragma once

#include "GameFramework/Actor.h"
#include "SteeringObstacle.generated.h"

UCLASS()
class SPACERTS_API ASteeringObstacle : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle")
	class USphereComponent* RadarSignature;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class USteering3D* Steering;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Obstacle")
	float SignatureRadius;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Steering")
	FVector TargetPosition;
	
	FVector Velocity;
	
	FVector NewVelocity;

	FVector PreferedVelocity;

	ASteeringObstacle(const FObjectInitializer& ObjectInitializer);

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaSeconds) override;
};
