// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SteeringAgentInterface.generated.h"

UINTERFACE()
class USteeringAgentInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class ISteeringAgentInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	virtual void SetTargetPosition(const FVector& TargetPosition);

	virtual void CalculatePreferedVelocity();

	virtual void ComputeNewVelocity(UWorld* World, float DeltaTime);

	virtual const FVector& GetSteeringVelocity();

	virtual bool IsPrioritySignature();

	virtual float GetSphereRadius();
};
