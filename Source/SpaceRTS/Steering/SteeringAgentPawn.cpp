#include "SpaceRTS.h"
#include "SteeringAgentPawn.h"
#include "SteeringLevelScriptActor.h"

ASteeringAgentPawn::ASteeringAgentPawn(const FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	Scene = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Scene"));
	SetRootComponent(Scene);

	SteeringAgentComponent = ObjectInitializer.CreateDefaultSubobject<USteeringAgentComponent>(this, TEXT("SteeringAgentComponent"));
	SteeringAgentComponent->AttachTo(RootComponent);
}

void ASteeringAgentPawn::BeginPlay()
{
	Super::BeginPlay();

	SteeringAgentComponent->EnableSteering();
}

void ASteeringAgentPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Due to performance reasons no shared/wseak pointers are used for the steering backend.
	// So to be on the save side, the levelscript should definitely know the actor has gone.
	// DisableSteering will indicate that, if steering was activated in the meantime.
	SteeringAgentComponent->DisableSteering();
}

void ASteeringAgentPawn::SetTargetPosition(const FVector& TargetPosition)
{
	SteeringAgentComponent->SetTargetPosition(TargetPosition);
}

void ASteeringAgentPawn::CalculatePreferedVelocity()
{
	SteeringAgentComponent->CalculatePreferedVelocity();
}

void ASteeringAgentPawn::ComputeNewVelocity(UWorld* World, float DeltaTime)
{
	SteeringAgentComponent->ComputeNewVelocity(World, DeltaTime);
}

const FVector& ASteeringAgentPawn::GetSteeringVelocity()
{
	return SteeringAgentComponent->GetVelocity();
}

bool ASteeringAgentPawn::IsPrioritySignature()
{
	return SteeringAgentComponent->GetIsPrioritySignature();
}

float ASteeringAgentPawn::GetSphereRadius()
{
	return SteeringAgentComponent->GetSphereRadius();
}