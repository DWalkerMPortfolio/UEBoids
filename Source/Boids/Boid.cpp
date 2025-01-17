// Fill out your copyright notice in the Description page of Project Settings.


#include "Boid.h"
#include <Kismet/GameplayStatics.h>
#include "Logging/StructuredLog.h"

// Sets default values
ABoid::ABoid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize scene root
	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(FName("GSceneRoot"));
	SetRootComponent(SceneComponent);

	// Initialize projectile movement
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(FName("GProjectileMovement"));
	ProjectileMovementComponent->RegisterComponent();
}

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
	Super::BeginPlay();
	
	// Get all other boids
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABoid::StaticClass(), Boids);

	// Find the attractor
	TArray<AActor*> FoundAttractors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("BoidAttractor"), FoundAttractors);
	Attractor = FoundAttractors[0];
}

// Called every frame
void ABoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Get information about all nearby boids
	FVector AvgPos = FVector::Zero(); // Average position of all boids within this boid's neighborhood radius
	FVector AvgVel = FVector::Zero(); // Average velocity of all boids within this boid's neighborhood radius
	FVector AvgNearPos = FVector::Zero(); // Average position of all boids within this boid's collision avoidance radius
	int NeighborCount = 0; // Number of boids within this boid's neighborhood radius
	int NearCount = 0; // Number of boids within this boid's collision avoidance radius
	for (int i = 0; i < Boids.Num(); i++)
	{
		// Skip self
		if (Boids[i] == this)
		{
			continue;
		}

		// Check if boid is within the neighborhood radius
		float Distance = FVector::Distance(GetActorLocation(), Boids[i]->GetActorLocation());
		if (Distance > 0)
		{
			if (Distance < NeighborhoodRadius)
			{
				AvgPos += Boids[i]->GetActorLocation();
				AvgVel += Boids[i]->GetVelocity();
				NeighborCount++;

				// Check if boid is within the near radius
				if (Distance < NearRadius)
				{
					AvgNearPos += Boids[i]->GetActorLocation();
					NearCount++;
				}
			}
		}
	}
	AvgPos /= NeighborCount;
	AvgVel /= NeighborCount;
	AvgNearPos /= NearCount;

	// Apply influences from nearby boids
	FVector SumVel = FVector::Zero(); // The sum of all influences on this boid
	// Apply flock centering
	FVector VelCenter = AvgPos; // Influence pulling the boid towards the center of the flock
	if (VelCenter != FVector::Zero())
	{
		VelCenter -= GetActorLocation();
		VelCenter.Normalize();
		SumVel += VelCenter * FlockCenteringStrength;
	}
	// Apply collision avoidance
	FVector VelAvoid = FVector::Zero();
	if (AvgNearPos != FVector::Zero())
	{
		VelAvoid = GetActorLocation() - AvgNearPos;
		VelAvoid.Normalize();
		SumVel += VelAvoid * CollisionAvoidanceStrength;
	}
	// Apply velocity matching
	FVector VelAlign = AvgVel;
	if (VelAlign != FVector::Zero())
	{
		VelAlign.Normalize();
		SumVel += VelAlign * VelocityMatchingStrength;
	}

	// Apply influence from the attractor
	FVector AttractorDelta = Attractor->GetActorLocation() - GetActorLocation();
	if (AttractorDelta.Length() > AttractorPushRadius)
	{
		SumVel += AttractorDelta.GetSafeNormal() * AttractorPullStrength;
	}
	else
	{
		SumVel -= AttractorDelta.GetSafeNormal() * AttractorPushStrength;
	}

	// Interpolate velocity
	SumVel.Normalize();
	FVector CurrentVel = ProjectileMovementComponent->Velocity;
	CurrentVel.Normalize();
	FVector Vel = FMath::Lerp(CurrentVel, SumVel, VelocityEasing); // The velocity to apply this frame
	ProjectileMovementComponent->Velocity = Vel * Speed;
}

