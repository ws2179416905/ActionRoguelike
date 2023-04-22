// Fill out your copyright notice in the Description page of Project Settings.


#include "SDashProjectile.h"
#include "Engine/Public/DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"


ASDashProjectile::ASDashProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASDashProjectile::BeginPlay()
{
	Super::BeginPlay();

	SphereComp->OnComponentHit.AddDynamic(this, &ASDashProjectile::OnActorHit);

	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASDashProjectile::PlayerDash, 1.5f);
}

void ASDashProjectile::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	APawn* MyOwner = GetInstigator();
	if (ensureAlways(MyOwner))
	{
		if (MyOwner != OtherActor)
		{
			GetWorldTimerManager().ClearTimer(TimerHandle);

			PlayerDash();
			this->Destroy();
		}
	}
}

void ASDashProjectile::PlayerDash()
{
	APawn* MyOwner = GetInstigator();
	if (ensureAlways(MyOwner))
	{
		FTransform MyTrans = GetActorTransform();
		MyTrans.SetScale3D(FVector(10.f, 10.f, 10.f));
		//DrawDebugSphere(GetWorld(), MyTrans.GetLocation(), 35.f, 12, FColor::Green, false, 7.f, 0, 1.f);
		if (ensureAlways(DashParticle))
		{
			UParticleSystemComponent* const ParticleSystemComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DashParticle, MyTrans);
		}

		FVector DashTarget = GetActorLocation();
		MyOwner->SetActorLocation(DashTarget);
	}
}
