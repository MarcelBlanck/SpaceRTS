// Pyrite Softworks 2015

#include "SpaceRTS.h"
#include "MoveableSteeringObstacle.h"
#include "Steering3D.h"

AMoveableSteeringObstacle::AMoveableSteeringObstacle(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	Steering = ObjectInitializer.CreateDefaultSubobject<USteering3D>(this, TEXT("Steering"));
}