// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"
#include "DrawDebugHelpers.h"
#include "SGameplayInterface.h"


// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USInteractionComponent::PrimaryInteract()
{
	TArray<FHitResult> ArrHits;

	FVector Start;
	FVector End;

	AActor* MyOwner = GetOwner();
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(Start, EyeRotation);
	End = Start + EyeRotation.Vector() * 1000.f;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	FCollisionShape Shape;
	float SphereRadius = 30.f;
	Shape.SetSphere(SphereRadius);

	bool bHit = GetWorld()->SweepMultiByObjectType(ArrHits, Start, End, FQuat::Identity, ObjectQueryParams, Shape);

	for (FHitResult Hit : ArrHits)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			if (HitActor->Implements<USGameplayInterface>())
			{
				APawn* MyPawn = Cast<APawn>(MyOwner);
				ISGameplayInterface::Execute_Interact(HitActor, MyPawn);

				DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(10.f, 10.f, 10.f), FColor::Blue, false, 5.f, 0.f, 2.f);
				//break;
			}
		}
	}

	FColor LineColor = bHit ? FColor::Green : FColor::Red;
	DrawDebugLine(GetWorld(), Start, End, LineColor, false, 5.f, 0.f, 2.f);
}

