// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AThirdPersonCharacter::AThirdPersonCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a CameraBoom to manage the camera distance and rotation
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;  // Distance behind character
	CameraBoom->bUsePawnControlRotation = true;  // Rotate arm based on character

	// Create a FollowCamera attached to the CameraBoom
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;  // Camera does not rotate relative to the arm
}

// Called when the game starts or when spawned
void AThirdPersonCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Set Default Speeds
	WalkingSpeed = 200.0f;
	RunningSpeed = 600.0f;

	// Set the initial walk speed
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
	}

	// Initially not sprinting
	isSprinting = false;
}

// Called every frame
void AThirdPersonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind axis for movement
	PlayerInputComponent->BindAxis("MoveForward", this, &AThirdPersonCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AThirdPersonCharacter::MoveRight);

	// Bind action for jump
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AThirdPersonCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AThirdPersonCharacter::StopJumping);

	// Bind Camera rotation
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	// Bind sprint actions (Shift Key)
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AThirdPersonCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AThirdPersonCharacter::SprintStop);
}

void AThirdPersonCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// Get the controller's rotation and derive the forward direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Move in that direction
		AddMovementInput(Direction, Value);
	}
}

void AThirdPersonCharacter::MoveForward(float Value)
{
	// Check the flag before movement can begin
	if (!canMove)
	{
		return;
	}

	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// Get the controller's rotation and derive the forward direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// Move in that direction
		AddMovementInput(Direction, Value);
	}
}

void AThirdPersonCharacter::SprintStart()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
		isSprinting = true;
	}
}

void AThirdPersonCharacter::SprintStop()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
		isSprinting = false;
	}
}

