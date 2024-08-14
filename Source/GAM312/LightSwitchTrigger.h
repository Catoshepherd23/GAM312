// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LightSwitchTrigger.generated.h"

UCLASS()
class GAM312_API ALightSwitchTrigger : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ALightSwitchTrigger();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Declaring point light component
    UPROPERTY(VisibleAnywhere, Category = "Light Switch")
    class UPointLightComponent* PointLight;

    // Declaring sphere component
    UPROPERTY(VisibleAnywhere, Category = "Light Switch")
    class USphereComponent* LightSphere;

    // The desired intensity for the light
    UPROPERTY(VisibleAnywhere, Category = "Light Switch")
    float LightIntensity;

    // Declaring overlap begin function
    UFUNCTION()
    void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    // Declaring overlap end function
    UFUNCTION()
    void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    // Declaring Toggle Light function
    UFUNCTION()
    void ToggleLight();

    
};
