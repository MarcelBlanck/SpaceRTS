// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SteeringAgentComponent.h"
#include "SteeringAgentInterface.generated.h"

UINTERFACE()
class USteeringAgentInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class ISteeringAgentInterface
{
	GENERATED_IINTERFACE_BODY()

	virtual USteeringAgentComponent* GetSteeringAgentComponent();

	virtual void CalculatePreferedVelocity();
};
