

#include "SpaceRTS.h"
#include "PlayerPawn.h"
#include "Kismet/KismetSystemLibrary.h"

APlayerPawn::APlayerPawn(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	Camera(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	Camera = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("PlayerCamera"));
	Camera->AttachTo(RootComponent);
	
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

	//UpdateLookAtActorAndCursor();
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
	
}

void APlayerPawn::OnLookRight(float Value)
{
	if (Camera != nullptr)
	{
		FRotator Rotator(0.f, Value, 0.f);
		Camera->AddRelativeRotation(Rotator);
	}

}

void APlayerPawn::OnLookUp(float Value)
{
	if (Camera != nullptr)
	{
		FRotator Rotator(Value, 0.f, 0.f);
		Camera->AddRelativeRotation(Rotator);
	}
}

void APlayerPawn::UpdateLookAtActorAndCursor()
{
	FCollisionQueryParams TraceParams(FName(TEXT("CurrentLookAtActorTrace")), true, this);
	TraceParams.bReturnPhysicalMaterial = false;

	//location the PC is focused on
	const FVector Start = Camera->GetComponentLocation();
	const FVector End = Start + Camera->GetComponentRotation().Vector() * 100000;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	FHitResult HitData(ForceInit);
	UKismetSystemLibrary::LineTraceSingle_NEW(GetWorld(), Start, End, ETraceTypeQuery::TraceTypeQuery3, false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, HitData, true);


	CurrentLookAtActor = HitData.Actor;

	if (CurrentLookAtActor.IsValid())
	{
		UE_LOG(Generic, Warning, TEXT("Looking at Actor %s"), *CurrentLookAtActor->GetName());
	}
	else
	{
		UE_LOG(Generic, Warning, TEXT("Looking at Actor NONE "));
	}
}