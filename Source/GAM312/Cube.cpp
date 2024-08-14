// Fill out your copyright notice in the Description page of Project Settings.


#include "Cube.h"
#include "Kismet/GameplayStatics.h"
#include "GAM312Projectile.h"


// Sets default values
ACube::ACube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Creates cubemesh component and sets it to start physics
	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
	DamagedCubeMaterial = CreateDefaultSubobject<UMaterialInstance>(TEXT("DamageMaterial"));
	CubeMaterial = CreateDefaultSubobject < UMaterialInstance>(TEXT("CubeMaterial"));

	CubeMesh->SetSimulatePhysics(true);
}

// Called when the game starts or when spawned
void ACube::BeginPlay()
{
	Super::BeginPlay();
	
	// Gets the OnComponentHit function to handle hits
	CubeMesh->OnComponentHit.AddDynamic(this, &ACube::OnComponentHit);
}

// Function that is called when cube takes damage
void ACube::OnTakeDamage()
{
	CubeMesh->SetMaterial(0, DamagedCubeMaterial);
	GetWorld()->GetTimerManager().SetTimer(DamageTimer, this, &ACube::ResetDamage, 1.5f, false);
}

// Function that resets the material back to blue after a delay
void ACube::ResetDamage()
{
	CubeMesh->SetMaterial(0, CubeMaterial);
}

// Function that is called when the cube is hit by GAM312Projectile
void ACube::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Checks to see if the cube is hit by the GAM312Projectile
	if (AGAM312Projectile* HitActor = Cast<AGAM312Projectile>(OtherActor))
	{
		// This applies damage and triggers the effect
		UGameplayStatics::ApplyDamage(this, 20.0f, nullptr, OtherActor, UDamageType::StaticClass());
		OnTakeDamage();
	}
}

// Called every frame
void ACube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

