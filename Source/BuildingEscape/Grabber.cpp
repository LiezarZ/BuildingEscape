// Copyright Tobias Zepp

#include "Grabber.h"
#include "Engine/World.h"

#define OUT


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}



// Called when the game starts - like the main method
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputComponent();
}


/// Look for attached Physics Handle in DefaultPawn
void UGrabber::FindPhysicsHandleComponent()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle)
	{
		/// if found
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing physics handle component"), *GetOwner()->GetName())
	}
}


/// Look for attached Input Component (only appears at run time)
void UGrabber::SetupInputComponent()
{
	InputComponet = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponet)
	{
		/// if found
		UE_LOG(LogTemp, Warning, TEXT("Input component found"))
		InputComponet->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponet->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		/// error message
		UE_LOG(LogTemp, Error, TEXT("%s missing input component"), *GetOwner()->GetName())
	}
}



void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab pressed"))

	/// LINE TRACE and see if we reach any actors with physics body collision channel set
	auto HitResult = GetFirstPhysicsBodyinReach();
	auto ComponentToGrab = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();

	/// if we hit something then attach a physics handle
	if (ActorHit != nullptr)
	{
		/// attach physics handle
		PhysicsHandle->GrabComponent(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true // allow rotastion
		);
	}

}



void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab released"))
	//TODO release physics
	PhysicsHandle->ReleaseComponent();
}



// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	/// Get player view point thist tack
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	/// OUT Parameters get changed!
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	/// Draw a red trace in the world to visualise by adding both vectors / lines
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	// if the physics handle is attached
	if (PhysicsHandle->GrabbedComponent)
	{
		// move the object that we're holding
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}
	

}



const FHitResult UGrabber::GetFirstPhysicsBodyinReach()
{
	/// Get player view point thist tack
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	/// OUT Parameters get changed!
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	/// Draw a red trace in the world to visualise by adding both vectors / lines
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
	/*DrawDebugLine(
	GetWorld(),
	PlayerViewPointLocation,
	LineTraceEnd,
	FColor(255, 0, 0),
	false,
	0.f,
	0.f,
	10.f
	);*/

	/// Setup query parameters
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	/// Line-trace (Ray-Cast) out to reacht distance
	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	/// See what we hit
	AActor* ActorHit = Hit.GetActor();
	if (ActorHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Line trace hit: %s"), *(ActorHit->GetName()))
	}

	return Hit;
}