// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cube.generated.h"

UCLASS()
class GAM312_API ACube : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACube();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Material for cube
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UStaticMeshComponent* CubeMesh;

	// Damage material for cube
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UMaterialInstance* CubeMaterial;

	// Function that is called when cube is hit by GAM312Projectile
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UMaterialInstance* DamagedCubeMaterial;

	// Handles timer for cube after being hit
	FTimerHandle DamageTimer;

	// Function that is called when cube is hit by another component
	UFUNCTION()

	void OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	// Function called when cube takes damage
	void OnTakeDamage();
	// Function that resets cube color after being struck
	void ResetDamage();
};
