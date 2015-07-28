

#include "SpaceRTS.h"
#include "TestObstacleAvoidanceLevelScript.h"
#include "../Actors/PlayerFregatte/PlayerFregatte.h"

void ATestObstacleAvoidanceLevelScript::StartWallTest(FVector Center, float Distance, int32 Rows, float RowDistance, int32 Columns, float ColumnDistance)
{
	UWorld* World = GetWorld();
	check(World);

	DestroyAllSpawnedActors();
	
	FVector LeftPosition;
	FVector RightPosition;

	float HalfWallWidth = Columns * ColumnDistance / 2.f;
	float HalsWallHeight = Rows * RowDistance / 2.f;

	for (int32 Row = 0; Row < Rows; ++Row)
	{
		for (int32 Column = 0; Column < Columns; ++Column)
		{
			LeftPosition.X = Center.X - HalfWallWidth + Row * RowDistance;
			LeftPosition.Y = Center.Y + Distance / 2; 
			LeftPosition.Z = Center.Z - HalsWallHeight + Column * ColumnDistance;

			RightPosition.X = Center.X - HalfWallWidth + Row * RowDistance;
			RightPosition.Y = Center.Y - Distance / 2;
			RightPosition.Z = Center.Z - HalsWallHeight + Column * ColumnDistance;

			FRotator LookDirection = FRotationMatrix::MakeFromX(RightPosition - LeftPosition).Rotator();
			ISteeringAgentInterface* SteeringAgent = Cast<ISteeringAgentInterface>(
				World->SpawnActor(APlayerFregatte::StaticClass(),
				&LeftPosition,
				&LookDirection));

			SpawnedActors.Add(Cast<AActor>(SteeringAgent));

			USteeringAgentComponent* SteeringAgentComponent = SteeringAgent->GetSteeringAgentComponent();
			if (SteeringAgentComponent != nullptr)
			{
				SteeringAgentComponent->SetTargetPosition(RightPosition);
			}



			LookDirection = FRotationMatrix::MakeFromX(LeftPosition - RightPosition).Rotator();
			SteeringAgent = Cast<ISteeringAgentInterface>(
				World->SpawnActor(APlayerFregatte::StaticClass(),
				&RightPosition,
				&LookDirection));

			SpawnedActors.Add(Cast<AActor>(SteeringAgent));

			SteeringAgentComponent = SteeringAgent->GetSteeringAgentComponent();
			if (SteeringAgentComponent != nullptr)
			{
				SteeringAgentComponent->SetTargetPosition(LeftPosition);
			}
		}
	}
}

void ATestObstacleAvoidanceLevelScript::StartCircularTest(FVector Center, float Radius, int32 Segments)
{
	UWorld* World = GetWorld();
	check(World);
	
	DestroyAllSpawnedActors();

	FActorSpawnParameters SpawnParams;
	SpawnParams.OverrideLevel = GetLevel();

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
							  &LookDirection, 
							  SpawnParams));
		
		SpawnedActors.Add(Cast<AActor>(SteeringAgent));

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

	DestroyAllSpawnedActors();

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

			SpawnedActors.Add(Cast<AActor>(SteeringAgent));

			SteeringAgentComponent = SteeringAgent->GetSteeringAgentComponent();
			if (SteeringAgentComponent != nullptr)
			{
				SteeringAgentComponent->SetTargetPosition(TargetPosition);
			}
		}
	}
}

void ATestObstacleAvoidanceLevelScript::DestroyAllSpawnedActors()
{
	for (AActor* SpawnedActor : SpawnedActors)
	{
		if (SpawnedActor == nullptr) continue;
		SpawnedActor->Destroy();
	}
	SpawnedActors.RemoveAll([](const AActor* Ptr){ return true; });
}

/*
AActor* ATestObstacleAvoidanceLevelScript::SpawnActorIntoLevel(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, FName Level, FVector Location, FRotator Rotation, bool SpawnEvenIfColliding)
{
	if (!ActorClass) return NULL;

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject);
	check(World);


	FActorSpawnParameters SpawnParameters;
	SpawnParameters.bNoCollisionFail = SpawnEvenIfColliding;
	SpawnParameters.bDeferConstruction = false;


	ULevel* FoundLevel = NULL;

	for (const ULevelStreaming* Level : World->StreamingLevels)
	{
		if (!Level) continue;

		ULevel* LevelPtr = Level->GetLoadedLevel();
		if (!LevelPtr) continue;

		if (Level->GetWorldAssetPackageFName() == Level)
		{
			FoundLevel = LevelPtr;
			break;
		}
	}

	if (FoundLevel)
	{
		SpawnParameters.OverrideLevel = FoundLevel;
	}

	return World->SpawnActor(ActorClass, &Location, &Rotation, SpawnParameters);
}
*/