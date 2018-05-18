// Fill out your copyright notice in the Description page of Project Settings.

#include "SLauncher.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "SProjectile.h"

void ASLauncher::Fire()
{
	FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
	FRotator MuzzleRotation = MeshComp->GetSocketRotation(MuzzleSocketName);
	FActorSpawnParameters SpawnInfo;
	GetWorld()->SpawnActor<ASProjectile>(Projectile, MuzzleLocation, MuzzleRotation, SpawnInfo);
}
