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
	SteeringAgentComponent->EnableSteering();
	SteeringAgentComponent->AttachTo(RootComponent);
}

void ASteeringAgentPawn::BeginPlay()
{
	Super::BeginPlay();

	ASteeringLevelScriptActor* SteeringLevelScript = Cast<ASteeringLevelScriptActor>(GetWorld()->GetLevelScriptActor());
	if (SteeringLevelScript != nullptr)
	{
		SteeringLevelScript->RegisterSteeringAgent(this);
	}
}

void ASteeringAgentPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ASteeringLevelScriptActor* SteeringLevelScript = Cast<ASteeringLevelScriptActor>(GetWorld()->GetLevelScriptActor());
	if (SteeringLevelScript != nullptr)
	{
		SteeringLevelScript->UnregisterSteeringAgent(this);
	}
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