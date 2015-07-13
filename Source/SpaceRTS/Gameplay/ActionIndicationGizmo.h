
#pragma once

#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Delegate.h"
#include "ActionIndicationGizmo.generated.h"



UCLASS(ClassGroup = (GUI), meta = (BlueprintSpawnableComponent))
class SPACERTS_API UActionIndicationGizmo : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEngageMovement, FVector, TargetPosition);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEngageAttack, AActor*, TargetActor);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEngageInteraction, AActor*, TargetActor);

public:
	UActionIndicationGizmo();

	UFUNCTION(BlueprintCallable, Category = Updating)
	bool IsActionIndicationEnabled();

	UFUNCTION(BlueprintCallable, Category = Updating)
	void EnableActionIndication();

	UFUNCTION(BlueprintCallable, Category = Updating)
	void DisableActionIndication();

	UFUNCTION(BlueprintCallable, Category = Updating)
	void UpdateGizmoRepresentation(FVector HeadPosition, FRotator HeadOrientation, AActor* SelectedActor, AActor* LookAtActor);
	
	UFUNCTION(BlueprintCallable, Category = Updating)
	void EnterSuccessiveState();

	UFUNCTION(BlueprintCallable, Category = Updating)
	bool TryEnterPreviousState();
	
	UPROPERTY(BlueprintAssignable)
	FOnEngageMovement OnEngageMovement;
	FOnEngageMovement& GetOnEngageMovementDelegate();

	UPROPERTY(BlueprintAssignable)
	FOnEngageAttack OnEngageAttack;
	FOnEngageAttack& GetOnEngageAttackDelegate();

	UPROPERTY(BlueprintAssignable)
	FOnEngageInteraction OnEngageInteraction;
	FOnEngageInteraction& GetOnEngageInteractionDelegate();

private:
	enum class GizmoStage
	{
		Off,
		FindMovementRadius,
		FindMovementHeight,
		IndicateEngagedMovement,
		IndicatePossibleAttack,
		IndicateEngagedAttack,
		IndicatePossibleInteraction,
		IndicateEngagedInteraction,
		KeepLastIndication
	};

	GizmoStage CurrentGizmoStage;
	GizmoStage StoredGizmoStage;

	AActor* OriginActor;
	AActor* KinkActor;
	AActor* EndActor;

	AActor* EndFollowActor;
		
	UParticleSystem* ActionIndicationParticleSystem;
	UParticleSystemComponent* ActionIndicationMain;
	UParticleSystemComponent* ActionIndicationAfterKink;

	void EnableOnlyActionIndicationMain();
	void EnableBothActionIndications();
};
