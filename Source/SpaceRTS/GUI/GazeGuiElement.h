

#pragma once

#include "GameFramework/Actor.h"
#include "../Gameplay/Interfaces/SelectableObject.h"
#include "GazeGuiElement.generated.h"

UCLASS()
class SPACERTS_API AGazeGuiElement : public AActor, public ISelectableObject
{
	GENERATED_BODY()
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelect);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeselect);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGazeBegin);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGazeEnd);

public:	
	AGazeGuiElement(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

	virtual ESelectableObjectType GetType() override;

	virtual void Select() override;

	virtual void Deselect() override;

	virtual void GazeBegin() override;

	virtual void GazeEnd() override;

	UPROPERTY(BlueprintAssignable)
	FOnSelect OnSelect;

	UPROPERTY(BlueprintAssignable)
	FDeselect OnDeselect;

	UPROPERTY(BlueprintAssignable)
	FGazeBegin OnGazeBegin;

	UPROPERTY(BlueprintAssignable)
	FGazeEnd OnGazeEnd;
};
