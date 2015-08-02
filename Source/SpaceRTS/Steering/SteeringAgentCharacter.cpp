

#include "SpaceRTS.h"
#include "SteeringAgentCharacter.h"
#include "SteeringLevelScriptActor.h"

ASteeringAgentCharacter::ASteeringAgentCharacter(const FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer)
{
	SteeringAgentComponent = ObjectInitializer.CreateDefaultSubobject<USteeringAgentComponent>(this, TEXT("SteeringAgentComponent"));
	SteeringAgentComponent->DisableSteering();
	SteeringAgentComponent->AttachTo(RootComponent);
}

void ASteeringAgentCharacter::BeginPlay()
{
	Super::BeginPlay();

	ASteeringLevelScriptActor* SteeringLevelScript = Cast<ASteeringLevelScriptActor>(GetWorld()->GetLevelScriptActor());
	if (SteeringLevelScript != nullptr)
	{
		SteeringLevelScript->RegisterSteeringAgent(this);
	}
}

void ASteeringAgentCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ASteeringLevelScriptActor* SteeringLevelScript = Cast<ASteeringLevelScriptActor>(GetWorld()->GetLevelScriptActor());
	if (SteeringLevelScript != nullptr)
	{
		SteeringLevelScript->UnregisterSteeringAgent(this);
	}
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