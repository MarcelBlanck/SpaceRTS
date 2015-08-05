// Marcel Blanck 2015

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
	HMDRotationFollower(nullptr),
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

	HMDRotationFollower = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("HMDRotationFollower"));
	HMDRotationFollower->AttachTo(RootComponent);

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
}

FVector APlayerPawn::GetPawnViewLocation() const
{
	return Camera->GetComponentLocation();
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
	}

	OnEngageMovmentDelegate.BindUFunction(this, TEXT("OnEngageMovement"));
	ActionIndicator->GetOnEngageMovementDelegate().Add(OnEngageMovmentDelegate);

	OnEngageAttackDelegate.BindUFunction(this, TEXT("OnEngageAttack"));
	ActionIndicator->GetOnEngageAttackDelegate().Add(OnEngageAttackDelegate);

	OnEngageInteractionDelegate.BindUFunction(this, TEXT("OnEngageInteraction"));
	ActionIndicator->GetOnEngageInteractionDelegate().Add(OnEngageInteractionDelegate);

	bBlockInput = false;
	AutoReceiveInput = EAutoReceiveInput::Player0;
	InputPriority = 1;

	SteeringAgentComponent->DisableSteering();

	OnGearVRTouchpadTapDelegate.BindUFunction(this, TEXT("OnLookInteraction"));
	TouchpadGearVR->OnSingleTap.Add(OnGearVRTouchpadTapDelegate);

	OnGearVRBackKeyDelegate.BindUFunction(this, TEXT("OnBackKey"));
	BackKeyGearVR->OnBackClicked.Add(OnGearVRBackKeyDelegate);
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

#if PLATFORM_ANDROID == 1
	// Adding debug controls for editor and PC builds
	check(InputComponent);
	InputComponent->BindAction("LookInteraction", EInputEvent::IE_Pressed, this, &APlayerPawn::OnLookInteraction);
	InputComponent->BindAxis("LookRight", this, &APlayerPawn::OnLookRight);
	InputComponent->BindAxis("LookUp", this, &APlayerPawn::OnLookUp);
#endif
}

void APlayerPawn::OnBackKey()
{
	// Directly handled in Level Blueprint atm (needs to be handled here to cancel selections etc.
}

void APlayerPawn::OnLookInteraction()
{
	UE_LOG(Generic, Warning, TEXT("APlayerPawn::OnLookInteraction"));

	if (CurrentLookAtActor.IsValid())
	{
		ISelectableObject* CurrentLookAtSelectable = Cast<ISelectableObject>(CurrentLookAtActor.Get());
		switch (CurrentLookAtSelectable->GetType())
		{
		case ESelectableObjectType::Prop:
		case ESelectableObjectType::Resource:
		case ESelectableObjectType::EnemySpaceship:
			UE_LOG(Generic, Warning, TEXT("APlayerPawn::OnLookInteraction A"));
			CurrentLookAtSelectable->Select();
			if (SelectedActor.IsValid())
			{
				ISelectableObject* SelectedSelectable = Cast<ISelectableObject>(SelectedActor.Get());
				SelectedSelectable->Deselect();
			}
			SelectedActor = CurrentLookAtActor.Get();
			break;
		case ESelectableObjectType::UI:
			UE_LOG(Generic, Warning, TEXT("APlayerPawn::OnLookInteraction B"));
			CurrentLookAtSelectable->Select();
			// Do not save selection, just trigger the ui element
			break;
		case ESelectableObjectType::PlayerControlledSpaceship:
			UE_LOG(Generic, Warning, TEXT("APlayerPawn::OnLookInteraction C"));
			CurrentLookAtSelectable->Select();
			ActionIndicator->DisableActionIndication();
			if (SelectedActor.IsValid())
			{
				ISelectableObject* SelectedSelectable = Cast<ISelectableObject>(SelectedActor.Get());
				SelectedSelectable->Deselect();
			}
			SelectedActor = CurrentLookAtActor.Get();
			ActionIndicator->EnableActionIndication();
			break;
		default:
			UE_LOG(Generic, Warning, TEXT("APlayerPawn::OnLookInteraction D"));
			break;
		}	
	}
	else if (ActionIndicator->IsActionIndicationEnabled())
	{
		UE_LOG(Generic, Warning, TEXT("APlayerPawn::OnLookInteraction E"));
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
	// Get the look orientation vector from the HMD or from the Camera rotation if no HMD is available
	FVector DeviceRotationVector;
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		FRotator DeviceRotation;
		FVector DevicePosition;
		UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(DeviceRotation, DevicePosition);
		HMDRotationFollower->SetRelativeRotation(DeviceRotation);
		DeviceRotationVector = HMDRotationFollower->GetComponentRotation().Vector();
	}
	else
	{
		DeviceRotationVector = Camera->GetComponentRotation().Vector();
	}


	// Line Trace from camera in look orientation 
	const FVector Start = GetPawnViewLocation();
	const FVector End = Start + DeviceRotationVector * 100000;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	FHitResult HitData(ForceInit);
	UKismetSystemLibrary::LineTraceSingle_NEW(GetWorld(), Start, End, TRACE_TYPE_CAMERA,
		false, ActorsToIgnore, EDrawDebugTrace::None, HitData, true);

	TWeakObjectPtr<class AActor> LastLookAtActor = CurrentLookAtActor;
	CurrentLookAtActor = HitData.Actor;


	// Fire Gaze Events and reset Gaze cursor if no new actor was hit
	const bool bLookAtActorHasChanged = (LastLookAtActor != CurrentLookAtActor);
	if (bLookAtActorHasChanged)
	{
		// Fire GazeEnd Event if last looked at Selectable is valid
		if (LastLookAtActor.IsValid())
		{
			ISelectableObject* LastLookAtSelectable = Cast<ISelectableObject>(LastLookAtActor.Get());
			LastLookAtSelectable->GazeEnd();
		}

		// Fire GazeBegin Event if current looked at Selectable is valid, also set the recticle color
		// according to Selectable type and play the Recticle animation
		if (CurrentLookAtActor.IsValid())
		{
			ISelectableObject* CurrentLookAtSelectable = Cast<ISelectableObject>(CurrentLookAtActor.Get());
			CurrentLookAtSelectable->GazeBegin();

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
		else
		{
			// otherwise reset the Recticle scale, color and animation
			Recticle->SetSpriteColor(RecticleColorNeutral);
			RecticleRoot->SetRelativeScale3D(FVector(1.f));
			Recticle->SetPlaybackPosition(0.f, false);
			Recticle->Stop();
		}
	}


	// Update Recticle position and scale to match the distance to the currently looked at vector
	if (CurrentLookAtActor.IsValid())
	{
		float HitDistance = (HitData.ImpactPoint - Start).Size();
		RecticleRoot->SetWorldLocation(GetPawnViewLocation() + DeviceRotationVector * HitDistance); // TODO wrong position if PlayerPawn HMD rotation is not in World space
		RecticleRoot->SetRelativeScale3D(FVector(HitDistance / RecticleDefaultDistance));
	}
	else
	{
		RecticleRoot->SetWorldLocation(GetPawnViewLocation() + DeviceRotationVector * RecticleDefaultDistance);
	}
	

	// Update the Recticle orientation to always face the player
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