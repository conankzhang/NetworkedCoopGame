// Fill out your copyright notice in the Description page of Project Settings.

#include "SLauncher.h"




void ASLauncher::Fire()
{
	FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
	FRotator MuzzleRotation = MeshComp->GetSocketRotation(MuzzleSocketName);
	//GetWorld()->SpawnActor(Projectile, MuzzleLocation, MuzzleRotation);

}
