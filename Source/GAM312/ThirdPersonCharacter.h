// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ThirdPersonCharacter.generated.h"

UCLASS()
class GAM312_API AThirdPersonCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AThirdPersonCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Movement Function
	void MoveRight(float Value);
	void MoveForward(float Value);

	// Sprint Function
	void SprintStart();
	void SprintStop();

	// Camera Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkingSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool isSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool canMove;
};
