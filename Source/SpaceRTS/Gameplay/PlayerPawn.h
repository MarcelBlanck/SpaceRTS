

#pragma once

#include "Steering/SteeringAgentPawn.h"
#include "PaperFlipbookComponent.h"
#include "ActionIndicationGizmo.h"
#include "../GearVR/TouchpadGearVR.h"
#include "PlayerPawn.generated.h"


UCLASS()
class SPACERTS_API APlayerPawn : public ASteeringAgentPawn
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UPaperFlipbookComponent* Recticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UActionIndicationGizmo* ActionIndicator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UTouchpadGearVR* TouchpadGearVR;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float MaxInteractionDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recticle")
	float RecticleDefaultDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recticle")
	float RecticleAnimationRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recticle")
	float RecticleAlpha;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recticle")
	FLinearColor RecticleColorNeutral;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recticle")
	FLinearColor RecticleColorFriendly;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recticle")
	FLinearColor RecticleColorAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recticle")
	FLinearColor RecticleColorInteract;

	APlayerPawn(const FObjectInitializer& ObjectInitializer);

	virtual FVector GetPawnViewLocation() const override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OnEngageMovement(FVector TargetPosition);

	UFUNCTION()
	void OnEngageAttack(AActor* TargetActor);

	UFUNCTION()
	void OnEngageInteraction(AActor* TargetActor);

	UFUNCTION()
	void OnLookInteraction();

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
		
	void OnLookRight(float Value);

	void OnLookUp(float Value);

private:
	FVector CameraLocation;

	TWeakObjectPtr<class AActor> CurrentLookAtActor;
	TWeakObjectPtr<class AActor> SelectedActor;

	TScriptDelegate<FWeakObjectPtr> OnEngageMovmentDelegate;

	TScriptDelegate<FWeakObjectPtr> OnGearVRTouchpadTapDelegate;

	void ApplyOrientationFromHMD();
	void UpdateLookAtActorAndRecticle();
};
