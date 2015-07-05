

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

USteeringAgentComponent* ASteeringAgentActor::GetSteeringAgentComponent()
{
	return SteeringAgentComponent;
}

void ASteeringAgentActor::CalculatePreferedVelocity()
{
	if (SteeringAgentComponent != nullptr)
	{
		SteeringAgentComponent->CalculatePreferedVelocity();
	}
}