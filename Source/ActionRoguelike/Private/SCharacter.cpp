// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "SInteractionComponent.h"
#include "Engine/Public/DrawDebugHelpers.h"


// Sets default values
ASCharacter::ASCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("InteractionComp");

	bUseControllerRotationYaw = false;
	SpringArmComponent->bUsePawnControlRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ASCharacter::Moveforward(float InVal)
{
	FRotator ControlRoa = GetControlRotation();
	ControlRoa.Pitch = 0.f;
	ControlRoa.Roll = 0.f;

	AddMovementInput(ControlRoa.Vector(), InVal);
}

void ASCharacter::MoveRight(float InVal)
{
	FRotator ControlRoa = GetControlRotation();
	ControlRoa.Pitch = 0.f;
	ControlRoa.Roll = 0.f;

	FVector RightVector = UKismetMathLibrary::GetRightVector(ControlRoa);

	AddMovementInput(RightVector, InVal);
}

void ASCharacter::PrimaryAttack()
{
	PlayAnimMontage(AttackAnim);

	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::PrimaryAttack_TimeElapsed, 0.2f);

	//GetWorldTimerManager().ClearTimer(TimerHandle);
}

void ASCharacter::PrimaryAttack_TimeElapsed()
{
	FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");

	FTransform SpawnTM = FTransform(GetControlRotation(), HandLocation);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;

	// 发射方向调整
	FVector tCameraPos = CameraComponent->GetComponentLocation();
	FRotator tCameraRoa = CameraComponent->GetComponentRotation();
	FHitResult tOutHit;
	FVector tEnd = HandLocation + tCameraRoa.Vector() * 3000.f;
	FCollisionObjectQueryParams tObjectQueryParams;
	tObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);
	tObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
	tObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_PhysicsBody);
	tObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	FCollisionQueryParams Params;
	bool bHit = GetWorld()->LineTraceSingleByObjectType(tOutHit, tCameraPos, tEnd, tObjectQueryParams, Params);
	FVector tTraceEnd = bHit ? tOutHit.ImpactPoint : tOutHit.TraceEnd;
	FString tTraceEndStr = tTraceEnd.ToString();
	UE_LOG(LogTemp, Warning, TEXT("TraceEnd Postion: %s"), *tTraceEndStr);
	FVector tDirection = tTraceEnd - HandLocation;
	tDirection.Normalize();
	FRotator tLookAtRotation = FRotationMatrix::MakeFromX(tDirection).Rotator();
	SpawnTM.SetRotation(tLookAtRotation.Quaternion());
	FColor tDebugColor = bHit ? FColor::Blue : FColor::Black;
	DrawDebugLine(GetWorld(), tCameraPos, tEnd, tDebugColor, false, 7.f, 0.f, 2.f);
	if (bHit)
	{
		DrawDebugSphere(GetWorld(), tOutHit.ImpactPoint, 35.f, 30, tDebugColor, false, 7.f, 0, 2.f);
	}
	// 发射方向调整

	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
}

void ASCharacter::PrimaryInteract()
{
	if (InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Moveforward", this, &ASCharacter::Moveforward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
}

