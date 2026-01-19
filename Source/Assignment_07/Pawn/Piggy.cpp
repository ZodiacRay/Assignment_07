// Fill out your copyright notice in the Description page of Project Settings.


#include "Piggy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PawnMovementComponent.h"

// Sets default values
APiggy::APiggy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SetRootComponent(SphereCollision);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(SphereCollision);
	Mesh->SetSimulatePhysics(false);
	Mesh->SetEnableGravity(true); 

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(SphereCollision);
	CameraBoom->TargetArmLength = 200.f;
	CameraBoom->bUsePawnControlRotation = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->SetRelativeRotation(FRotator(0.f, -20.f, 0.f));

	
}

void APiggy::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	PreviousLocation = GetActorLocation();
}

void APiggy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSpeed(DeltaTime);

	CheckGround(); 

	if (bIsFalling)
	{
		VerticalVelocity += GravityConstant * DeltaTime;

		FVector VerticalMove = FVector(0, 0, VerticalVelocity * DeltaTime);
		AddActorLocalOffset(VerticalMove, true);
	}

}

// Called to bind functionality to input
void APiggy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APiggy::Move);
		EnhancedInputComp->BindAction(MoveUpAction, ETriggerEvent::Triggered, this, &APiggy::MoveUp);
		EnhancedInputComp->BindAction(MoveDownAction, ETriggerEvent::Triggered, this, &APiggy::MoveDown);
		EnhancedInputComp->BindAction(LookAction, ETriggerEvent::Triggered, this, &APiggy::Look);
		EnhancedInputComp->BindAction(LookRollAction, ETriggerEvent::Triggered, this, &APiggy::LookRoll);
		EnhancedInputComp->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &APiggy::Zoom);
	}
}

void APiggy::Move(const FInputActionValue& Value)
{
	if (!Controller) return;

	FVector2D MoveVector = Value.Get<FVector2D>(); 

	const FVector ForwardDir = GetActorForwardVector();
	const FVector RightDir = GetActorRightVector();

	FVector DeltaLocation = ForwardDir * MoveVector.Y + RightDir * MoveVector.X;
	float DelatTime = GetWorld()->GetDeltaSeconds(); 

	float FinalSpeed = bIsFalling ? MoveSpeed * AirMovementRegist : MoveSpeed;

	AddActorWorldOffset(DeltaLocation * FinalSpeed * DelatTime, true);
}

void APiggy::MoveUp(const FInputActionValue& Value)
{
	if (!Controller) return;

	AddActorLocalOffset(FVector::UpVector * MoveSpeed * GetWorld()->GetDeltaSeconds(), true);
	VerticalVelocity = 0; 
}

void APiggy::MoveDown(const FInputActionValue& Value)
{
	if (!Controller || !bIsFalling) return; 

	AddActorLocalOffset(FVector::DownVector * MoveSpeed * GetWorld()->GetDeltaSeconds(), true);
}

void APiggy::Look(const FInputActionValue& Value)
{
	if (!Controller) return;

	FVector2D LookVector = Value.Get<FVector2D>();
	
	AddActorLocalRotation(FRotator(0.f, LookVector.X, 0.f));

	if (CameraBoom)
	{
		FRotator NewArmRotation = CameraBoom->GetRelativeRotation();
	
		NewArmRotation.Pitch = FMath::Clamp(NewArmRotation.Pitch + (LookVector.Y * -1.f), -60.f, 60.f);
		CameraBoom->SetRelativeRotation(NewArmRotation);
	}

}

void APiggy::LookRoll(const FInputActionValue& Value)
{
	if (!Controller || !CameraBoom) return; 

	float RollValue = Value.Get<float>(); 

	FRotator NewArmRotation = CameraBoom->GetRelativeRotation();
	CurrentRoll = (CurrentRoll + (RollValue * 2.f));
	NewArmRotation.Roll = CurrentRoll; 

	CameraBoom->SetRelativeRotation(NewArmRotation);
}

void APiggy::Zoom(const FInputActionValue& Value)
{
	if (!Controller) return; 

	float ZoomAmount = Value.Get<float>(); 
	CameraBoom->TargetArmLength += ZoomAmount * ZoomStep;
}

void APiggy::CheckGround()
{
	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = Start + FVector::DownVector * (SphereCollision->GetScaledSphereRadius() + 3);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start, End, 
		ECC_Visibility,
		Params
	);

	if (bHit)
	{
		bIsFalling = false; 
		VerticalVelocity = 0.f;
	}
	else
	{
		bIsFalling = true;
	}
}

void APiggy::UpdateSpeed(float DeltaTime)
{
	FVector CurrentLocation = GetActorLocation();

	FVector Displacement = CurrentLocation - PreviousLocation;
	float DistanceMoved = Displacement.Size();
	float Speed = DistanceMoved / DeltaTime;

	FVector MovedDir = Displacement.GetUnsafeNormal();
	float DotDir = FVector::DotProduct(GetActorForwardVector(), MovedDir); 


	CurrentSpeed = DotDir >= 0 ? Speed : -Speed; 

	PreviousLocation = CurrentLocation;
}
