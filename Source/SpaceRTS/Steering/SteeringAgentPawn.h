

#pragma once

#include "GameFramework/Pawn.h"
#include "SteeringAgentInterface.h"
#include "SteeringAgentComponent.h"
#include "SteeringAgentPawn.generated.h"

UCLASS()
class SPACERTS_API ASteeringAgentPawn : public APawn, public ISteeringAgentInterface
{
	GENERATED_BODY()

public:
	ASteeringAgentPawn(const FObjectInitializer& ObjectInitializer);

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
