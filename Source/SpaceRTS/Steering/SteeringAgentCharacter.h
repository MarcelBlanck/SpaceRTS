

#pragma once

#include "GameFramework/Character.h"
#include "SteeringAgentInterface.h"
#include "SteeringAgentCharacter.generated.h"

UCLASS()
class SPACERTS_API ASteeringAgentCharacter : public ACharacter, public ISteeringAgentInterface
{
	GENERATED_BODY()

public:
	ASteeringAgentCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual USteeringAgentComponent* GetSteeringAgentComponent() override;

	virtual void CalculatePreferedVelocity() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Steering")
	USteeringAgentComponent* SteeringAgentComponent;
};
