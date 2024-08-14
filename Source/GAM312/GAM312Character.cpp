// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAM312Character.h"
#include "Projectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "FPSGameMode.h"




//////////////////////////////////////////////////////////////////////////
// AGAM312Character

AGAM312Character::AGAM312Character()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
}



void AGAM312Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void AGAM312Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGAM312Character::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGAM312Character::Look);

		// Bind DisplayRaycast event
		PlayerInputComponent->BindAction("Raycast", IE_Pressed, this, &AGAM312Character::DisplayRaycast);
	}
}


void AGAM312Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AGAM312Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGAM312Character::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AGAM312Character::GetHasRifle()
{
	return bHasRifle;
}

void AGAM312Character::DisplayRaycast()
{
	// Create a FHitResult object to store information about the hit
	FHitResult HitResult;

	// Get the starting position of the line trace from the camera component
	FVector StartTrace = FirstPersonCameraComponent->GetComponentLocation();

	// Get the forward vector of the camera component
	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();

	// Calculate the end position of the line trace based on the forward vector and distance
	FVector EndTrace = (ForwardVector * 3319.0f) + StartTrace;

	// Create a FCollisionQueryParams object to customize the behavior of the line trace
	FCollisionQueryParams* TraceParams = new FCollisionQueryParams();

	// Perform a line trace using the specified parameters
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Visibility, *TraceParams))
	{
		// Draw a debug line in the world to visualize the line trace
		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor(255, 0, 0), true);
		// Display a debug message on the screen with information about the hit actor
		GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, FString::Printf(TEXT("You hit: %s"), *HitResult.GetActor()->GetName()));
	}
}

void AGAM312Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

	// Check to see if player has fell off the map
	if (GetActorLocation().Z < KillHeight)
	{
		// If player fell off map activate respawn
		Respawn();
	}
}

void AGAM312Character::Respawn()
{
	// Destroy the previous player controller
	APlayerController* OldPlayerController = GetController<APlayerController>();
	if (OldPlayerController)
	{
		OldPlayerController->UnPossess();
		OldPlayerController->Destroy();
	}

	// Spawn and possess a new player controller
	APlayerController* NewPlayerController = GetWorld()->SpawnActor<APlayerController>(APlayerController::StaticClass());

	// Reset the character's location to a spawn point
	SetActorLocation(RespawnLocation);

	//Possess the new player controller
	NewPlayerController->Possess(this);
}

// Function that deals damage to enemy
void AGAM312Character::DealDamage(float DamageAmount)
{
	Health -= DamageAmount;

	if (Health <= 0.0f)
	{
		Respawn();
	}
}
