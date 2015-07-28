

#include "SpaceRTS.h"
#include "PlayerPawn.h"
#include "Interfaces/SelectableObject.h"
#include "Interfaces/PlayerControlledSpaceship.h"
#include "PaperFlipbook.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

APlayerPawn::APlayerPawn(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	Camera(nullptr),
	MaxInteractionDistance(20000.f),
	RecticleDefaultDistance(3000.f),
	RecticleAnimationRate(5.f),
	RecticleAlpha(0.6f),
	RecticleColorNeutral(1.f, 1.0, 1.f, RecticleAlpha),
	RecticleColorFriendly(0.f, 1.0, 0.f, RecticleAlpha),
	RecticleColorAttack(1.f, 0.0, 0.f, RecticleAlpha),
	RecticleColorInteract(0.f, 0.0, 1.f, RecticleAlpha)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	Camera = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("PlayerCamera"));
	Camera->AttachTo(RootComponent);
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 90.f;

	RecticleRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RecticleRoot"));
	RecticleRoot->SetRelativeLocation(FVector(RecticleDefaultDistance, 0.f, 0.f));
	RecticleRoot->AttachTo(RootComponent);

	ConstructorHelpers::FObjectFinder<UPaperFlipbook> RecticleFlipbook(TEXT("PaperFlipbook'/Game/GUI/Recticle/RecticleFlipbook.RecticleFlipbook'"));
	checkf(RecticleFlipbook.Object, TEXT("Did not find PaperFlipbook'/Game/GUI/Recticle/RecticleFlipbook.RecticleFlipbook' - Maybe the asset was moved or renamed."));
	Recticle = ObjectInitializer.CreateDefaultSubobject<UPaperFlipbookComponent>(this, TEXT("Recticle"));
	Recticle->AttachTo(RecticleRoot);
    Recticle->SetFlipbook(RecticleFlipbook.Object);
	Recticle->SetLooping(false);
	Recticle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Recticle->SetCollisionProfileName(FName("NoCollision"));
	Recticle->bGenerateOverlapEvents = false;
	Recticle->SetEnableGravity(false);
	Recticle->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	Recticle->SetPlaybackPosition(0.f, false);
	Recticle->SetSpriteColor(RecticleColorNeutral);
	Recticle->Stop();

	ActionIndicator = ObjectInitializer.CreateDefaultSubobject<UActionIndicationGizmo>(this, TEXT("ActionIndicator"));

	TouchpadGearVR = ObjectInitializer.CreateDefaultSubobject<UTouchpadGearVR>(this, TEXT("TouchpadGearVR"));

	BackKeyGearVR = ObjectInitializer.CreateDefaultSubobject<UBackKeyGearVR>(this, TEXT("BackKeyGearVR"));
	BackKeyGearVR->RegisterComponent();

	SteeringAgentComponent->DisableSteering();
}

FVector APlayerPawn::GetPawnViewLocation() const
{
	return (GetActorLocation() * 2) - Camera->GetComponentLocation();
}

void APlayerPawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		UHeadMountedDisplayFunctionLibrary::EnableHMD(true);
		UHeadMountedDisplayFunctionLibrary::EnableLowPersistenceMode(true);
		UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
	}

	OnEngageMovmentDelegate.BindUFunction(this, TEXT("OnEngageMovement"));
	ActionIndicator->GetOnEngageMovementDelegate().Add(OnEngageMovmentDelegate);
	//ActionIndicator->GetOnEngageAttackDelegate().AddDynamic(this, &APlayerPawn::OnEngageAttack); // Needs Script Delegate also
	//ActionIndicator->GetOnEngageInteractionDelegate().AddDynamic(this, &APlayerPawn::OnEngageInteraction); // Needs Script Delegate also

	OnGearVRTouchpadTapDelegate.BindUFunction(this, TEXT("OnLookInteraction"));
	TouchpadGearVR->OnSingleTap.Add(OnGearVRTouchpadTapDelegate);

	OnGearVRBackKeyDelegate.BindUFunction(this, TEXT("OnBackKey"));
	BackKeyGearVR->OnBackClicked.Add(OnGearVRBackKeyDelegate);

	bBlockInput = false;
	AutoReceiveInput = EAutoReceiveInput::Player0;
	InputPriority = 1;
}


void APlayerPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	EnableInput(Cast<APlayerController>(NewController));
}

void APlayerPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	//TickFpsCounter();
	
	UpdateLookAtActorAndRecticle();

	if (ActionIndicator->IsActionIndicationEnabled())
	{
		ActionIndicator->UpdateGizmoRepresentation(
			Camera->GetComponentLocation(),
			Camera->GetComponentRotation(), 
			SelectedActor.Get(),
			CurrentLookAtActor.Get());
	}
}


void APlayerPawn::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	check(InputComponent);

	InputComponent->BindAction("LookInteraction", EInputEvent::IE_Pressed, this, &APlayerPawn::OnLookInteraction);
	InputComponent->BindAxis("LookRight", this, &APlayerPawn::OnLookRight);
	InputComponent->BindAxis("LookUp", this, &APlayerPawn::OnLookUp);
}

void APlayerPawn::OnBackKey()
{

}

void APlayerPawn::OnLookInteraction()
{
	ISelectableObject* LookAtSelectable = Cast<ISelectableObject>(CurrentLookAtActor.Get());
	if (LookAtSelectable != nullptr)
	{
		switch (LookAtSelectable->GetType())
		{
		case ESelectableObjectType::Prop:
		case ESelectableObjectType::Resource:
		case ESelectableObjectType::EnemySpaceship:
			LookAtSelectable->Select();
			if (SelectedActor.IsValid())
			{
				ISelectableObject* SelectedSelectable = Cast<ISelectableObject>(SelectedActor.Get());
				SelectedSelectable->Deselect();
			}
			SelectedActor = CurrentLookAtActor.Get();
			break;
		case ESelectableObjectType::UI:
			LookAtSelectable->Select();
			// Do not save selection, just trigger the ui element
			break;
		case ESelectableObjectType::PlayerControlledSpaceship:
			ActionIndicator->DisableActionIndication();
			if (SelectedActor.IsValid())
			{
				ISelectableObject* SelectedSelectable = Cast<ISelectableObject>(SelectedActor.Get());
				SelectedSelectable->Deselect();
			}
			SelectedActor = CurrentLookAtActor.Get();
			ActionIndicator->EnableActionIndication();
			break;
		}	
	}
	else if (ActionIndicator->IsActionIndicationEnabled())
	{
		ActionIndicator->EnterSuccessiveState();
	}
}

void APlayerPawn::OnLookRight(float Value)
{
	FRotator Rotator(0.f, Value, 0.f);
	Camera->AddRelativeRotation(Rotator);
}

void APlayerPawn::OnLookUp(float Value)
{
	FRotator Rotator(Value, 0.f, 0.f);
	Camera->AddRelativeRotation(Rotator);
}

void APlayerPawn::UpdateLookAtActorAndRecticle()
{
	FCollisionQueryParams TraceParams(FName(TEXT("CurrentLookAtActorTrace")), false, this);

	FVector DeviceRotationVector;
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		FRotator DeviceRotation;
		FVector DevicePosition;
		UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(DeviceRotation, DevicePosition);
		DeviceRotationVector = DeviceRotation.Vector();
	}
	else
	{
		DeviceRotationVector = Camera->GetComponentRotation().Vector();
	}

	const FVector Start = GetPawnViewLocation();
	const FVector End = Start + DeviceRotationVector * 100000;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	FHitResult HitData(ForceInit);
	UKismetSystemLibrary::LineTraceSingle_NEW(GetWorld(), Start, End, ETraceTypeQuery::TraceTypeQuery2, 
		false, ActorsToIgnore, EDrawDebugTrace::None, HitData, true);

	AActor* LastLookAtActor = CurrentLookAtActor.Get();
	ISelectableObject* LastLookAtSelectable = Cast<ISelectableObject>(LastLookAtActor);

	CurrentLookAtActor = HitData.Actor;
	ISelectableObject* CurrentLookAtSelectable = Cast<ISelectableObject>(CurrentLookAtActor.Get());

	bool bLookAtActorHasChanged = (LastLookAtActor != CurrentLookAtActor);

	// Fire Gaze Events
	if (bLookAtActorHasChanged)
	{
		if (LastLookAtSelectable != nullptr)
		{
			LastLookAtSelectable->GazeEnd();
		}
		if (CurrentLookAtSelectable != nullptr)
		{
			CurrentLookAtSelectable->GazeBegin();
		}
	}

	// Update Recticle Position, Color and Animation
	if (CurrentLookAtActor.IsValid())
	{
		FVector HitVector = (HitData.ImpactPoint - Start);
		float HitDistance = HitVector.Size();
		
		RecticleRoot->SetWorldLocation(GetPawnViewLocation() + DeviceRotationVector * HitDistance);
		RecticleRoot->SetRelativeScale3D(FVector(HitDistance / RecticleDefaultDistance));

		if (bLookAtActorHasChanged)
		{
			if (CurrentLookAtSelectable != nullptr)
			{
				switch (CurrentLookAtSelectable->GetType())
				{
					case ESelectableObjectType::Prop:
						Recticle->SetSpriteColor(RecticleColorNeutral);
						break;
					case ESelectableObjectType::UI:
						Recticle->SetSpriteColor(RecticleColorInteract);
						break;
					case ESelectableObjectType::Resource:
						Recticle->SetSpriteColor(RecticleColorInteract);
						break;
					case ESelectableObjectType::EnemySpaceship:
						Recticle->SetSpriteColor(RecticleColorAttack);
						break;
					case ESelectableObjectType::PlayerControlledSpaceship:
						Recticle->SetSpriteColor(RecticleColorFriendly);
						break;
				}
				Recticle->SetPlayRate(RecticleAnimationRate);
				Recticle->PlayFromStart();
			}
		}	
	}
	else
	{
		if (bLookAtActorHasChanged)
		{
			Recticle->SetSpriteColor(RecticleColorNeutral);
			RecticleRoot->SetRelativeScale3D(FVector(1.f));
			Recticle->SetPlaybackPosition(0.f, false);
			Recticle->Stop();
		}
		RecticleRoot->SetWorldLocation(GetPawnViewLocation() + DeviceRotationVector * RecticleDefaultDistance);
	}
	
	RecticleRoot->SetWorldRotation(FRotationMatrix::MakeFromX(GetPawnViewLocation() - RecticleRoot->GetComponentLocation()).Rotator());
}

void APlayerPawn::OnEngageMovement(FVector TargetPosition)
{
	if (SelectedActor.IsValid())
	{
		UPlayerControlledSpaceshipBPFunctionLibrary::SteerToLocation(SelectedActor.Get(), TargetPosition);
		SelectedActor.Reset();
	}
}

void APlayerPawn::OnEngageAttack(AActor* TargetActor)
{
	if (SelectedActor.IsValid())
	{
		IPlayerControlledSpaceship* Interface = Cast<IPlayerControlledSpaceship>(SelectedActor.Get());
	}
}

void APlayerPawn::OnEngageInteraction(AActor* TargetActor)
{
	if (SelectedActor.IsValid())
	{
		IPlayerControlledSpaceship* Interface = Cast<IPlayerControlledSpaceship>(SelectedActor.Get());
	}
}