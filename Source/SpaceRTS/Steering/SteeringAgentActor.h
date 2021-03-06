

#pragma once

#include "GameFramework/Actor.h"
#include "SteeringAgentInterface.h"
#include "SteeringAgentComponent.h"
#include "SteeringAgentActor.generated.h"

UCLASS()
class SPACERTS_API ASteeringAgentActor : public AActor, public ISteeringAgentInterface
{
	GENERATED_BODY()
	
public:	
	ASteeringAgentActor(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// ISteeringAgentInterface implementation (inlining makes no sense here due to polymorphic class usage)
	virtual void SetTargetPosition(const FVector& TargetPosition) override;
	virtual void CalculatePreferedVelocity() override;
	virtual void ComputeNewVelocity(UWorld* World, float DeltaTime) override;
	virtual const FVector& GetSteeringVelocity() override;
	virtual bool IsPrioritySignature() override;
	virtual float GetSphereRadius() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	class USceneComponent* Scene;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Steering")
	USteeringAgentComponent* SteeringAgentComponent;
};
