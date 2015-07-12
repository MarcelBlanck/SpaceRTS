
#include "SpaceRTS.h"
#include "PlayerControlledSpaceship.h"


UPlayerControlledSpaceship::UPlayerControlledSpaceship(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

UPlayerControlledSpaceshipBPFunctionLibrary::UPlayerControlledSpaceshipBPFunctionLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UPlayerControlledSpaceshipBPFunctionLibrary::AttackObject(AActor *Actor, const TScriptInterface<ISelectableObject> &Object)
{
	IPlayerControlledSpaceship* Interface = Cast<IPlayerControlledSpaceship>(Actor);
	if (Interface != nullptr)
	{
		TScriptInterface<IPlayerControlledSpaceship> ScriptInterface = TScriptInterface<IPlayerControlledSpaceship>();
		ScriptInterface.SetObject(Actor);
		ScriptInterface.SetInterface(Interface);
		ScriptInterface->Execute_AttackObject(Actor, Object);
		ScriptInterface->ExecuteNative_AttackObject(Object);
	}
}

void UPlayerControlledSpaceshipBPFunctionLibrary::InteractWithObject(AActor *Actor, const TScriptInterface<ISelectableObject> &Object)
{
	IPlayerControlledSpaceship* Interface = Cast<IPlayerControlledSpaceship>(Actor);
	if (Interface != nullptr)
	{
		TScriptInterface<IPlayerControlledSpaceship> ScriptInterface = TScriptInterface<IPlayerControlledSpaceship>();
		ScriptInterface.SetObject(Actor);
		ScriptInterface.SetInterface(Interface);
		ScriptInterface->Execute_InteractWithObject(Actor, Object);
		ScriptInterface->ExecuteNative_InteractWithObject(Object);
	}
}

void UPlayerControlledSpaceshipBPFunctionLibrary::SteerToLocation(AActor *Actor, FVector& Location)
{
	IPlayerControlledSpaceship* Interface = Cast<IPlayerControlledSpaceship>(Actor);
	if (Interface != nullptr)
	{
		TScriptInterface<IPlayerControlledSpaceship> ScriptInterface = TScriptInterface<IPlayerControlledSpaceship>();
		ScriptInterface.SetObject(Actor);
		ScriptInterface.SetInterface(Interface);
		ScriptInterface->Execute_SteerToLocation(Actor, Location);
		ScriptInterface->ExecuteNative_SteerToLocation(Location);
	}
}

void IPlayerControlledSpaceship::ExecuteNative_AttackObject(const TScriptInterface<ISelectableObject>& Object)
{
	// Nothing to do here
}

void IPlayerControlledSpaceship::ExecuteNative_InteractWithObject(const TScriptInterface<ISelectableObject>& Object)
{
	// Nothing to do here
}

void IPlayerControlledSpaceship::ExecuteNative_SteerToLocation(FVector& Location)
{
	// Nothing to do here
}