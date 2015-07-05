

#include "SpaceRTS.h"
#include "TestSteeringSpaceship.h"

ATestSteeringSpaceship::ATestSteeringSpaceship(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	// One time asset lookup
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinder<UMaterial> MaterialAsset;
		ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset;
		FConstructorStatics() :
			MaterialAsset(TEXT("Material'/Game/Actors/SpaceShipTest/_Meshes/MAT_Human_SPaceship.MAT_Human_SPaceship'")),
			MeshAsset(TEXT("StaticMesh'/Game/Actors/SpaceShipTest/_Meshes/PBR_Test.PBR_Test'"))
		{
			checkf(MaterialAsset.Object, TEXT("Did not find Material'/Game/Testing/TestAssets/TestObjects/MAT_TestShip.MAT_TestShip' - Maybe the asset was moved or renamed."));
			checkf(MeshAsset.Object, TEXT("Did not find StaticMesh'/Game/Testing/TestAssets/TestObjects/TestShip.TestShip' - Maybe the asset was moved or renamed."));
		}
	};
	static FConstructorStatics ConstructorStatics;


	Material = ConstructorStatics.MaterialAsset.Object;

	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	Mesh->SetStaticMesh(ConstructorStatics.MeshAsset.Object);
	Mesh->SetMobility(EComponentMobility::Movable);
	Mesh->bCastDynamicShadow = false;
	Mesh->bTreatAsBackgroundForOcclusion = false;
	Mesh->SetSimulatePhysics(false);
	Mesh->bGenerateOverlapEvents = false;
	Mesh->SetEnableGravity(false);
	Mesh->BodyInstance.SetInstanceNotifyRBCollision(false);
	Mesh->BodyInstance.SetCollisionProfileName("NoCollision");
	Mesh->SetRelativeRotation(FRotator(0.f, 280.f, 0.f));
	Mesh->AttachTo(RootComponent);

	SteeringAgentComponent->SetMaxVelocity(1600.f);

	SteeringAgentComponent->SetSphereRadius(420.f);
}
