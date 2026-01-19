// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Piggy.generated.h"


class USkeletalMeshComponent; 
class USphereComponent; 
class USpringArmComponent; 
class UCameraComponent; 
class UInputAction;
class UInputMappingContext; 
struct FInputActionValue; 

UCLASS()
class ASSIGNMENT_07_API APiggy : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APiggy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void MoveUp(const FInputActionValue& Value);
	void MoveDown(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);
	void LookRoll(const FInputActionValue& Value);


	void Zoom(const FInputActionValue& Value); 


protected:

	void CheckGround(); 

	bool IsFalling() { return bIsFalling; }

	void UpdateSpeed(float DeltaTime);

private: 

	// Components 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (allowprivateaccess = true), Category = "Component")
	USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (allowprivateaccess = true), Category = "Component")
	USphereComponent* SphereCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (allowprivateaccess = true), Category = "Component")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (allowprivateaccess = true), Category = "Component")
	UCameraComponent* FollowCamera;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;

	// Input Actions 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveDownAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookRollAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* ZoomAction;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed = 600.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float CurrentSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsFalling = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AirMovementRegist = 0.4f;

protected: 
	FVector PreviousLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	float ZoomStep = 200.f;

	float CurrentRoll = 0.f;

	float VerticalVelocity = 0.f; 

	const float GravityConstant = -980.f;
};

