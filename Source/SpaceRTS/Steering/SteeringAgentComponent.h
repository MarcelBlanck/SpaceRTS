// Marcel Blanck 2015

#pragma once

#include "Components/SphereComponent.h"
#include "SteeringAgentComponent.generated.h"

UCLASS(ClassGroup = (Steering), meta = (BlueprintSpawnableComponent))
class SPACERTS_API USteeringAgentComponent : public USphereComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTargetPositionReachedDelegate);

public:
	USteeringAgentComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void EnableSteering();

	virtual void DisableSteering();

	virtual void SetTargetPosition(FVector& TargetPosition);

	virtual void SetFocusActor(AActor* NewFocusActor);

	virtual void ClearFocusActor();

	virtual void SetMaxVelocity(float NewMaxVelocity);
	
	virtual void CalculatePreferedVelocity();

	virtual void ComputeNewVelocity(UWorld* World, float DeltaTime);

	UPROPERTY(BlueprintAssignable)
	FTargetPositionReachedDelegate OnTargetPositionReached;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Steering")
	bool IsSteeringEnabled;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Steering")
	bool IsPrioritySignature;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Steering")
	float MaxVelocity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Steering")
	FVector TargetPosition;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Steering")
	AActor* FocusActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Radar")
	float ScanRadius;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Radar")
	TEnumAsByte<ETraceTypeQuery> TraceType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Performance")
	int32 MaxComputedNeighbors;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tweaking")
	float TimeHorizon;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Steering")
	FVector Velocity;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Steering")
	FVector NewVelocity;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Steering")
	FVector PreferedVelocity;

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

	struct FObstacleProcessingData
	{
		AActor* ObstacleActor;
		USteeringAgentComponent* Steering;
		FVector RelativePosition; // Distance from Signature to Signature
		float DistanceSquared; // Squared distance from Signature to Signature
	};

	AActor* Owner; // Faster access to owner, only needs to be checked on begin play

	bool IsTargetPositionReachedReported;

	bool GetRadarBlipResult(FVector const & OwnerLocation, UWorld* World, TArray<FHitResult>& OutHits);

	/* Algorithms ported from ROV2 lib, optimized for seamless usage in UE4 */
	bool  ROV2_LinearProgram1(const TArray<FPlane> &Planes, int32 PlaneNo, const FLine &Line, float Radius, const FVector &OptVelocity, bool DirectionOpt, FVector &Result);
	bool  ROV2_LinearProgram2(const TArray<FPlane> &Planes, int32 PlaneNo, float Radius, const FVector &OptVelocity, bool DirectionOpt, FVector &Result);
	int32 ROV2_LinearProgram3(const TArray<FPlane> &Planes, float Radius, const FVector &OptVelocity, bool DirectionOpt, FVector &Result);
	void  ROV2_LinearProgram4(const TArray<FPlane> &Planes, int32 BeginPlane, float Radius, FVector &Result);

	inline static bool SortByDistanceAndPriority(const FObstacleProcessingData& ip1, const FObstacleProcessingData& ip2)
	{
		return (ip1.DistanceSquared < ip2.DistanceSquared) || ip1.Steering->IsPrioritySignature;
	}
};
