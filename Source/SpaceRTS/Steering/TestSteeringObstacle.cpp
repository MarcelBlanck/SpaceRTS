

#include "SpaceRTS.h"
#include "TestSteeringObstacle.h"
#include "Steering3D.h"



ATestSteeringObstacle::ATestSteeringObstacle(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	ConstructorHelpers::FObjectFinder<UMaterial> MaterialFinder(TEXT("Material'/Game/Testing/TestAssets/TestObjects/MAT_TestShip.MAT_TestShip'"));
	checkf(MaterialFinder.Object, TEXT("Did not find Material'/Game/Testing/TestAssets/TestObjects/MAT_TestShip.MAT_TestShip' - Maybe the asset was moved or renamed."));
	Material = MaterialFinder.Object;

	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("StaticMesh'/Game/Testing/TestAssets/TestObjects/TestShip.TestShip'"));
	checkf(MeshFinder.Object, TEXT("Did not find StaticMesh'/Game/Testing/TestAssets/TestObjects/TestShip.TestShip' - Maybe the asset was moved or renamed."));
	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	Mesh->SetStaticMesh(MeshFinder.Object);
	Mesh->SetMobility(EComponentMobility::Movable);
	Mesh->bCastDynamicShadow = false;
	Mesh->bTreatAsBackgroundForOcclusion = true;
	Mesh->bGenerateOverlapEvents = 0;
	Mesh->bReceivesDecals = 0;
	Mesh->CastShadow = 0;
	Mesh->bReturnMaterialOnMove = 0;
	Mesh->bAffectDynamicIndirectLighting = 0;
	Mesh->bAffectDistanceFieldLighting = 0;
	Mesh->bCastStaticShadow = 0;
	Mesh->bCastVolumetricTranslucentShadow = 0;
	Mesh->bSelfShadowOnly = 0;
	Mesh->bCastFarShadow = 0;
	Mesh->bCastInsetShadow = 0;
	Mesh->bCastCinematicShadow = 0;
	Mesh->bCastHiddenShadow = 0;
	Mesh->SetSimulatePhysics(false);
	Mesh->bGenerateOverlapEvents = false;
	Mesh->SetEnableGravity(false);
	Mesh->BodyInstance.bAutoWeld = false;
	Mesh->BodyInstance.SetInstanceNotifyRBCollision(false);
	Mesh->BodyInstance.SetCollisionProfileName("NoCollision");

	Steering->MaxVelocity = 500.f;
}

void ATestSteeringObstacle::BeginPlay()
{
	Super::BeginPlay();

	UMaterialInstanceDynamic* MaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
	MaterialInstance->SetVectorParameterValue("Color", Color);
	Mesh->SetMaterial(0, MaterialInstance);
	Mesh->SetMaterial(1, MaterialInstance);
	Mesh->SetMaterial(2, MaterialInstance);
	Mesh->SetMaterial(3, MaterialInstance);
	UE_LOG(Generic, Warning, TEXT("SetVectorParameterValue to %s"), *Color.ToString());
}
