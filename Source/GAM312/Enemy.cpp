// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/BoxComponent.h"
#include "GAM312Character.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Creates and attachs the Damage Collision Component
	DamageCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Damage Collision"));
	DamageCollision->SetupAttachment(RootComponent);

	// Creates the AI Perception Component and Sigh Config
	AIPerComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));

	// Configures Sight Config parameters
	SightConfig->SightRadius = 1250.0f;
	SightConfig->LoseSightRadius = 1285.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->SetMaxAge(0.1f);

	// Configures AI Perception Component
	AIPerComp->ConfigureSense(*SightConfig);
	AIPerComp->SetDominantSense(SightConfig->GetSenseImplementation());
	AIPerComp->OnPerceptionUpdated.AddDynamic(this, &AEnemy::OnSensed);

	// Initialize Movement parameters
	CurrentVelocity = FVector::ZeroVector;
	MovementSpeed = 375.0f;
	DistanceSquared = BIG_NUMBER;

	// Initialize BiteMontage
		static ConstructorHelpers::FObjectFinder<UAnimMontage> BiteMontageAsset(TEXT("/Game/_EnemyAnim/BiteMontage"));
	if (BiteMontageAsset.Succeeded())
	{
		BiteMontage = BiteMontageAsset.Object;
	}
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind OnHit function to DamageCollision's overlap event
	DamageCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnHit);

	// Store the initial location as the base location
	BaseLocation = this->GetActorLocation();
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Move the enemy based on the CurrentVelocity
	if (!CurrentVelocity.IsZero())
	{
		UpdatedLocation = GetActorLocation() + CurrentVelocity * DeltaTime;

		if (BackToBaseLocation)
		{
			// Check if the enemy has reached the base location
			if ((UpdatedLocation - BaseLocation).SizeSquared2D() < DistanceSquared)
			{
				DistanceSquared = (UpdatedLocation - BaseLocation).SizeSquared2D();
			}
			else
			{
				// Stop moving, reset parameters, and set new rotation
				CurrentVelocity = FVector::ZeroVector;
				DistanceSquared = BIG_NUMBER;
				BackToBaseLocation = false;

				SetNewRotation(GetActorForwardVector(), GetActorLocation());
			}
		}
		// Update the actor's location
		SetActorLocation(UpdatedLocation);
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Function called when the enemy collides with another actor
void AEnemy::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	AGAM312Character* Char = Cast<AGAM312Character>(OtherActor);

	if (Char)
	{
		// Deal damage to the character
		Char->DealDamage(DamageValue);

		if (BiteMontage)
		{
			PlayAnimMontage(BiteMontage);

			UE_LOG(LogTemp, Display, TEXT("Montage Worked"));
		}

		
	}
}

// Function called when the enemy senses other actors
void AEnemy::OnSensed(const TArray<AActor*>& UpdatedActors)
{
	for (int i = 0; i < UpdatedActors.Num(); i++)
	{
		FActorPerceptionBlueprintInfo Info;

		// Retrieve perception information for the sensed actor
		AIPerComp->GetActorsPerception(UpdatedActors[i], Info);

		if (Info.LastSensedStimuli[0].WasSuccessfullySensed())
		{
			// Move towards the sensed actor
			FVector dir = UpdatedActors[i]->GetActorLocation() - GetActorLocation();
			dir.Z = 0.0f;

			CurrentVelocity = dir.GetSafeNormal() * MovementSpeed;

			// Set the rotation to face the sensed actor
			SetNewRotation(UpdatedActors[i]->GetActorLocation(), GetActorLocation());
		}
		else
		{
			// Move back to the base location if the actor is not sensed
			FVector dir = BaseLocation - GetActorLocation();
			dir.Z = 0.0f;

			if (dir.SizeSquared2D() > 1.0f)
			{
				CurrentVelocity = dir.GetSafeNormal() * MovementSpeed;
				BackToBaseLocation = true;

				// Set the rotation to face the base location
				SetNewRotation(BaseLocation, GetActorLocation());
			}
		}
	}
}

// Function to set the new rotation based on the target and current positions
void AEnemy::SetNewRotation(FVector TargetPosition, FVector CurrentPosition)
{
	FVector NewDirection = TargetPosition - CurrentPosition;
	NewDirection.Z = 0.0f;

	EnemyRotation = NewDirection.Rotation();

	// Set the actor's rotation
	SetActorRotation(EnemyRotation);
}

// Function to apply damage to the enemy and destroy it if health is zero or below
void AEnemy::DealDamage(float DamageAmount)
{
	Health -= DamageAmount;

	if (Health <= 0.0f)
	{
		Destroy();
	}
}

