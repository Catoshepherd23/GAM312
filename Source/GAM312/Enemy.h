// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class GAM312_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Box component for handling damage collision
	UPROPERTY(EditAnywhere)
	class UBoxComponent* DamageCollision;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* BiteMontage;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	

	// Function called when the enemy collides with another actor
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);


	// AI Perception Component for handling enemy perception
	UPROPERTY(VisibleDefaultsOnly, Category = Enemy)
	class UAIPerceptionComponent* AIPerComp;

	// Sight configuration for the AI Perception Component
	UPROPERTY(VisibleDefaultsOnly, Category = Enemy)
	class UAISenseConfig_Sight* SightConfig;

	// Function called when the enemy senses other actors
	UFUNCTION()
	void OnSensed(const TArray<AActor*>& UpdatedActors);

	// Rotation of the enemy
	UPROPERTY(VisibleAnywhere, Category = Movement)
	FRotator EnemyRotation;

	// Initial location of the enemy
	UPROPERTY(VisibleAnywhere, Category = Movement)
	FVector BaseLocation;

	// Current velocity of the enemy
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	FVector CurrentVelocity;

	// Movement speed of the enemy
	UPROPERTY(VisibleAnywhere, Category = Movement)
	float MovementSpeed;

	// Function to set the new rotation based on the target and current positions
	void SetNewRotation(FVector TargetPosition, FVector CurrentPosition);

	// Flag indicating whether the enemy is returning to its base location
	bool BackToBaseLocation;

	// New location for the enemy
	FVector UpdatedLocation;

	// Squared distance to be used for movement calculations
	float DistanceSquared;

	// Health of the enemy
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Health = 100.0f;

	// Damage value inflicted by the enemy
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DamageValue = 5.0f;

public:
	// Function to apply damage to the enemy and destroy it if health is zero or below
	void DealDamage(float DamageAmount);
};