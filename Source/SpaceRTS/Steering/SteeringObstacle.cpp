// Pyrite Softworks 2015

#include "SpaceRTS.h"
#include "SteeringObstacle.h"

#include "Steering3D.h"


// Sets default values
ASteeringObstacle::ASteeringObstacle(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	RadarSignature(nullptr),
	Steering(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	RadarSignature = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("RadarSignature"));
	RadarSignature->SetSimulatePhysics(false);
	RadarSignature->SetEnableGravity(false);
	RadarSignature->BodyInstance.bAutoWeld = false;
	RadarSignature->BodyInstance.SetCollisionProfileName("Obstacle");
	RadarSignature->AttachTo(RootComponent);

	Steering = ObjectInitializer.CreateDefaultSubobject<USteering3D>(this, TEXT("SteeringObstacle"));
}

void ASteeringObstacle::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FVector Origin; FVector BoxExtent;
	GetActorBounds(false, Origin, BoxExtent);
	SignatureRadius = 120.0f; // FMath::Sqrt(BoxExtent.X * BoxExtent.X + BoxExtent.Y * BoxExtent.Y + BoxExtent.Z * BoxExtent.Z) / Transform.GetMaximumAxisScale();
	RadarSignature->SetSphereRadius(SignatureRadius, false);
	UE_LOG(Generic, Warning, TEXT("Setting RadarSignature Radius to %f"), SignatureRadius);
}

//UE_LOG(Generic, Warning, TEXT("Steering1: %s %f"), *Steering.ToString(), Steering.Size());
//DrawDebugLine(GetWorld(), OwnerPosition, OwnerPosition + 10000 * Steering, FColor::Cyan);

void ASteeringObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}