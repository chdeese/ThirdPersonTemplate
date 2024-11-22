// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Curves/CurveFloat.h"

// Sets default values
AMovingPlatform::AMovingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* sceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	sceneComponent->SetupAttachment(RootComponent);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMesh->SetupAttachment(sceneComponent);

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(StaticMesh);
}

// Called when the game starts or when spawned
void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	//only do things in contructor to yourself, this is editing another thing, so it needs to be in BeginPlay.
	//adding a component does not finish initializing that component.
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AMovingPlatform::OnTriggerOverlapBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &AMovingPlatform::OnTriggerOverlapEnd);

	StartPosition = GetActorLocation();
}

// Called every frame
void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//verify curve exists
	if (MovementCurve == nullptr) return;

	//increase/decrease progress
	if (bIsProgressing)
		Progress += DeltaTime;
	else
		Progress -= DeltaTime;

	Progress = FMath::Clamp(Progress, 0, 1);

	//sample curve
	float value = MovementCurve->GetFloatValue(Progress);

	//apply movement
	FVector newLocation = FMath::Lerp(FVector::Zero(), EndPosition, value);
	StaticMesh->SetRelativeLocation(newLocation);
}


void AMovingPlatform::OnTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Forward();
}
void AMovingPlatform::OnTriggerOverlapEnd(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Reverse();
}


void AMovingPlatform::Forward()
{
	bIsProgressing = true;
}

void AMovingPlatform::Reverse()
{
	bIsProgressing = false;
}

