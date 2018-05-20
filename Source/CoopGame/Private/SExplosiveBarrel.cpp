// Fill out your copyright notice in the Description page of Project Settings.

#include "SExplosiveBarrel.h"
#include "Kismet/GameplayStatics.h"
#include "SHealthComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"


// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->bAutoActivate = false;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bIgnoreOwningActor = true;
	RadialForceComp->SetupAttachment(RootComponent);

	BarrelLaunchImpulse = FVector(0.0f, 100.0f, 0.0f);
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
	HealthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveBarrel::OnHealthChanged);
}

void ASExplosiveBarrel::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if(Health <= 0.0f && !bExploded)
	{
		bExploded = true;
		Explode();
	}
}

void ASExplosiveBarrel::Explode()
{
	if(ExplodeEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeEffect, GetActorLocation());
	}

	if(ExplodeMaterial)
	{
		MeshComp->SetMaterial(0, ExplodeMaterial);
	}

	MeshComp->AddImpulse(BarrelLaunchImpulse);
	RadialForceComp->FireImpulse();
}