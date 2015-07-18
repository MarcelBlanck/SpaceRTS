

#pragma once

#include "Steering/SteeringAgentPawn.h"
#include "Gameplay/Interfaces/SelectableObject.h"
#include "Gameplay/Interfaces/PlayerControlledSpaceship.h"
#include "PlayerFregatte.generated.h"

UCLASS()
class SPACERTS_API APlayerFregatte : public ASteeringAgentPawn, public ISelectableObject, public IPlayerControlledSpaceship
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* Mesh;

	APlayerFregatte(const FObjectInitializer& ObjectInitializer);

	virtual ESelectableObjectType GetType() override;

	virtual void Select() override;

	virtual void ExecuteNative_AttackObject(const TScriptInterface<ISelectableObject>& Object) override;
	
	virtual void ExecuteNative_SteerToLocation(FVector Location) override;

private:
	class UMaterial* Material;
};
