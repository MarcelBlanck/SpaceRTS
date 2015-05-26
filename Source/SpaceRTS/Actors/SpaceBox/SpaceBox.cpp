

#include "SpaceRTS.h"
#include "SpaceBox.h"

ASpaceBox::ASpaceBox(const FObjectInitializer& ObjectInitializer)
	: AActor(ObjectInitializer),
	SpaceBoxSizeKiloMeters(1.f),
	Texture(nullptr),
	FollowActor(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
	SetTickableWhenPaused(false);

	ConstructorHelpers::FObjectFinder<UMaterial> SpaceBoxMatFinder(TEXT("Material'/Game/Actors/SpaceBox/MAT_SpaceBox.MAT_SpaceBox'"));
	checkf(SpaceBoxMatFinder.Object, TEXT("Did not find Material'/Game/Actors/SpaceBox/MAT_SpaceBox.MAT_SpaceBox' - Maybe the asset was moved or renamed."));
	SpaceBoxMaterial = SpaceBoxMatFinder.Object;

	ConstructorHelpers::FObjectFinder<UStaticMesh> SpaceBoxMeshFinder(TEXT("StaticMesh'/Game/Actors/SpaceBox/SM_SpaceBox.SM_SpaceBox'"));
	checkf(SpaceBoxMeshFinder.Object, TEXT("Did not find StaticMesh'/Game/Actors/SpaceBox/SM_SpaceBox.SM_SpaceBox' - Maybe the asset was moved or renamed."));
	SpaceBoxMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("SpaceBoxMesh"));
	SpaceBoxMesh->SetStaticMesh(SpaceBoxMeshFinder.Object);
	SpaceBoxMesh->SetMobility(EComponentMobility::Movable);
	SpaceBoxMesh->bCastDynamicShadow = false;
	SpaceBoxMesh->bTreatAsBackgroundForOcclusion = true;
	SpaceBoxMesh->bGenerateOverlapEvents = 0;
	SpaceBoxMesh->bReceivesDecals = 0;
	SpaceBoxMesh->CastShadow = 0;
	SpaceBoxMesh->bReturnMaterialOnMove = 0;
	SpaceBoxMesh->bAffectDynamicIndirectLighting = 0;
	SpaceBoxMesh->bAffectDistanceFieldLighting = 0;
	SpaceBoxMesh->bCastStaticShadow = 0;
	SpaceBoxMesh->bCastVolumetricTranslucentShadow = 0;
	SpaceBoxMesh->bSelfShadowOnly = 0;
	SpaceBoxMesh->bCastFarShadow = 0;
	SpaceBoxMesh->bCastInsetShadow = 0;
	SpaceBoxMesh->bCastCinematicShadow = 0;
	SpaceBoxMesh->bCastHiddenShadow = 0;
	SpaceBoxMesh->SetSimulatePhysics(false);
	SpaceBoxMesh->bGenerateOverlapEvents = false;
	SpaceBoxMesh->SetEnableGravity(false);
	SpaceBoxMesh->BodyInstance.bAutoWeld = false;
	SpaceBoxMesh->BodyInstance.SetInstanceNotifyRBCollision(false);
	SpaceBoxMesh->BodyInstance.SetCollisionProfileName("NoCollision");
	RootComponent = SpaceBoxMesh;
}

void ASpaceBox::OnConstruction(const FTransform& Transform)
{
	FVector ActorScale(SpaceBoxSizeKiloMeters);
	SetActorScale3D(ActorScale);

	UMaterialInstanceDynamic* MaterialInstance = UMaterialInstanceDynamic::Create(SpaceBoxMaterial, this);
	MaterialInstance->SetTextureParameterValue("SpaceBoxTexture", Texture);
	SpaceBoxMesh->SetMaterial(0, MaterialInstance);
}

void ASpaceBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FollowActor != nullptr)
	{
		SetActorLocation(FollowActor->GetActorLocation());
	}
}

