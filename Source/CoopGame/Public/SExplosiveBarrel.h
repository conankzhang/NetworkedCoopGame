// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SExplosiveBarrel.generated.h"

class USHealthComponent;
class UStaticMeshComponent;
class UParticleSystem;
class URadialForceComponent;
class UMaterial;
	
UCLASS()
class COOPGAME_API ASExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASExplosiveBarrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	URadialForceComponent* RadialForceComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ExplodeBarrel")
	UParticleSystem* ExplodeEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ExplodeBarrel")
	float BarrelLaunchImpulse;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ExplodeBarrel")
	UMaterial* ExplodeMaterial;


	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser );

	UPROPERTY(ReplicatedUsing=OnRep_Exploded)
	bool bExploded;

	UFUNCTION()
	void OnRep_Exploded();

	void Explode();
};
