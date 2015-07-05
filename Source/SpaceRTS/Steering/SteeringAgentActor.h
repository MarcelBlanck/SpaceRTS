

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

	virtual USteeringAgentComponent* GetSteeringAgentComponent() override;	

	virtual void CalculatePreferedVelocity() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	class USceneComponent* Scene;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Steering")
	USteeringAgentComponent* SteeringAgentComponent;
};
