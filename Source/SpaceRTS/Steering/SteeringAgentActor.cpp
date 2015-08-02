

#include "SpaceRTS.h"
#include "SteeringAgentActor.h"
#include "SteeringLevelScriptActor.h"


ASteeringAgentActor::ASteeringAgentActor(const FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	Scene = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Scene"));
	SetRootComponent(Scene);

	SteeringAgentComponent = ObjectInitializer.CreateDefaultSubobject<USteeringAgentComponent>(this, TEXT("SteeringAgentComponent"));
	SteeringAgentComponent->DisableSteering();
	SteeringAgentComponent->AttachTo(RootComponent);
}

void ASteeringAgentActor::BeginPlay()
{
	Super::BeginPlay();

	ASteeringLevelScriptActor* SteeringLevelScript = Cast<ASteeringLevelScriptActor>(GetWorld()->GetLevelScriptActor());
	if (SteeringLevelScript != nullptr)
	{
		SteeringLevelScript->RegisterSteeringAgent(this);
	}
}

void ASteeringAgentActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ASteeringLevelScriptActor* SteeringLevelScript = Cast<ASteeringLevelScriptActor>(GetWorld()->GetLevelScriptActor());
	if (SteeringLevelScript != nullptr)
	{
		SteeringLevelScript->UnregisterSteeringAgent(this);
	}
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