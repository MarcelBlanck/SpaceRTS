

#include "SpaceRTS.h"
#include "PlayerPawn.h"
#include "Interfaces/SelectableObject.h"
#include "PaperFlipbook.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

APlayerPawn::APlayerPawn(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	Camera(nullptr),
	MaxInteractionDistance(20000.f),
	RecticleDefaultDistance(3000.f),
	RecticleAnimationRate(5.f),
	RecticleAlpha(0.2f),
	RecticleColorNeutral(1.f, 1.0, 1.f, RecticleAlpha),
	RecticleColorFriendly(0.f, 1.0, 0.f, RecticleAlpha),
	RecticleColorAttack(1.f, 0.0, 0.f, RecticleAlpha),
	RecticleColorInteract(0.f, 0.0, 1.f, RecticleAlpha)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	Camera = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("PlayerCamera"));
	Camera->AttachTo(RootComponent);
	Camera->SetRelativeScale3D(FVector(0.2f));

	ConstructorHelpers::FObjectFinder<UPaperFlipbook> RecticleFlipbook(TEXT("PaperFlipbook'/Game/GUI/Recticle/RecticleFlipbook.RecticleFlipbook'"));
	checkf(RecticleFlipbook.Object, TEXT("Did not find PaperFlipbook'/Game/GUI/Recticle/RecticleFlipbook.RecticleFlipbook' - Maybe the asset was moved or renamed."));
	Recticle = ObjectInitializer.CreateDefaultSubobject<UPaperFlipbookComponent>(this, TEXT("Recticle"));
	Recticle->SetFlipbook(RecticleFlipbook.Object);
	Recticle->SetLooping(false);
	Recticle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Recticle->SetCollisionProfileName(FName("NoCollision"));
	Recticle->bGenerateOverlapEvents = false;
	Recticle->SetEnableGravity(false);
	Recticle->AttachTo(Camera);
	Recticle->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	Recticle->SetRelativeLocation(FVector(RecticleDefaultDistance, 0.f, 0.f));
	Recticle->SetRelativeScale3D(FVector(1.f));
	Recticle->SetPlaybackPosition(0.f, false);
	Recticle->SetSpriteColor(RecticleColorNeutral);
	Recticle->Stop();

	SteeringAgentComponent->DisableSteering();
}

FVector APlayerPawn::GetPawnViewLocation() const
{
	return GetActorLocation() + CameraLocation;
}

void APlayerPawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	CameraLocation = Camera->GetComponentLocation();
}

void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	EnableInput(GetWorld()->GetFirstPlayerController());
}

void APlayerPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	//TickFpsCounter();
	
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		ApplyOrientationFromHMD();
	}

	UpdateLookAtActorAndRecticle();
}


void APlayerPawn::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	check(InputComponent);

	InputComponent->BindAction("LookInteraction", EInputEvent::IE_Pressed, this, &APlayerPawn::OnLookInteraction);
	InputComponent->BindAxis("LookRight", this, &APlayerPawn::OnLookRight);
	InputComponent->BindAxis("LookUp", this, &APlayerPawn::OnLookUp);
}

void APlayerPawn::OnLookInteraction()
{
	SelectedActor = CurrentLookAtActor;
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
	
	const FVector Start = Camera->GetComponentLocation();
	const FVector End = Start + Camera->GetComponentRotation().Vector() * 100000;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	FHitResult HitData(ForceInit);
	UKismetSystemLibrary::LineTraceSingle_NEW(GetWorld(), Start, End, ETraceTypeQuery::TraceTypeQuery2, 
		false, ActorsToIgnore, EDrawDebugTrace::None, HitData, true);

	AActor* LastLookAtActor = CurrentLookAtActor.Get();
	CurrentLookAtActor = HitData.Actor;
	bool bLookAtActorHasChanged = (LastLookAtActor != CurrentLookAtActor);

	if (CurrentLookAtActor.IsValid())
	{
		float HitDistance = (HitData.ImpactPoint - Start).Size();
		Recticle->SetRelativeLocation(FVector(HitDistance, 0.f, 0.f));
		Recticle->SetRelativeScale3D(FVector(HitDistance / RecticleDefaultDistance));

		

		if (bLookAtActorHasChanged)
		{
			ISelectableObject* Object = Cast<ISelectableObject>(CurrentLookAtActor.Get());
			if (Object != nullptr)
			{
				switch (Object->GetType())
				{
					case ESelectableObjectType::Prop:
						Recticle->SetSpriteColor(RecticleColorNeutral);
						break;
					case ESelectableObjectType::UI:
						Recticle->SetSpriteColor(RecticleColorNeutral);
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
			Recticle->SetRelativeLocation(FVector(RecticleDefaultDistance, 0.f, 0.f));
			Recticle->SetRelativeScale3D(FVector(1.f));
			Recticle->SetPlaybackPosition(0.f, false);
			Recticle->Stop();
		}
	}
}

void APlayerPawn::ApplyOrientationFromHMD()
{
	FRotator DeviceRotation;
	FVector DevicePosition;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(DeviceRotation, DevicePosition);

	Camera->SetRelativeRotation(DeviceRotation);
}