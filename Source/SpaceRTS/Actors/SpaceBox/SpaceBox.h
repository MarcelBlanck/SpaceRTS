

#pragma once

#include "GameFramework/Actor.h"
#include "SpaceBox.generated.h"

UCLASS()
class SPACERTS_API ASpaceBox : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Appearance")
	float SpaceBoxSizeKiloMeters;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Appearance")
	UTexture2D* Texture;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Positioning")
	AActor* FollowActor;	
	
	ASpaceBox(const FObjectInitializer& ObjectInitializer);

	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void Tick( float DeltaSeconds ) override;

protected:
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* SpaceBoxMesh;

private:
	class UMaterial* SpaceBoxMaterial;
};
