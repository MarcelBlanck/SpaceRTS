

#pragma once

#include "Steering/SteeringAgentPawn.h"
#include "PlayerPawn.generated.h"

/**
 * 
 */
UCLASS()
class SPACERTS_API APlayerPawn : public ASteeringAgentPawn
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* Camera;
	
	APlayerPawn(const FObjectInitializer& ObjectInitializer);

	virtual FVector GetPawnViewLocation() const override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	void OnLookInteraction();
	
	void OnLookRight(float Value);

	void OnLookUp(float Value);

private:
	FVector CameraLocation;

	TWeakObjectPtr<class AActor> CurrentLookAtActor;

	void UpdateLookAtActorAndCursor();
};
