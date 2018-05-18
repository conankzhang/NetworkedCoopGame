// Fill out your copyright notice in the Description page of Project Settings.

#include "SLauncher.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "SProjectile.h"
#include "Kismet/GameplayStatics.h"

void ASLauncher::Fire()
{
	FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
	FRotator MuzzleRotation = MeshComp->GetSocketRotation(MuzzleSocketName);

	MuzzleRotation.Pitch = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation().Pitch;
	FActorSpawnParameters SpawnInfo;
	
	if(Projectile)
	{
		ASProjectile* Munition = GetWorld()->SpawnActor<ASProjectile>(Projectile, MuzzleLocation, MuzzleRotation, SpawnInfo);
	}
}
