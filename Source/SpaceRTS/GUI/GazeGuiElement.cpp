
#include "SpaceRTS.h"
#include "../Gameplay/GameInstanceSpaceRTS.h"
#include "GazeGuiElement.h"

AGazeGuiElement::AGazeGuiElement(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	SwitchActive(false),
	GazeAnimationDurationS(1.f),
	HighlightAnimationDurationS(1.0f)
{
	PrimaryActorTick.bCanEverTick = 1;
}

void AGazeGuiElement::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AGazeGuiElement::BeginPlay()
{
	Super::BeginPlay();

	OriginalScale = GetActorScale();
	CurrentScale = 1.f;
	TargetScale = 1.f;

	FVector PlayerPawnLocation = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();
	SetActorRotation(FRotationMatrix::MakeFromX(PlayerPawnLocation - GetActorLocation()).Rotator());

	FLinearColor GuiColor = Cast<UGameInstanceSpaceRTS>(GetGameInstance())->GuiColor;

	TArray<UActorComponent*> TmpPaperSpriteComponents = GetComponentsByClass(UPaperSpriteComponent::StaticClass());
	for (UActorComponent* Component : TmpPaperSpriteComponents)
	{
		UPaperSpriteComponent* PaperSpriteComponent = Cast<UPaperSpriteComponent>(Component);
		if (PaperSpriteComponent != nullptr)
		{
			PaperSpriteComponent->SetSpriteColor(GuiColor);
			PaperSpriteComponents.AddUnique(PaperSpriteComponent);
		}
	}

	TArray<UActorComponent*> TmpTextRenderComponents = GetComponentsByClass(UTextRenderComponent::StaticClass());
	for (UActorComponent* Component : TmpTextRenderComponents)
	{
		UTextRenderComponent* TextRenderComponent = Cast<UTextRenderComponent>(Component);
		if (TextRenderComponent != nullptr)
		{
			TextRenderComponent->SetTextRenderColor(GuiColor);
			TextRenderComponents.AddUnique(TextRenderComponent);
		}
	}
}

void AGazeGuiElement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GazeGuiElementType != EGazeGuiElementType::Indicator)
	{
		AnimateScale();
		AnimateColor();
	}
	else
	{
		FLinearColor GuiColor = Cast<UGameInstanceSpaceRTS>(GetGameInstance())->GuiColor;

		for (UPaperSpriteComponent* PaperSpriteComponent : PaperSpriteComponents)
		{
			PaperSpriteComponent->SetSpriteColor(GuiColor);
		}
		for (UTextRenderComponent* TextRenderComponent : TextRenderComponents)
		{
			TextRenderComponent->SetTextRenderColor(GuiColor);
		}
	}
}

bool AGazeGuiElement::IsSwitchActive()
{
	return SwitchActive;
}


ESelectableObjectType AGazeGuiElement::GetType()
{
	return ESelectableObjectType::UI;
}

void AGazeGuiElement::Select()
{
	if (OnSelect.IsBound())
	{
		OnSelect.Broadcast();
	}

	if (GazeGuiElementType == EGazeGuiElementType::Trigger)
	{
		SwitchActive = false;
		StartTriggerAnimation();
	}	
	else if (GazeGuiElementType == EGazeGuiElementType::Switch && !SwitchActive)
	{
		SwitchActive = true;
		StartSwitchAnimation(SwitchActive);
	}
	else if (GazeGuiElementType == EGazeGuiElementType::ToggleSwitch)
	{
		SwitchActive = !SwitchActive;
		StartSwitchAnimation(SwitchActive);
	}
}

void AGazeGuiElement::Deselect()
{
	if (OnDeselect.IsBound())
	{
		OnDeselect.Broadcast();
	}

	if (GazeGuiElementType == EGazeGuiElementType::Switch && SwitchActive)
	{
		SwitchActive = false;
		StartSwitchAnimation(SwitchActive);
	}
}

void AGazeGuiElement::GazeBegin()
{
	if (OnGazeBegin.IsBound())
	{
		OnGazeBegin.Broadcast();
	}

	StartScaleAnimation(1.2f);
}

void AGazeGuiElement::GazeEnd()
{
	if (OnGazeEnd.IsBound())
	{
		OnGazeEnd.Broadcast();
	}

	StartScaleAnimation(1.f);
}

void AGazeGuiElement::StartScaleAnimation(float NewTargetScale)
{
	if (GetWorld() == nullptr)
		return;

	TargetScale = NewTargetScale;
	ScaleChangeFinishedTimeS = GetWorld()->GetTimeSeconds() + GazeAnimationDurationS;
	ScaleChangeDurationS = GazeAnimationDurationS;
}

void AGazeGuiElement::AnimateScale()
{
	if (!FMath::IsNearlyEqual(CurrentScale, TargetScale))
	{
		float ScaleTweenProgress = 1 - (ScaleChangeFinishedTimeS - GetWorld()->GetTimeSeconds()) / ScaleChangeDurationS;
		CurrentScale = FMath::Lerp(CurrentScale, TargetScale, ScaleTweenProgress);
		SetActorScale3D(OriginalScale * CurrentScale);
	}
}

void AGazeGuiElement::StartTriggerAnimation()
{
	CurrentHighlight = 0.f;
	TargetHighlight = 1.f;
	HighlightChangeFinishedTimeS = GetWorld()->GetTimeSeconds() + HighlightAnimationDurationS;
	HighlightChangeDurationS = HighlightAnimationDurationS;
	OnTriggered.Broadcast(this);
}

void AGazeGuiElement::StartSwitchAnimation(bool NewActive)
{
	CurrentHighlight = 0.f;
	TargetHighlight = 1.f;
	HighlightChangeFinishedTimeS = GetWorld()->GetTimeSeconds() + HighlightAnimationDurationS;
	HighlightChangeDurationS = HighlightAnimationDurationS;
	OnSwitched.Broadcast(this, NewActive);
}

void AGazeGuiElement::AnimateColor()
{
	if (GetWorld() == nullptr)
		return;

	if (!FMath::IsNearlyEqual(CurrentHighlight, TargetHighlight))
	{
		float ColorBlendProgress = 0.f;
		float HighlightProgress = FMath::Clamp<float>(1 - (HighlightChangeFinishedTimeS - GetWorld()->GetTimeSeconds()) / HighlightChangeDurationS, 0.f, 1.f);
		switch (GazeGuiElementType)
		{
		case EGazeGuiElementType::Trigger:
			if (HighlightProgress < 0.5f)
			{
				CurrentHighlight = FMath::Clamp<float>(FMath::InterpEaseInOut<float>(0.f, 0.5f, HighlightProgress * 2.f, 3.0f), 0.f, 1.f);
				ColorBlendProgress = CurrentHighlight * 2;
			}
			else
			{
				CurrentHighlight = FMath::Clamp<float>(FMath::InterpEaseInOut<float>(0.5f, 1.f, (HighlightProgress - 0.5f) * 2.f, 2.0f), 0.f, 1.f);
				ColorBlendProgress = 1.f - ((CurrentHighlight - 0.5f) * 2);
			}
			break;
		case EGazeGuiElementType::Switch:
			if (SwitchActive)
			{
				CurrentHighlight = FMath::Clamp<float>(FMath::InterpEaseIn<float>(0.f, 1.f, HighlightProgress, 2.0f), 0.f, 1.f);
				ColorBlendProgress = CurrentHighlight;
			}
			else
			{
				CurrentHighlight = FMath::Clamp<float>(FMath::InterpEaseOut<float>(0.f, 1.f, HighlightProgress, 2.0f), 0.f, 1.f);
				ColorBlendProgress = 1.f - CurrentHighlight;
			}
			break;
		}

		FLinearColor GuiColor = Cast<UGameInstanceSpaceRTS>(GetGameInstance())->GuiColor;
		FLinearColor GuiHighlightColor = Cast<UGameInstanceSpaceRTS>(GetGameInstance())->GuiHighlightColor;

		FLinearColor CurrentColor = FLinearColor::LerpUsingHSV(GuiColor, GuiHighlightColor, ColorBlendProgress);

		for (UPaperSpriteComponent* PaperSpriteComponent : PaperSpriteComponents)
		{
			PaperSpriteComponent->SetSpriteColor(CurrentColor);
		}
		for (UTextRenderComponent* TextRenderComponent : TextRenderComponents)
		{
			TextRenderComponent->SetTextRenderColor(CurrentColor);
		}
	}
}