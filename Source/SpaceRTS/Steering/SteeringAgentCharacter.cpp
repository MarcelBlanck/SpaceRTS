

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

USteeringAgentComponent* ASteeringAgentCharacter::GetSteeringAgentComponent()
{
	return SteeringAgentComponent;
}

void ASteeringAgentCharacter::CalculatePreferedVelocity()
{
	if (SteeringAgentComponent != nullptr)
	{
		SteeringAgentComponent->CalculatePreferedVelocity();
	}
}