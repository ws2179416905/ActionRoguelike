// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Engine/Public/DrawDebugHelpers.h"


// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SM_Barrel = CreateDefaultSubobject<UStaticMeshComponent>("SM_Barrel");
	RootComponent = SM_Barrel;
	SM_Barrel->SetSimulatePhysics(true);

	ExplosionForce = CreateDefaultSubobject<URadialForceComponent>("ExplosionForce");
	ExplosionForce->SetupAttachment(RootComponent);
	ExplosionForce->SetAutoActivate(false);
	ExplosionForce->Radius = 750.f;
	ExplosionForce->ImpulseStrength = 2500.f;
	ExplosionForce->bImpulseVelChange = true;
	ExplosionForce->AddCollisionChannelToAffect(ECollisionChannel::ECC_WorldDynamic);
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();

}

void ASExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SM_Barrel->OnComponentHit.AddDynamic(this, &ASExplosiveBarrel::OnActorHit);
}

// Called every frame
void ASExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASExplosiveBarrel::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ExplosionForce->FireImpulse();


	UE_LOG(LogTemp, Log, TEXT("Explosion FireImpulse"));

	UE_LOG(LogTemp, Warning, TEXT("Hit actor name: %s | time: %f"), *GetNameSafe(OtherActor), GetWorld()->TimeSeconds);

	FString CombineStr = FString::Printf(TEXT("Hit location: %s"), *(Hit.ImpactPoint.ToString()));
	DrawDebugString(GetWorld(), Hit.ImpactPoint, CombineStr, nullptr, FColor::Green, 3.f, true);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, *CombineStr);
}

