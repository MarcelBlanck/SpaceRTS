

#include "SpaceRTS.h"
#include "TestObstacleAvoidanceLevelScript.h"
#include "../Actors/PlayerFregatte/PlayerFregatte.h"

void ATestObstacleAvoidanceLevelScript::StartWallTest(FVector Center, float Distance, int32 Rows, int32 Columns)
{

}

void ATestObstacleAvoidanceLevelScript::StartCircularTest(FVector Center, float Radius, int32 Segments)
{
	UWorld* World = GetWorld();
	check(World);
	
	float RadDelta = 2 * PI / Segments;
	float CircleX;
	float CircleY;
	FVector StartPosition(Center);
	FVector TargetPosition(Center);

	for (int32 i = 0; i < Segments; ++i)
	{
		CircleX = Radius * FMath::Cos(RadDelta * i);
		CircleY = Radius * FMath::Sin(RadDelta * i);

		StartPosition.X = Center.X + CircleX;
		StartPosition.Y = Center.Y + CircleY;
		StartPosition.Z = 50.f * FMath::SRand() * ((FMath::RandBool()) ? 1.f : -1.f);
		TargetPosition.X = Center.X - Radius * FMath::Cos(RadDelta * (i + 1));
		TargetPosition.Y = Center.Y - Radius * FMath::Sin(RadDelta * (i + 1));	

		FRotator LookDirection = FRotationMatrix::MakeFromX(TargetPosition - StartPosition).Rotator();
		ISteeringAgentInterface* SteeringAgent = Cast<ISteeringAgentInterface>(
			World->SpawnActor(APlayerFregatte::StaticClass(),
							  &StartPosition,
							  &LookDirection));
		USteeringAgentComponent* SteeringAgentComponent = SteeringAgent->GetSteeringAgentComponent();
		if (SteeringAgentComponent != nullptr)
		{
			SteeringAgentComponent->SetTargetPosition(TargetPosition);
		}
	}
}

void ATestObstacleAvoidanceLevelScript::StartSphereTest(FVector Center, float Radius, int32 Segments)
{
	UWorld* World = GetWorld();
	check(World);

	FVector PositionOnSphere;
	FVector StartPosition(Center);
	FVector TargetPosition(Center);

	FRotator LookDirection;
	ISteeringAgentInterface* SteeringAgent;
	USteeringAgentComponent* SteeringAgentComponent;

	float DeltaTheta = PI / Segments;
	float DeltaPhi = (2 * PI) / Segments;

	for (int j = 1; j < Segments; j++)
	{
		float Theta = DeltaTheta * j;

		for (int i = 0; i < Segments; i++)
		{
			float Phi = DeltaPhi * i;

			PositionOnSphere.X = FMath::Sin(Theta) * FMath::Cos(Phi) * Radius;
			PositionOnSphere.Y = FMath::Sin(Theta) * FMath::Sin(Phi) * Radius,
			PositionOnSphere.Z = FMath::Cos(Theta) * Radius;

			StartPosition = Center + PositionOnSphere;
			TargetPosition = Center - PositionOnSphere;

			LookDirection = FRotationMatrix::MakeFromX(TargetPosition - StartPosition).Rotator();
			SteeringAgent = Cast<ISteeringAgentInterface>(World->SpawnActor(APlayerFregatte::StaticClass(), &StartPosition, &LookDirection));

			SteeringAgentComponent = SteeringAgent->GetSteeringAgentComponent();
			if (SteeringAgentComponent != nullptr)
			{
				SteeringAgentComponent->SetTargetPosition(TargetPosition);
			}
		}
	}
}