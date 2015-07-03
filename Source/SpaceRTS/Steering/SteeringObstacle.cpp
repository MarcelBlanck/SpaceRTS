// Pyrite Softworks 2015

#include "SpaceRTS.h"
#include "SteeringObstacle.h"

#include "Steering3D.h"


// Sets default values
ASteeringObstacle::ASteeringObstacle(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	RadarSignature(nullptr),
	Steering(nullptr),
	SignatureRadius(1.f),
	IsPrioritySignature(false)
{
	PrimaryActorTick.bCanEverTick = true;

	Scene = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Scene"));
	SetRootComponent(Scene);

	RadarSignature = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("RadarSignature"));
	RadarSignature->SetSimulatePhysics(false);
	RadarSignature->SetEnableGravity(false);
	RadarSignature->BodyInstance.bAutoWeld = false;
	RadarSignature->BodyInstance.SetCollisionProfileName("RadarVisible");
	RadarSignature->AttachTo(RootComponent);

	Steering = ObjectInitializer.CreateDefaultSubobject<USteering3D>(this, TEXT("Steering"));
}

void ASteeringObstacle::OnConstruction(const FTransform& Transform)
{
	RadarSignature->SetSphereRadius(SignatureRadius, false);
	TargetPosition = FVector(ForceInitToZero);
	Super::OnConstruction(Transform);
}