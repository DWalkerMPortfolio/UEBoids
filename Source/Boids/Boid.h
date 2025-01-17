// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Boid.generated.h"

UCLASS()
class BOIDS_API ABoid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere); float NeighborhoodRadius;
	UPROPERTY(EditAnywhere); float NearRadius;
	UPROPERTY(EditAnywhere); float VelocityEasing;
	UPROPERTY(EditAnywhere); float Speed;
	UPROPERTY(EditAnywhere); float AttractorPushRadius;
	UPROPERTY(EditAnywhere); float AttractorPullStrength;
	UPROPERTY(EditAnywhere); float AttractorPushStrength;
	UPROPERTY(EditAnywhere); float FlockCenteringStrength;
	UPROPERTY(EditAnywhere); float CollisionAvoidanceStrength;
	UPROPERTY(EditAnywhere); float VelocityMatchingStrength;

	UPROPERTY(EditAnywhere); UProjectileMovementComponent* ProjectileMovementComponent;

private:
	// All boids in the current level
	TArray<AActor*> Boids;
	// The attractor all boids are drawn towards
	AActor* Attractor;
};
