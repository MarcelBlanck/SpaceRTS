// Pyrite Softworks 2015

#pragma once

#include "Components/ActorComponent.h"
#include "SteeringObstacle.h"
#include "Steering3D.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACERTS_API USteering3D : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Steering")
	float MaxVelocity;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Radar")
	float ScanRadius;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Radar")
	TEnumAsByte<ETraceTypeQuery> TraceType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Performance")
	int32 MaxComputedNeighbors;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Performance")
	float TimeHorizon;

	USteering3D(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	void ComputeNewVelocity(UWorld* World, class ASteeringObstacle* Owner, float DeltaTime);

	inline static bool SortByDistanceAndPriority(const TPair<ASteeringObstacle*, float>& ip1, const TPair<ASteeringObstacle*, float>& ip2)
	{
		return (ip1.Value < ip2.Value) || ip1.Key->IsPrioritySignature;
	}

	inline static bool NotPrioritySignaturePredicate(const TPair<ASteeringObstacle*, float>& ip1)
	{
		return !ip1.Key->IsPrioritySignature;
	}

private:
	struct FLine
	{
		FVector Direction;
		FVector Point;
	};

	struct FPlane
	{
		FVector Point;
		FVector Normal;
	};

	bool GetRadarBlipResult(FVector const & OwnerLocation, AActor* Owner, UWorld* World, TArray<FHitResult>& OutHits);

	bool  linearProgram1(const TArray<FPlane> &Planes, int32 PlaneNo, const FLine &Line, float Radius, const FVector &OptVelocity, bool DirectionOpt, FVector &Result);
	bool  linearProgram2(const TArray<FPlane> &Planes, int32 PlaneNo, float Radius, const FVector &OptVelocity, bool DirectionOpt, FVector &Result);
	int32 linearProgram3(const TArray<FPlane> &Planes, float Radius, const FVector &OptVelocity, bool DirectionOpt, FVector &Result);
	void  linearProgram4(const TArray<FPlane> &Planes, int32 BeginPlane, float Radius, FVector &Result);
};
