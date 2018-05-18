// Fill out your copyright notice in the Description page of Project Settings.

#include "SProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
ASProjectile::ASProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->InitSphereRadius(10.0f);
	RootComponent = SphereComp;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = SphereComp;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	LaunchSpeed = 2000.0f;
	FuzeTime = 1.0f;
	DamageRadius = 75.0f;

	ProjectileMovement->InitialSpeed = LaunchSpeed;
}

// Called when the game starts or when spawned
void ASProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASProjectile::OnExplode, FuzeTime, false);
}

void ASProjectile::OnExplode()
{
	if(ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	}

	UGameplayStatics::ApplyRadialDamage(GetWorld(), 20.0f, GetActorLocation(), DamageRadius, DamageType, TArray<AActor*>(), this);
	DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 20, FColor::Red, false, 1.0f);
	Destroy();
}

// Called every frame
void ASProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

