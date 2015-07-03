

#include "SpaceRTS.h"
#include "TestObstacleAvoidanceLevelScript.h"
#include "../Steering/TestSteeringObstacle.h"
#include "../Steering/Steering3D.h"


ATestObstacleAvoidanceLevelScript::ATestObstacleAvoidanceLevelScript() :
     Super(),
	 ComputedObstaclesPerFrame(5),
	 FrameSlices(0U)
{

}

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
		StartPosition.Z = 50.f * FMath::SRand() * ((FMath::RandBool()) ? 1.f : -1.f);
		FRotator bla = FRotationMatrix::MakeFromX(TargetPosition - StartPosition).Rotator();
		ATestSteeringObstacle* SteeringObstacle = Cast<ATestSteeringObstacle>(
			GetWorld()->SpawnActor(ATestSteeringObstacle::StaticClass(),
			&StartPosition,
			&bla));
		SteeringObstacle->TargetPosition = TargetPosition;
		SteeringObstacle->Color = FVector(FMath::Abs(FMath::Cos(RadDelta * i)), FMath::Abs(FMath::Sin(RadDelta * i)), FMath::Sin(RadDelta * i));
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

	int32 ObstacleCount = Obstacles.Num();
	FrameSlices = FMath::Max(ObstacleCount / ComputedObstaclesPerFrame, 1);

	int32 CurrentComputationGroup = FrameIndex % FrameSlices;

	int32 ComputationGroupOffset = CurrentComputationGroup * ComputedObstaclesPerFrame;
	for (int32 i = ComputationGroupOffset; i < ObstacleCount && i < ComputationGroupOffset + ComputedObstaclesPerFrame; ++i)
	{
		Obstacles[i]->Steering->ComputeNewVelocity(World, Obstacles[i], DeltaSeconds);
	}
}