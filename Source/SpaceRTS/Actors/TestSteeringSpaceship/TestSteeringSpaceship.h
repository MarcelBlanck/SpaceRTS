

#pragma once

#include "Steering/SteeringAgentPawn.h"
#include "Gameplay/Interfaces/SelectableObject.h"
#include "Gameplay/Interfaces/PlayerControlledSpaceship.h"
#include "TestSteeringSpaceship.generated.h"

UCLASS()
class SPACERTS_API ATestSteeringSpaceship : public ASteeringAgentPawn, public ISelectableObject, public IPlayerControlledSpaceship
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* Mesh;

	ATestSteeringSpaceship(const FObjectInitializer& ObjectInitializer);

	virtual ESelectableObjectType GetType() override;

	virtual void Select() override;

	virtual void ExecuteNative_AttackObject(const TScriptInterface<ISelectableObject>& Object) override;
	
	virtual void ExecuteNative_SteerToLocation(FVector Location) override;

private:
	class UMaterial* Material;
};
