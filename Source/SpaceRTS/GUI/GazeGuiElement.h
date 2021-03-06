

#pragma once

#include "GameFramework/Actor.h"
#include "PaperSpriteComponent.h"
#include "Components/TextRenderComponent.h"
#include "../Gameplay/Interfaces/SelectableObject.h"
#include "GazeGuiElement.generated.h"

UENUM(BlueprintType)
enum class EGazeGuiElementType : uint8
{
	Indicator              UMETA(DisplayName = "Indicator"),
	Trigger                UMETA(DisplayName = "Trigger"),
	Switch                 UMETA(DisplayName = "Switch"),
	ToggleSwitch           UMETA(DisplayName = "ToggleSwitch")
};

UCLASS()
class SPACERTS_API AGazeGuiElement : public AActor, public ISelectableObject
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelect);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeselect);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGazeBegin);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGazeEnd);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTriggered, AGazeGuiElement*, GazeGuiElement);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSwitched, AGazeGuiElement*, GazeGuiElement, bool, Active);

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gaze Gui")
	EGazeGuiElementType GazeGuiElementType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gaze Gui")
	float GazeAnimationDurationS;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gaze Gui")
	float HighlightAnimationDurationS;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gaze Gui")
	TArray<UPaperSpriteComponent*> PaperSpriteComponents;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gaze Gui")
	TArray<UTextRenderComponent*> TextRenderComponents;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Sounds")
	USoundCue* GazeBeginSoundCue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Sounds")
	USoundCue* GazeEndSoundCue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Sounds")
	USoundCue* TriggeredSoundCue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Sounds")
	USoundCue* SwitchedSoundCue;

	AGazeGuiElement(const FObjectInitializer& ObjectInitializer);

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	bool IsSwitchActive();

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

	UPROPERTY(BlueprintAssignable)
	FOnTriggered OnTriggered;

	UPROPERTY(BlueprintAssignable)
	FOnSwitched OnSwitched;

private:
	FVector OriginalScale;
	float CurrentScale;
	float TargetScale;
	float ScaleChangeFinishedTimeS;
	float ScaleChangeDurationS;

	float CurrentHighlight;
	float TargetHighlight;
	float HighlightChangeFinishedTimeS;
	float HighlightChangeDurationS;

	bool SwitchActive;

	void StartScaleAnimation(float NewTargetScale);
	void AnimateScale();

	void StartTriggerAnimation();
	void StartSwitchAnimation(bool NewActive);
	void AnimateColor();
};
