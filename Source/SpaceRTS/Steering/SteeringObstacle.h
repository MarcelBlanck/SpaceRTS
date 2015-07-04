// Pyrite Softworks 2015

#pragma once

#include "GameFramework/Actor.h"
#include "SteeringObstacle.generated.h"

UCLASS()
class SPACERTS_API ASteeringObstacle : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	class USceneComponent* Scene;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Steering")
	class USphereComponent* RadarSignature;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Steering")
	float SignatureRadius;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Steering")
	bool IsPrioritySignature;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Steering")
	FVector TargetPosition;
	
	FVector Velocity;
	
	FVector NewVelocity;

	FVector PreferedVelocity;

	ASteeringObstacle(const FObjectInitializer& ObjectInitializer);

	virtual void OnConstruction(const FTransform& Transform) override;
};
