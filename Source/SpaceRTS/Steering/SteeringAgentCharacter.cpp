

#include "SpaceRTS.h"
#include "SteeringAgentCharacter.h"
#include "SteeringLevelScriptActor.h"

ASteeringAgentCharacter::ASteeringAgentCharacter(const FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	SteeringAgentComponent = ObjectInitializer.CreateDefaultSubobject<USteeringAgentComponent>(this, TEXT("SteeringAgentComponent"));
	SteeringAgentComponent->AttachTo(RootComponent);
}

void ASteeringAgentCharacter::BeginPlay()
{
	Super::BeginPlay();

	SteeringAgentComponent->EnableSteering();
}

void ASteeringAgentCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Due to performance reasons no shared/wseak pointers are used for the steering backend.
	// So to be on the save side, the levelscript should definitely know the actor has gone.
	// DisableSteering will indicate that, if steering was activated in the meantime.
	SteeringAgentComponent->DisableSteering();
}

void ASteeringAgentCharacter::SetTargetPosition(const FVector& TargetPosition)
{
	SteeringAgentComponent->SetTargetPosition(TargetPosition);
}

void ASteeringAgentCharacter::CalculatePreferedVelocity()
{
	SteeringAgentComponent->CalculatePreferedVelocity();
}

void ASteeringAgentCharacter::ComputeNewVelocity(UWorld* World, float DeltaTime)
{
	SteeringAgentComponent->ComputeNewVelocity(World, DeltaTime);
}

const FVector& ASteeringAgentCharacter::GetSteeringVelocity()
{
	return SteeringAgentComponent->GetVelocity();
}

bool ASteeringAgentCharacter::IsPrioritySignature()
{
	return SteeringAgentComponent->GetIsPrioritySignature();
}

float ASteeringAgentCharacter::GetSphereRadius()
{
	return SteeringAgentComponent->GetSphereRadius();
}