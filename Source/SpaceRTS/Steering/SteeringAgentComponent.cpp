

#include "SpaceRTS.h"
#include "SteeringAgentComponent.h"
#include "SteeringAgentInterface.h"
#include "Kismet/KismetSystemLibrary.h"

#define EPSILON 0.00001f


USteeringAgentComponent::USteeringAgentComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	MaxVelocity(4000.f),
	ScanRadius(2200.f),
	MaxComputedNeighbors(9),
	TimeHorizon(10.0f),
	IsTargetPositionReachedReported(false),
	FocusActor(nullptr)
{
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	SetSimulatePhysics(false);
	SetEnableGravity(false);
	BodyInstance.bAutoWeld = false;
	BodyInstance.SetCollisionProfileName("RadarVisible");
}

void USteeringAgentComponent::BeginPlay()
{
	Owner = Cast<AActor>(GetOwner());
	check(Cast<ISteeringAgentInterface>(Owner));

	Super::BeginPlay();

	AddTickPrerequisiteActor(GetWorld()->GetLevelScriptActor());
}

void USteeringAgentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (IsSteeringEnabled)
	{
		Velocity = NewVelocity.GetClampedToMaxSize(MaxVelocity);
		if (!Velocity.IsNearlyZero())
		{
			Owner->SetActorLocation(Owner->GetActorLocation() + Velocity * DeltaTime);

			if (FocusActor == nullptr)
			{
				Owner->SetActorRotation(FMath::RInterpTo(Owner->GetActorRotation(),
					FRotationMatrix::MakeFromX(Velocity).Rotator(), DeltaTime, 0.5f));
			}
		}

		if (FocusActor != nullptr)
		{
			Owner->SetActorRotation(FMath::RInterpTo(Owner->GetActorRotation(), 
				FRotationMatrix::MakeFromX(FocusActor->GetActorLocation() -  Owner->GetActorLocation()).Rotator(), DeltaTime, 1.f));
		}
	}
}

void USteeringAgentComponent::EnableSteering()
{
	IsSteeringEnabled = true;
}

void USteeringAgentComponent::DisableSteering()
{
	IsSteeringEnabled = false;
}

void USteeringAgentComponent::SetTargetPosition(const FVector& NewTargetPosition)
{
	TargetPosition = NewTargetPosition;
}

void USteeringAgentComponent::SetFocusActor(AActor* NewFocusActor)
{
	FocusActor = NewFocusActor;
}

void USteeringAgentComponent::ClearFocusActor()
{
	FocusActor = nullptr;
}

void USteeringAgentComponent::SetMaxVelocity(float NewMaxVelocity)
{
	MaxVelocity = NewMaxVelocity;
}

void USteeringAgentComponent::CalculatePreferedVelocity()
{
	if (IsSteeringEnabled)
	{
		FVector TargetVector = TargetPosition - Owner->GetActorLocation();

		if (TargetVector.IsNearlyZero())
		{
			PreferedVelocity.Set(0.f, 0.f, 0.f);

			OnTargetPositionReached.Broadcast();
			IsTargetPositionReachedReported = true;
		}
		else
		{
			float DotToTargetPosition = FVector::DotProduct((TargetVector).GetSafeNormal(), Owner->GetActorForwardVector());
			float SpeedFactor = FMath::Max(((DotToTargetPosition + 1.f) / 2.f), 0.1f);
			PreferedVelocity = (TargetPosition - Owner->GetActorLocation()).GetClampedToMaxSize(MaxVelocity) * SpeedFactor;

			IsTargetPositionReachedReported = false;
		}
	}
	else
	{
		PreferedVelocity.Set(0.f, 0.f, 0.f);
	}	
}

void USteeringAgentComponent::ComputeNewVelocity(UWorld* World, float DeltaTime)
{
	if (!IsSteeringEnabled)
	{
		return;
	}

	TArray<FPlane> Planes;
	const float InvTimeHorizon = 1.0f / TimeHorizon;

	FVector OwnerLocation = Owner->GetActorLocation();
	FVector OwnerVelocity = Velocity;

	// Check for Obstacles in Range using the Radar
	bool PriotitySignatureNear = false;
	TArray<FObstacleProcessingData> HighPrioObstaclesInRange;
	{
		TArray<FObstacleProcessingData> LowPrioObstaclesInRange;

		TArray<FHitResult> RadarHits;
		GetRadarBlipResult(OwnerLocation, World, RadarHits);
		int32 RadarHitCount = RadarHits.Num();
		if (RadarHitCount > 0)
		{
			for (int32 i = 0; i < RadarHitCount; ++i)
			{
				AActor* HitActor = RadarHits[i].Actor.Get();
				ISteeringAgentInterface* SteeringAgent = Cast<ISteeringAgentInterface>(HitActor);
				if (SteeringAgent != nullptr)
				{
					TArray<FObstacleProcessingData>* ObstaclesInRangeList;
					if (SteeringAgent->IsPrioritySignature())
					{
						ObstaclesInRangeList = &HighPrioObstaclesInRange;
					}
					else
					{
						ObstaclesInRangeList = &LowPrioObstaclesInRange;
					}

					uint32 Index = ObstaclesInRangeList->AddUninitialized();
					FObstacleProcessingData* ProcessingData = &(ObstaclesInRangeList->GetData()[Index]);
					ProcessingData->ObstacleActor = HitActor;
					ProcessingData->SteeringAgent = SteeringAgent;
					ProcessingData->RelativePosition = HitActor->GetActorLocation() - OwnerLocation;
					ProcessingData->DistanceSquared = (ProcessingData->RelativePosition).SizeSquared();
					ProcessingData->SignatureDistance = FMath::Sqrt(ProcessingData->DistanceSquared) - SphereRadius - SteeringAgent->GetSphereRadius();
				}
			}
		}

		HighPrioObstaclesInRange.Sort(USteeringAgentComponent::SortBySignatureDistance);
		if (HighPrioObstaclesInRange.Num() < MaxComputedNeighbors)
		{
			LowPrioObstaclesInRange.Sort(USteeringAgentComponent::SortBySignatureDistance);
			HighPrioObstaclesInRange.Append(LowPrioObstaclesInRange);
		}
	}

	int32 ActualObstaclesToCompute = FMath::Min(MaxComputedNeighbors, HighPrioObstaclesInRange.Num());
	
	for (int32 i = 0; i < ActualObstaclesToCompute; ++i)
	{
		const FObstacleProcessingData& Obstacle = HighPrioObstaclesInRange[i];

		const FVector RelativePosition = Obstacle.RelativePosition;
		const FVector RelativeVelocity = OwnerVelocity - Obstacle.SteeringAgent->GetSteeringVelocity();
		const float DistanceSquared = Obstacle.DistanceSquared;
		const float CombinedRadius = SphereRadius + Obstacle.SteeringAgent->GetSphereRadius();
		const float CombinedRadiusSq = CombinedRadius * CombinedRadius;

		FPlane Plane;
		FVector U(ForceInitToZero);

		if (DistanceSquared > CombinedRadiusSq) {
			// No collision
			const FVector W = RelativeVelocity - InvTimeHorizon * RelativePosition;
			// Vector from cutoff center to relative velocity
			const float LengthSqW = W.SizeSquared();

			const float DotProduct = FVector::DotProduct(W, RelativePosition);

			if (DotProduct < 0.0f && FMath::Square(DotProduct) > CombinedRadiusSq * LengthSqW)
			{
				/* Project on cut-off circle. */
				const float LengthW = FMath::Sqrt(LengthSqW);
				const FVector NormalizedW = W / LengthW;

				Plane.Normal = NormalizedW;
				U = (CombinedRadius * InvTimeHorizon - LengthW) * NormalizedW;
			}
			else
			{
				/* Project on cone. */
				const float A = DistanceSquared;
				const float B = FVector::DotProduct(RelativePosition, RelativeVelocity);
				const float C = RelativeVelocity.SizeSquared() - FVector::CrossProduct(RelativePosition, RelativeVelocity).SizeSquared() / (DistanceSquared - CombinedRadiusSq);
				const float T = (B + FMath::Sqrt(B * B - A * C)) / A;
				const FVector W = RelativeVelocity - T * RelativePosition;
				const float LengthW = W.Size();
				const FVector NormalizedW = W / LengthW;

				Plane.Normal = NormalizedW;
				U = (CombinedRadius * T - LengthW) * NormalizedW;
			}
		}
		else
		{
			// Collision occured, back away
			const float InvDeltaTime = 1.0f / DeltaTime;
			const FVector W = RelativeVelocity - InvDeltaTime * RelativePosition;
			const float LengthW = W.Size();
			const FVector NormalizedW = W / LengthW;

			Plane.Normal = NormalizedW;
			U = (CombinedRadius * InvDeltaTime - LengthW) * NormalizedW;
		}

		Plane.Point = OwnerVelocity + 0.5f * U;
		Planes.Add(Plane);
	}

	// Start ROV2 algorithm calculations		
	const int32 PlaneFail = ROV2_LinearProgram3(Planes, MaxVelocity, PreferedVelocity, false, NewVelocity);

	if (PlaneFail < Planes.Num())
	{
		ROV2_LinearProgram4(Planes, PlaneFail, MaxVelocity, NewVelocity);
	}
}

bool USteeringAgentComponent::GetRadarBlipResult(FVector const & OwnerLocation, UWorld* World, TArray<FHitResult>& OutHits)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Owner);
	return UKismetSystemLibrary::SphereTraceMulti_NEW(World, OwnerLocation, OwnerLocation + Owner->GetActorForwardVector(), ScanRadius,
				ETraceTypeQuery::TraceTypeQuery3, false, ActorsToIgnore, EDrawDebugTrace::None, OutHits, false);

}

bool USteeringAgentComponent::ROV2_LinearProgram1(const TArray<FPlane> &Planes, int32 PlaneNo, const FLine &Line, float Radius, const FVector &OptVelocity, bool DirectionOpt, FVector &Result)
{
	const float DotProduct = FVector::DotProduct(Line.Point, Line.Direction);
	const float Discriminant = FMath::Square(DotProduct) + FMath::Square(Radius) - Line.Point.SizeSquared();

	if (Discriminant < 0.0f)
	{
		// Max speed sphere fully invalidates line
		return false;
	}

	const float SqrtDiscriminant = FMath::Sqrt(Discriminant);
	float LeftT = -DotProduct - SqrtDiscriminant;
	float RightT = -DotProduct + SqrtDiscriminant;

	for (int32 i = 0; i < PlaneNo; ++i)
	{
		const float Numerator = FVector::DotProduct((Planes[i].Point - Line.Point), Planes[i].Normal);
		const float Denominator = FVector::DotProduct(Line.Direction, Planes[i].Normal);

		if (FMath::Square(Denominator) <= EPSILON)
		{
			// Lines line is (almost) parallel to plane i
			if (Numerator > 0.0f)
			{
				return false;
			}
			else
			{
				continue;
			}
		}

		const float T = Numerator / Denominator;

		if (Denominator >= 0.0f)
		{
			// Plane i bounds line on the left
			LeftT = FMath::Max(LeftT, T);
		}
		else
		{
			// Plane i bounds line on the right
			RightT = FMath::Max(RightT, T);
		}

		if (LeftT > RightT)
		{
			return false;
		}
	}

	if (DirectionOpt)
	{
		// Optimize direction
		if (FVector::DotProduct(OptVelocity, Line.Direction) > 0.0f)
		{
			// Take right extreme
			Result = Line.Point + RightT * Line.Direction;
		}
		else
		{
			// Take left extreme
			Result = Line.Point + LeftT * Line.Direction;
		}
	}
	else
	{
		// Optimize closest Point
		const float T = FVector::DotProduct(Line.Direction, (OptVelocity - Line.Point));

		if (T < LeftT)
		{
			Result = Line.Point + LeftT * Line.Direction;
		}
		else if (T > RightT)
		{
			Result = Line.Point + RightT * Line.Direction;
		}
		else
		{
			Result = Line.Point + T * Line.Direction;
		}
	}

	return true;
}

bool USteeringAgentComponent::ROV2_LinearProgram2(const TArray<FPlane> &Planes, int32 PlaneNo, float Radius, const FVector &OptVelocity, bool DirectionOpt, FVector &Result)
{
	const float PlaneDist = FVector::DotProduct(Planes[PlaneNo].Point, Planes[PlaneNo].Normal);
	const float PlaneDistSq = FMath::Square(PlaneDist);
	const float RadiusSq = FMath::Square(Radius);

	if (PlaneDistSq > RadiusSq)
	{
		// Max speed sphere fully invalidates plane PlaneNo
		return false;
	}

	const float PlaneRadiusSq = RadiusSq - PlaneDistSq;

	const FVector PlaneCenter = PlaneDist * Planes[PlaneNo].Normal;

	if (DirectionOpt)
	{
		/* Project direction optVelocity on plane PlaneNo. */
		const FVector PlaneOptVelocity = OptVelocity - FVector::DotProduct(OptVelocity, Planes[PlaneNo].Normal) * Planes[PlaneNo].Normal;
		const float PlaneOptVelocityLengthSq = PlaneOptVelocity.SizeSquared();

		if (PlaneOptVelocityLengthSq <= EPSILON)
		{
			Result = PlaneCenter;
		}
		else
		{
			Result = PlaneCenter + FMath::Sqrt(PlaneRadiusSq / PlaneOptVelocityLengthSq) * PlaneOptVelocity;
		}
	}
	else
	{
		// Project Point optVelocity on plane PlaneNo
		Result = OptVelocity + FVector::DotProduct((Planes[PlaneNo].Point - OptVelocity), Planes[PlaneNo].Normal) * Planes[PlaneNo].Normal;

		// If outside planeCircle, project on planeCircle
		if (Result.SizeSquared() > RadiusSq)
		{
			const FVector PlaneResult = Result - PlaneCenter;
			const float PlaneResultLengthSq = PlaneResult.SizeSquared();
			Result = PlaneCenter + FMath::Sqrt(PlaneRadiusSq / PlaneResultLengthSq) * PlaneResult;
		}
	}

	for (int32 i = 0; i < PlaneNo; ++i)
	{
		if (FVector::DotProduct(Planes[i].Normal, (Planes[i].Point - Result)) > 0.0f)
		{
			/* Result does not satisfy constraint i. Compute new optimal result.
			* Compute intersection line of plane i and plane PlaneNo. */
			FVector CrossProduct = FVector::CrossProduct(Planes[i].Normal, Planes[PlaneNo].Normal);

			if (CrossProduct.SizeSquared() <= EPSILON)
			{
				// Planes PlaneNo and i are (almost) parallel, and plane i fully invalidates plane PlaneNo
				return false;
			}

			FLine Line;
			Line.Direction = CrossProduct.GetSafeNormal();
			const FVector LineNormal = FVector::CrossProduct(Line.Direction, Planes[PlaneNo].Normal);
			Line.Point = Planes[PlaneNo].Point + (FVector::DotProduct((Planes[i].Point - Planes[PlaneNo].Point), Planes[i].Normal) / FVector::DotProduct(LineNormal, Planes[i].Normal)) * LineNormal;

			if (!ROV2_LinearProgram1(Planes, i, Line, Radius, OptVelocity, DirectionOpt, Result)) {
				return false;
			}
		}
	}

	return true;
}

int32 USteeringAgentComponent::ROV2_LinearProgram3(const TArray<FPlane> &Planes, float Radius, const FVector &OptVelocity, bool DirectionOpt, FVector &Result)
{
	if (DirectionOpt)
	{
		// Optimize direction. Note that the optimization velocity is of unit length in this case
		Result = OptVelocity * Radius;
	}
	else if (OptVelocity.SizeSquared() > FMath::Square(Radius))
	{
		// Optimize closest Point and outside circle
		Result = OptVelocity.GetSafeNormal() * Radius;
	}
	else
	{
		// Optimize closest Point and inside circle
		Result = OptVelocity;
	}

	int32 PlaneCount = Planes.Num();
	for (size_t i = 0; i < PlaneCount; ++i)
	{
		if (FVector::DotProduct(Planes[i].Normal, (Planes[i].Point - Result)) > 0.0f)
		{
			// Result does not satisfy constraint i. Compute new optimal result
			const FVector TempResult = Result;

			if (!ROV2_LinearProgram2(Planes, i, Radius, OptVelocity, DirectionOpt, Result))
			{
				Result = TempResult;
				return i;
			}
		}
	}

	return Planes.Num();
}

void USteeringAgentComponent::ROV2_LinearProgram4(const TArray<FPlane> &Planes, int32 BeginPlane, float Radius, FVector &Result)
{
	float Distance = 0.0f;

	int32 PlaneCount = Planes.Num();
	for (int32 i = BeginPlane; i < PlaneCount; ++i)
	{
		if (FVector::DotProduct(Planes[i].Normal, (Planes[i].Point - Result)) > Distance)
		{
			// Result does not satisfy constraint of plane i
			TArray<FPlane> ProjPlanes;

			for (int32 j = 0; j < i; ++j)
			{
				FPlane Plane;

				const FVector CrossProduct = FVector::CrossProduct(Planes[j].Normal, Planes[i].Normal);

				if (CrossProduct.SizeSquared() <= EPSILON)
				{
					/* Plane i and plane j are (almost) parallel. */
					if (FVector::DotProduct(Planes[i].Normal, Planes[j].Normal) > 0.0f)
					{
						// Plane i and plane j Point in the same direction
						continue;
					}
					else
					{
						// Plane i and plane j Point in opposite direction
						Plane.Point = 0.5f * (Planes[i].Point + Planes[j].Point);
					}
				}
				else
				{
					// Plane.Point is Point on line of intersection between plane i and plane j
					const FVector LineNormal = FVector::CrossProduct(CrossProduct, Planes[i].Normal);
					Plane.Point = Planes[i].Point + (FVector::DotProduct((Planes[j].Point - Planes[i].Point), Planes[j].Normal) / FVector::DotProduct(LineNormal, Planes[j].Normal)) * LineNormal;
				}

				Plane.Normal = (Planes[j].Normal - Planes[i].Normal).GetSafeNormal();
				ProjPlanes.Add(Plane);
			}

			const FVector TempResult = Result;

			if (ROV2_LinearProgram3(ProjPlanes, Radius, Planes[i].Normal, true, Result) < ProjPlanes.Num())
			{
				/* This should in principle not happen.
				* The result is by definition already in the feasible region of this linear program.
				* If it fails, it is due to small floating Point error, and the current result is kept. */
				Result = TempResult;
			}

			Distance = FVector::DotProduct(Planes[i].Normal, (Planes[i].Point - Result));
		}
	}
}


