

#include "SpaceRTS.h"
#include "PlayerFregatte.h"

APlayerFregatte::APlayerFregatte(const FObjectInitializer& ObjectInitializer) :
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
			checkf(MaterialAsset.Object, TEXT("Did not find Material'/Game/Actors/SpaceShipTest/_Meshes/MAT_Human_SPaceship.MAT_Human_SPaceship' - Maybe the asset was moved or renamed."));
			checkf(MeshAsset.Object, TEXT("Did not find StaticMesh'/Game/Actors/SpaceShipTest/_Meshes/PBR_Test.PBR_Test' - Maybe the asset was moved or renamed."));
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
	Mesh->BodyInstance.SetCollisionProfileName("Selectable");
	Mesh->SetRelativeRotation(FRotator(0.f, 280.f, 0.f));
	Mesh->AttachTo(RootComponent);

	SteeringAgentComponent->SetMaxVelocity(1600.f);
	SteeringAgentComponent->SetSphereRadius(420.f);
	SteeringAgentComponent->ScanRadius = 6000.f;
}

ESelectableObjectType APlayerFregatte::GetType()
{
	return ESelectableObjectType::PlayerControlledSpaceship;
}

void APlayerFregatte::Select()
{
	// Spawn Selection Display
}

void APlayerFregatte::ExecuteNative_AttackObject(const TScriptInterface<ISelectableObject>& Object)
{
	UE_LOG(Generic, Warning, TEXT("APlayerFregatte::ExecuteNative_AttackObject"));
}

void APlayerFregatte::ExecuteNative_SteerToLocation(FVector Location)
{
	SteeringAgentComponent->SetTargetPosition(Location);
}