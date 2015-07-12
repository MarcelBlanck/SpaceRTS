
#pragma once

#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "ActionIndicationGizmo.generated.h"



UCLASS(ClassGroup = (GUI), meta = (BlueprintSpawnableComponent))
class SPACERTS_API UActionIndicationGizmo : public UActorComponent
{
	GENERATED_BODY()

	//DECLARE_MULTICAST_DELEGATE_TwoParams(FEngageMovementDelegate, AActor*, FVector);
	//DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEngageAttack, AActor*, AActor*)
	//DECLARE_MULTICAST_DELEGATE_TwoParams(FEngageInteractionDelegate, AActor*, AActor*);
public:
	UActionIndicationGizmo();

	UFUNCTION(BlueprintCallable, Category = Updating)
	void EnableActionIndication();

	UFUNCTION(BlueprintCallable, Category = Updating)
	void DisableActionIndication();

	UFUNCTION(BlueprintCallable, Category = Updating)
	void UpdateGizmoRepresentation(FVector HeadPosition, FRotator HeadOrientation, AActor* SelectedActor, AActor* LookAtActor);
	
	UFUNCTION(BlueprintCallable, Category = Updating)
	void EnterSuccessiveState();
	
	//UPROPERTY(BlueprintAssignable)
	//FEngageMovementDelegate OnEngageMovement;

	//UPROPERTY(BlueprintAssignable)
	//FOnEngageAttack OnEngageAttack;

	//UPROPERTY(BlueprintAssignable)
	//FEngageInteractionDelegate OnEngageInteraction;

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

	AActor* OriginActor;
	AActor* KinkActor;
	AActor* EndActor;

	AActor* EndFollowActor;
};
