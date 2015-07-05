

#pragma once

#include "../../Steering/SteeringAgentPawn.h"
#include "TestSteeringSpaceship.generated.h"

UCLASS()
class SPACERTS_API ATestSteeringSpaceship : public ASteeringAgentPawn
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* Mesh;

	ATestSteeringSpaceship(const FObjectInitializer& ObjectInitializer);

private:
	class UMaterial* Material;
};
