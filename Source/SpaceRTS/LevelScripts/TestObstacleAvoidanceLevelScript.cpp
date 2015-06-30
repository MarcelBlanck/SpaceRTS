

#include "SpaceRTS.h"
#include "TestObstacleAvoidanceLevelScript.h"
#include "../Steering/TestSteeringObstacle.h"
#include "../Steering/Steering3D.h"

void ATestObstacleAvoidanceLevelScript::StartCircularTest(FVector Center, float Radius, int32 ObjectCount)
{
	float RadDelta = 2 * PI / ObjectCount;
	float CircleX;
	float CircleY;
	FVector StartPosition(Center);
	FVector TargetPosition(Center);

	for (int32 i = 0; i < ObjectCount; ++i)
	{
		CircleX = Radius * FMath::Cos(RadDelta * i);
		CircleY = Radius * FMath::Sin(RadDelta * i);
		StartPosition.X = Center.X + CircleX;
		StartPosition.Y = Center.Y + CircleY;
		TargetPosition.X = Center.X - Radius * FMath::Cos(RadDelta * (i + 1));
		TargetPosition.Y = Center.Y - Radius * FMath::Sin(RadDelta * (i + 1));
		FRotator bla = FRotationMatrix::MakeFromX(TargetPosition - StartPosition).Rotator();
		ATestSteeringObstacle* SteeringObstacle = Cast<ATestSteeringObstacle>(
			GetWorld()->SpawnActor(ATestSteeringObstacle::StaticClass(),
			&StartPosition,
			&bla));
		SteeringObstacle->TargetPosition = TargetPosition;
		SteeringObstacle->Color = FVector(FMath::Cos(RadDelta * i), FMath::Sin(RadDelta * i), 0.f);
		Obstacles.Add(SteeringObstacle);
	}
	

}

void ATestObstacleAvoidanceLevelScript::BeginPlay()
{
	Super::BeginPlay();
}

void ATestObstacleAvoidanceLevelScript::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	for (ASteeringObstacle* Obstacle : Obstacles)
	{
		Obstacle->PreferedVelocity = (Obstacle->TargetPosition - Obstacle->GetActorLocation()).GetClampedToMaxSize(Obstacle->Steering->MaxVelocity);
	}

	UWorld* World = GetWorld();
	if (World == nullptr)
		return;

	for (ASteeringObstacle* Obstacle : Obstacles)
	{
		Obstacle->Steering->ComputeNewVelocity(World, Obstacle, DeltaSeconds);
	}
}