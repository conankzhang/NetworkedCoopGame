// Fill out your copyright notice in the Description page of Project Settings.

#include "SLauncher.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "SProjectile.h"
#include "Kismet/GameplayStatics.h"

void ASLauncher::Fire()
{
	AActor* MyOwner = GetOwner();

	if(MyOwner && Projectile)
	{
		FVector EyeLocation;
		FRotator EyeRotation;

		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		GetWorld()->SpawnActor<ASProjectile>(Projectile, MuzzleLocation, EyeRotation, SpawnParams);
	}
}
