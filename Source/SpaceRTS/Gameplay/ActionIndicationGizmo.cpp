

#include "SpaceRTS.h"
#include "ActionIndicationGizmo.h"

UActionIndicationGizmo::UActionIndicationGizmo() :
	Super(),
	CurrentGizmoStage(GizmoStage::Off),
	OriginActor(nullptr),
	KinkActor(nullptr),
	EndActor(nullptr),
	EndFollowActor(nullptr)
{

}

void UActionIndicationGizmo::EnableActionIndication()
{
	OriginActor = GetWorld()->SpawnActor(AActor::StaticClass());
	KinkActor = GetWorld()->SpawnActor(AActor::StaticClass());
	EndActor = GetWorld()->SpawnActor(AActor::StaticClass());

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

	CurrentGizmoStage = GizmoStage::Off;
}

void UActionIndicationGizmo::UpdateGizmoRepresentation(FVector HeadPosition, FRotator HeadOrientation, AActor* SelectedActor, AActor* LookAtActor)
{
	if (SelectedActor == nullptr || CurrentGizmoStage == GizmoStage::Off)
		return;

	FVector GizmoOrigin = SelectedActor->GetActorLocation();
	FVector GizmoKink;
	FVector GizmoEnd;

	if (LookAtActor != nullptr)
	{
		GizmoEnd = SelectedActor->GetActorLocation();
	}

	switch (CurrentGizmoStage)
	{
	case GizmoStage::FindMovementRadius:
		{
			FPlane FindRadiusPlane(GizmoOrigin, FVector(0.f, 0.f, 1.f));
			KinkActor->SetActorLocation(FMath::LinePlaneIntersection(HeadPosition, HeadPosition + HeadOrientation.Vector() * 1000000.f, FindRadiusPlane));
		} break;
	case GizmoStage::FindMovementHeight:
		{
			FPlane FindHeightPlane(GizmoKink, HeadPosition - GizmoKink);
			EndActor->SetActorLocation(FMath::LinePlaneIntersection(HeadPosition, HeadPosition + HeadOrientation.Vector() * 1000000.f, FindHeightPlane));
		} break;
	case GizmoStage::IndicateEngagedMovement:
		{
			EndFollowActor = EndActor;
			CurrentGizmoStage = GizmoStage::KeepLastIndication;
			// Notify Delegate
			// Display Green  Origin -> End
		} break;
	case GizmoStage::IndicatePossibleAttack:
		{
			EndActor->SetActorLocation(LookAtActor->GetActorLocation());
			// Display Red Origin -> End
		} break;
	case GizmoStage::IndicateEngagedAttack:
		{
			EndActor->SetActorLocation(LookAtActor->GetActorLocation());
			EndFollowActor = LookAtActor;
			CurrentGizmoStage = GizmoStage::KeepLastIndication;
			// Notify Delegate
			// Display Red blinking Origin -> End
		} break;
	case GizmoStage::IndicatePossibleInteraction:
		{
			EndActor->SetActorLocation(LookAtActor->GetActorLocation());
			// Display Blue Origin -> End
		} break;
	case GizmoStage::IndicateEngagedInteraction:
		{
			EndActor->SetActorLocation(LookAtActor->GetActorLocation());
			EndFollowActor = LookAtActor;
			CurrentGizmoStage = GizmoStage::KeepLastIndication;
			// Notify Delegate
			// Display Blue Origin -> End
		} break;
	case GizmoStage::KeepLastIndication:
		{
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

}


