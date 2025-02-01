// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/BoxComponent.h"
#include "GAM312Character.h"
#include "Perception/AIPerceptionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
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
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
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

	// Get all currently perceived actors (including the player)
	TArray<AActor*> OverlappedActors;
	AIPerComp->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), OverlappedActors);

	// Simulate immediate sensing upon game start
	OnSensed(OverlappedActors);

	// Check for the player directly if perception hasn't picked it up yet
	AGAM312Character* PlayerCharacter = Cast<AGAM312Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (PlayerCharacter)
	{
		float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());

		// If the player is already within attack range, start attacking
		if (DistanceToPlayer <= 150.0f)
		{
			CurrentVelocity = FVector::ZeroVector;
			bIsAttacking = true;
			GetCharacterMovement()->DisableMovement();  // Disable movement
			GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, FTimerDelegate::CreateUObject(this, &AEnemy::AttackPlayer, PlayerCharacter), AttackInterval, true);

			UE_LOG(LogTemp, Display, TEXT("Player detected at game start, attacking!"));
		}
	}
}



// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Move the enemy only if it's not currently attacking
	if (!bIsAttacking && !CurrentVelocity.IsZero())
	{
		UpdatedLocation = GetActorLocation() + CurrentVelocity * DeltaTime;

		// Check if the enemy is supposed to move back to base
		if (BackToBaseLocation)
		{
			// Don't allow movement to base if wolf is attacking
			if (!bIsAttacking)  // Make sure this movement doesn't happen while attacking
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

		GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, FTimerDelegate::CreateUObject(this, &AEnemy::AttackPlayer, Char), AttackInterval, true);
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

		if (Info.LastSensedStimuli[0].WasSuccessfullySensed())  // Check if the player was successfully sensed
		{
			AGAM312Character* Char = Cast<AGAM312Character>(UpdatedActors[i]);
			if (Char)
			{
				float DistanceToPlayer = FVector::Dist(GetActorLocation(), Char->GetActorLocation());

				if (DistanceToPlayer <= 150.0f)  // Assuming 150.0f is your attack range
				{
					// Player is within range, stop movement and trigger attack immediately
					CurrentVelocity = FVector::ZeroVector;
					BackToBaseLocation = false;  // Prevent wolf from moving back to base

					// Stop movement and start attacking
					bIsAttacking = true;
					GetCharacterMovement()->DisableMovement();  // Disable all movement
					GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, FTimerDelegate::CreateUObject(this, &AEnemy::AttackPlayer, Char), AttackInterval, true);

					
				}
				else
				{
					// If player is out of attack range, move towards the player
					FVector dir = Char->GetActorLocation() - GetActorLocation();
					dir.Z = 0.0f;

					// Only move if not currently attacking
					if (!bIsAttacking)
					{
						CurrentVelocity = dir.GetSafeNormal() * MovementSpeed;
						SetNewRotation(Char->GetActorLocation(), GetActorLocation());
					}
				}
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

void AEnemy::AttackPlayer(AGAM312Character* Char)
{
	if (Char && Char->Health > 0)  // Check if player character is still alive
	{
		// Calculate distance to the player
		float DistanceToPlayer = FVector::Dist(GetActorLocation(), Char->GetActorLocation());

		if (DistanceToPlayer <= 150.0f)  // Assuming 150.0f is your attack range
		{
			// Stop all movement by setting the velocity to zero and setting the attacking flag
			CurrentVelocity = FVector::ZeroVector;
			bIsAttacking = true;

			

			// Disable movement component entirely if present
			if (GetCharacterMovement())
			{
				GetCharacterMovement()->DisableMovement();
			}

			// Apply damage to the player
			Char->DealDamage(DamageValue);

			// Play bite animation if available
			if (BiteMontage)
			{
				PlayAnimMontage(BiteMontage);
			}
		}
		else
		{
			// Stop attacking if the player is out of range
			GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
			bIsAttacking = false;  // Allow movement again after attack ends
			UE_LOG(LogTemp, Display, TEXT("Player out of range, movement can resume"));

			// Optionally, restart movement to chase the player
			FVector DirectionToPlayer = Char->GetActorLocation() - GetActorLocation();
			CurrentVelocity = DirectionToPlayer.GetSafeNormal() * MovementSpeed;

			// Enable movement again if needed
			if (GetCharacterMovement())
			{
				GetCharacterMovement()->SetMovementMode(MOVE_Walking);  // Re-enable walking mode
			}
		}
	}
	else
	{
		// Stop attacking if player is dead or invalid
		GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
		bIsAttacking = false;

		// Log that the attack stopped
		UE_LOG(LogTemp, Display, TEXT("Attack stopped due to player death or invalidity"));

		// Ensure movement is enabled again
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
	}
}
