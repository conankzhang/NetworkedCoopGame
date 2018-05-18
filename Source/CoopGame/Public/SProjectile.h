// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SProjectile.generated.h"

class USphereComponent;
class UParticleSystem;
class UProjectileMovementComponent;
class UDamageType;

UCLASS()
class COOPGAME_API ASProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	float FuzeTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	float DamageRadius;

	FTimerHandle TimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UFUNCTION()
	void OnExplode();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
