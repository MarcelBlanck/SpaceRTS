

#include "SpaceRTS.h"
#include "EmptyActor.h"
#include "Interfaces/SelectableObject.h"
#include "ActionIndicationGizmo.h"

UActionIndicationGizmo::UActionIndicationGizmo() :
	Super(),
	CurrentGizmoStage(GizmoStage::Off),
	StoredGizmoStage(GizmoStage::Off),
	OriginActor(nullptr),
	KinkActor(nullptr),
	EndActor(nullptr),
	EndFollowActor(nullptr),
	ActionIndicationParticleSystem(nullptr)
{
	ConstructorHelpers::FObjectFinder<UParticleSystem> ActionIndicationAsset(TEXT("ParticleSystem'/Game/FX/ActionIndication/ActionIndication.ActionIndication'"));
	checkf(ActionIndicationAsset.Object, TEXT("Did not find ParticleSystem'/Game/FX/ActionIndication/ActionIndication.ActionIndication' - Maybe the asset was moved or renamed."));
	ActionIndicationParticleSystem = ActionIndicationAsset.Object;
}

bool UActionIndicationGizmo::IsActionIndicationEnabled()
{
	return CurrentGizmoStage != GizmoStage::Off;
}

void UActionIndicationGizmo::EnableActionIndication()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Name = TEXT("OriginActor");
	OriginActor = GetWorld()->SpawnActor(AEmptyActor::StaticClass(), &FVector::ZeroVector, &FRotator::ZeroRotator, SpawnParams);
	SpawnParams.Name = TEXT("KinkActor");
	KinkActor = GetWorld()->SpawnActor(AEmptyActor::StaticClass(), &FVector::ZeroVector, &FRotator::ZeroRotator, SpawnParams);
	SpawnParams.Name = TEXT("EndActor");
	EndActor = GetWorld()->SpawnActor(AEmptyActor::StaticClass(), &FVector::ZeroVector, &FRotator::ZeroRotator, SpawnParams);

	ActionIndicationMain = UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		ActionIndicationParticleSystem,
		FVector(0.f, 0.f, 0.f),FRotator::ZeroRotator, false);
	ActionIndicationMain->SetActorParameter(TEXT("Source"), OriginActor);
	ActionIndicationMain->SetActorParameter(TEXT("Target"), KinkActor);
	ActionIndicationMain->SetVectorParameter(TEXT("Color"), FVector(0.f, 1.f, 0.f));
	
	ActionIndicationAfterKink = UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		ActionIndicationParticleSystem,
		FVector(0.f, 0.f, 0.f), FRotator::ZeroRotator, false);
	ActionIndicationAfterKink->SetActorParameter(TEXT("Source"), KinkActor);
	ActionIndicationAfterKink->SetActorParameter(TEXT("Target"), EndActor);
	ActionIndicationAfterKink->SetVectorParameter(TEXT("Color"), FVector(1.f, 0.f, 0.f));


	CurrentGizmoStage = GizmoStage::FindMovementRadius;
}

void UActionIndicationGizmo::DisableActionIndication()
{
	OriginActor->Destroy();
	OriginActor = nullptr;

	KinkActor->Destroy();
	KinkActor = nullptr;

	EndActor->Destroy();
	EndActor = nullptr;

	EndFollowActor = nullptr;

	ActionIndicationMain->Deactivate();
	ActionIndicationMain->GetOwner()->Destroy();
	ActionIndicationMain = nullptr;

	ActionIndicationAfterKink->Deactivate();
	ActionIndicationAfterKink->GetOwner()->Destroy();
	ActionIndicationAfterKink = nullptr;

	CurrentGizmoStage = GizmoStage::Off;
	StoredGizmoStage = GizmoStage::Off;
}

void UActionIndicationGizmo::UpdateGizmoRepresentation(FVector HeadPosition, FRotator HeadOrientation, AActor* SelectedActor, AActor* LookAtActor)
{
	if (SelectedActor == nullptr || CurrentGizmoStage == GizmoStage::Off)
		return;

	OriginActor->SetActorLocation(SelectedActor->GetActorLocation());
	
	ISelectableObject* LookAtObject = Cast<ISelectableObject>(LookAtActor);
	/*
	if (LookAtObject == nullptr 
			&& (CurrentGizmoStage == GizmoStage::IndicatePossibleAttack 
				|| CurrentGizmoStage == GizmoStage::IndicatePossibleInteraction))
	{
		CurrentGizmoStage = StoredGizmoStage;
	}
	else if (LookAtObject->GetType() == ESelectableObjectType::EnemySpaceship
				&& (CurrentGizmoStage == GizmoStage::FindMovementRadius
					|| CurrentGizmoStage == GizmoStage::FindMovementHeight))
	{
		StoredGizmoStage = CurrentGizmoStage;
		CurrentGizmoStage = GizmoStage::IndicatePossibleAttack;
	}
	else if (LookAtObject->GetType() == ESelectableObjectType::PlayerControlledSpaceship
				&& (CurrentGizmoStage == GizmoStage::FindMovementRadius
					|| CurrentGizmoStage == GizmoStage::FindMovementHeight))
	{
		StoredGizmoStage = CurrentGizmoStage;
		CurrentGizmoStage = GizmoStage::IndicatePossibleInteraction;
	}
	*/
	switch (CurrentGizmoStage)
	{
	case GizmoStage::FindMovementRadius:
		{
			EnableOnlyActionIndicationMain();
			FPlane FindRadiusPlane(SelectedActor->GetActorLocation(), FVector(0.f, 0.f, 1.f));
			KinkActor->SetActorLocation(FMath::LinePlaneIntersection(HeadPosition, HeadPosition + HeadOrientation.Vector() * 1000000.f, FindRadiusPlane));
		} break;

	case GizmoStage::FindMovementHeight:
	{
		EnableBothActionIndications();
		
		FPlane FindRadiusPlane(SelectedActor->GetActorLocation(), FVector(0.f, 0.f, 1.f));
		KinkActor->SetActorLocation(FMath::LinePlaneIntersection(HeadPosition, HeadPosition + HeadOrientation.Vector() * 1000000.f, FindRadiusPlane));

		FPlane FindHeightPlane(KinkActor->GetActorLocation(), HeadPosition - KinkActor->GetActorLocation());
		FVector HeightPlaneIntersection = FMath::LinePlaneIntersection(HeadPosition, HeadPosition + HeadOrientation.Vector() * 1000000.f, FindHeightPlane);
		HeightPlaneIntersection.X = KinkActor->GetActorLocation().X;
		HeightPlaneIntersection.Y = KinkActor->GetActorLocation().Y;
		EndActor->SetActorLocation(HeightPlaneIntersection);
	} break;
	case GizmoStage::IndicateEngagedMovement:
		{
			EnableOnlyActionIndicationMain();
			EndFollowActor = EndActor;
			CurrentGizmoStage = GizmoStage::KeepLastIndication;
			// Notify Delegate
			// Display Green  Origin -> End
		} break;
	case GizmoStage::IndicatePossibleAttack:
		{
			EnableOnlyActionIndicationMain();
			if (LookAtActor != nullptr)
			{
				EndActor->SetActorLocation(LookAtActor->GetActorLocation());
			}
			// Display Red Origin -> End
		} break;
	case GizmoStage::IndicateEngagedAttack:
		{
			EnableOnlyActionIndicationMain();
			if (LookAtActor != nullptr)
			{
				EndActor->SetActorLocation(LookAtActor->GetActorLocation());
				EndFollowActor = LookAtActor;
				CurrentGizmoStage = GizmoStage::KeepLastIndication;
			}
			// Notify Delegate
			// Display Red blinking Origin -> End
		} break;
	case GizmoStage::IndicatePossibleInteraction:
		{
			EnableOnlyActionIndicationMain();
			if (LookAtActor != nullptr)
			{
				EndActor->SetActorLocation(LookAtActor->GetActorLocation());
			}
		} break;
	case GizmoStage::IndicateEngagedInteraction:
		{
			EnableOnlyActionIndicationMain();
			if (LookAtActor != nullptr)
			{
				EndActor->SetActorLocation(LookAtActor->GetActorLocation());
				EndFollowActor = LookAtActor;
			}
			CurrentGizmoStage = GizmoStage::KeepLastIndication;
			// Notify Delegate
			// Display Blue Origin -> End
		} break;
	case GizmoStage::KeepLastIndication:
		{
			EnableOnlyActionIndicationMain();
			if (EndFollowActor != nullptr)
			{
				EndActor->SetActorLocation(EndFollowActor->GetActorLocation());
			}
			// Display blinking last color Origin -> End
		} break;
	}
}

void UActionIndicationGizmo::EnterSuccessiveState()
{
	switch (CurrentGizmoStage)
	{
	case GizmoStage::FindMovementRadius:
		CurrentGizmoStage = GizmoStage::FindMovementHeight;
		break;
	case GizmoStage::FindMovementHeight:
		//CurrentGizmoStage = GizmoStage::IndicateEngagedMovement;
		break;
	case GizmoStage::IndicatePossibleAttack:
		CurrentGizmoStage = GizmoStage::IndicateEngagedAttack;
		break;
	case GizmoStage::IndicatePossibleInteraction:
		CurrentGizmoStage = GizmoStage::IndicateEngagedInteraction;
		break;
	default:
		// Other states are transition to KeepLastIndication...
		// KeepLastIndication will be ended by deactivation
		break;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("CurrentGizmoStage %d"), (int32)CurrentGizmoStage));
}

bool UActionIndicationGizmo::TryEnterPreviousState()
{
	bool PreviousStateEntered = false;

	switch (CurrentGizmoStage)
	{
	case GizmoStage::FindMovementHeight:
		CurrentGizmoStage = GizmoStage::FindMovementRadius;
		PreviousStateEntered = true;
		break;
	default:
		// Other states are not permanent or can not be reverted
		break;
	}

	return PreviousStateEntered;
}

void UActionIndicationGizmo::EnableOnlyActionIndicationMain()
{
	//ActionIndicationMain->Activate();
	//ActionIndicationAfterKink->Deactivate();
}

void UActionIndicationGizmo::EnableBothActionIndications()
{
	//ActionIndicationMain->Activate();
	//ActionIndicationAfterKink->Activate(true);
}

UActionIndicationGizmo::FOnEngageMovement& UActionIndicationGizmo::GetOnEngageMovementDelegate()
{
	return OnEngageMovement;
}

UActionIndicationGizmo::FOnEngageAttack& UActionIndicationGizmo::GetOnEngageAttackDelegate()
{
	return OnEngageAttack;
}

UActionIndicationGizmo::FOnEngageInteraction& UActionIndicationGizmo::GetOnEngageInteractionDelegate()
{
	return OnEngageInteraction;
}
