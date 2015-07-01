// Pyrite Softworks 2015

#include "SpaceRTS.h"
#include "Steering3D.h"

#include "SteeringObstacle.h"
#include "Kismet/KismetSystemLibrary.h"

#define RVO_EPSILON 0.00001f


USteering3D::USteering3D(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
    MaxVelocity(4000.f),
	ScanRadius(2200.f),
	MaxComputedNeighbors(10),
	TimeHorizon(10.0f)
{
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void USteering3D::BeginPlay()
{
	Super::BeginPlay();
	AddTickPrerequisiteActor(GetWorld()->GetLevelScriptActor());
}

void USteering3D::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	ASteeringObstacle* Owner = Cast<ASteeringObstacle>(GetOwner());
	if (Owner == nullptr)
		return;

	Owner->Velocity = Owner->NewVelocity.GetClampedToMaxSize(MaxVelocity);
	FMath::RInterpTo(Owner->GetActorRotation(), FRotationMatrix::MakeFromX(Owner->Velocity).Rotator(), DeltaTime, 1.0f);
	Owner->SetActorLocation(Owner->GetActorLocation() + Owner->Velocity * DeltaTime);
}

bool USteering3D::GetRadarBlipResult(FVector const & OwnerLocation, AActor* Owner, UWorld* World, TArray<FHitResult>& OutHits)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Owner);
	return UKismetSystemLibrary::SphereTraceMulti_NEW(World, OwnerLocation, OwnerLocation + Owner->GetActorForwardVector(), ScanRadius, ETraceTypeQuery::TraceTypeQuery3, false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, OutHits, false);
}

void USteering3D::ComputeNewVelocity(UWorld* World, ASteeringObstacle* Owner, float DeltaTime)
{
	TArray<FPlane> Planes;
	const float InvTimeHorizon = 1.0f / TimeHorizon;

	FVector OwnerLocation = Owner->GetActorLocation();
	FVector OwnerVelocity = Owner->Velocity;

	// Check for Obstacles in Range using the Radar
	TArray<TPair<ASteeringObstacle*, float>> ObstaclesWithSquaredDistance;
	int ObstaclesInRangeCount = 0;
	{	
		TArray<FHitResult> RadarHits;
		GetRadarBlipResult(OwnerLocation, Cast<AActor>(Owner), World, RadarHits);
		ObstaclesInRangeCount = RadarHits.Num();

		if (ObstaclesInRangeCount > 0)
		{
			for (int32 i = 0; i < ObstaclesInRangeCount; ++i)
			{
				ASteeringObstacle * Obstacle = Cast<ASteeringObstacle>(RadarHits[i].Actor.Get());
				if (Obstacle != nullptr)
				{
					uint32 index = ObstaclesWithSquaredDistance.AddUninitialized();
					ObstaclesWithSquaredDistance[index].Key = Obstacle;
					ObstaclesWithSquaredDistance[index].Value = (ObstaclesWithSquaredDistance[index].Key->GetActorLocation() - OwnerLocation).SizeSquared();
				}
			}
		}
		ObstaclesWithSquaredDistance.Sort(USteering3D::ConstPredicate);
	}

	int32 ActualObstaclesToCompute = FMath::Min(MaxComputedNeighbors, ObstaclesInRangeCount);
	for (int32 i = 0; i < ActualObstaclesToCompute; ++i)
	{
		const ASteeringObstacle * const Obstacle = ObstaclesWithSquaredDistance[i].Key;

		const FVector RelativePosition = Obstacle->GetActorLocation() - OwnerLocation;
		const FVector RelativeVelocity = OwnerVelocity - Obstacle->Velocity;
		const float DistanceSquared = ObstaclesWithSquaredDistance[i].Value;
		const float CombinedRadius = Owner->SignatureRadius + Obstacle->SignatureRadius;
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
			// Collision occured
			UE_LOG(Generic, Warning, TEXT("Collision occured: Impact depth => %f"), (FMath::Sqrt(DistanceSquared) - CombinedRadius));
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

	// Programms		
	const int32 PlaneFail = linearProgram3(Planes, MaxVelocity, Owner->PreferedVelocity, false, Owner->NewVelocity);

	if (PlaneFail < Planes.Num())
	{
		linearProgram4(Planes, PlaneFail, MaxVelocity, Owner->NewVelocity);
	}
}

bool USteering3D::linearProgram1(const TArray<FPlane> &Planes, int32 PlaneNo, const FLine &Line, float Radius, const FVector &OptVelocity, bool DirectionOpt, FVector &Result)
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

		if (FMath::Square(Denominator) <= RVO_EPSILON)
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

bool USteering3D::linearProgram2(const TArray<FPlane> &Planes, int32 PlaneNo, float Radius, const FVector &OptVelocity, bool DirectionOpt, FVector &Result)
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

		if (PlaneOptVelocityLengthSq <= RVO_EPSILON) 
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

			if (CrossProduct.SizeSquared() <= RVO_EPSILON)
			{
				// Planes PlaneNo and i are (almost) parallel, and plane i fully invalidates plane PlaneNo
				return false;
			}

			FLine Line;
			Line.Direction = CrossProduct.GetSafeNormal();
			const FVector LineNormal = FVector::CrossProduct(Line.Direction, Planes[PlaneNo].Normal);
			Line.Point = Planes[PlaneNo].Point + (FVector::DotProduct((Planes[i].Point - Planes[PlaneNo].Point), Planes[i].Normal) / FVector::DotProduct(LineNormal, Planes[i].Normal)) * LineNormal;

			if (!linearProgram1(Planes, i, Line, Radius, OptVelocity, DirectionOpt, Result)) {
				return false;
			}
		}
	}

	return true;
}

int32 USteering3D::linearProgram3(const TArray<FPlane> &Planes, float Radius, const FVector &OptVelocity, bool DirectionOpt, FVector &Result)
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

			if (!linearProgram2(Planes, i, Radius, OptVelocity, DirectionOpt, Result))
			{
				Result = TempResult;
				return i;
			}
		}
	}

	return Planes.Num();
}

void USteering3D::linearProgram4(const TArray<FPlane> &Planes, int32 BeginPlane, float Radius, FVector &Result)
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

				if (CrossProduct.SizeSquared() <= RVO_EPSILON)
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

			if (linearProgram3(ProjPlanes, Radius, Planes[i].Normal, true, Result) < ProjPlanes.Num())
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