

#include "SpaceRTS.h"
#include "EmptyActor.h"

AEmptyActor::AEmptyActor(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	
	Scene = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Scene"));
	Scene->SetMobility(EComponentMobility::Movable);
	SetRootComponent(Scene);
}

