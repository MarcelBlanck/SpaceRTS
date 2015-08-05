

#include "SpaceRTS.h"
#include "SteeringAgentActor.h"


ASteeringAgentActor::ASteeringAgentActor(const FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);

	Scene = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Scene"));
	SetRootComponent(Scene);

	SteeringAgentComponent = ObjectInitializer.CreateDefaultSubobject<USteeringAgentComponent>(this, TEXT("SteeringAgentComponent"));
	SteeringAgentComponent->AttachTo(RootComponent);
}

void ASteeringAgentActor::BeginPlay()
{
	Super::BeginPlay();

	SteeringAgentComponent->DisableSteering();
}

void ASteeringAgentActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Due to performance reasons no shared/wseak pointers are used for the steering backend.
	// So to be on the save side, the levelscript should definitely know the actor has gone.
	// DisableSteering will indicate that, if steering was activated in the meantime.
	SteeringAgentComponent->DisableSteering(); 
}

void ASteeringAgentActor::SetTargetPosition(const FVector& TargetPosition)
{
	SteeringAgentComponent->SetTargetPosition(TargetPosition);
}

void ASteeringAgentActor::CalculatePreferedVelocity()
{
	SteeringAgentComponent->CalculatePreferedVelocity();
}

void ASteeringAgentActor::ComputeNewVelocity(UWorld* World, float DeltaTime)
{
	SteeringAgentComponent->ComputeNewVelocity(World, DeltaTime);
}

const FVector& ASteeringAgentActor::GetSteeringVelocity()
{
	return SteeringAgentComponent->GetVelocity();
}

bool ASteeringAgentActor::IsPrioritySignature()
{
	return SteeringAgentComponent->GetIsPrioritySignature();
}

float ASteeringAgentActor::GetSphereRadius()
{
	return SteeringAgentComponent->GetSphereRadius();
}