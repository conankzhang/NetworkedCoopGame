// Fill out your copyright notice in the Description page of Project Settings.

#include "STrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "SHealthComponent.h"
#include "SCharacter.h"
#include "Sound/SoundCue.h"

static int32 DebugTrackerBot = 0;
FAutoConsoleVariableRef CVARDebugTrackerBot(
	TEXT("COOP.DebugTrackerBot"), 
	DebugTrackerBot, 
	TEXT("Draw Debug for TrackerBots"), 
	ECVF_Cheat);

// Sets default values
ASTrackerBot::ASTrackerBot()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleTakeDamage);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(200);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(RootComponent);

	bUseVelocityChange = false;
	MovementForce = 1000;
	RequiredDistanceToTarget = 100;

	ExplosionDamage = 60;
	ExplosionRadius = 350;

	SelfDamageInterval = 0.25f;

	BuffCheckInterval = 1.0f;
	BuffCheckRadius = 400;

	MaxPowerLevel = 5;
	DamageBoost = 10;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority)
	{
		NextPathPoint = GetNextPathPoint();
		GetWorldTimerManager().SetTimer(TimerHandle_BuffCheck, this, &ASTrackerBot::BuffCheck, BuffCheckInterval, true, 0.0f);
	}
}

void ASTrackerBot::HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{

	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateDynamicMaterialInstance(0, MeshComp->GetMaterial(0));
	}

	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	if (Health <= 0.0f)
	{
		SelfDestruct();
	}
}

FVector ASTrackerBot::GetNextPathPoint()
{
	AActor* BestTarget = nullptr;
	float NearestTargetDistance = FLT_MAX;

	for(FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* TestPawn = It->Get();
		if(TestPawn == nullptr || USHealthComponent::IsFriendly(TestPawn, this))
		{
			continue;
		}

		USHealthComponent* TestPawnHealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if(TestPawnHealthComp && TestPawnHealthComp->GetHealth() > 0.0f)
		{
			float Distance = (TestPawn->GetActorLocation() - GetActorLocation()).Size();
			if(Distance < NearestTargetDistance)
			{
				BestTarget = TestPawn;
				NearestTargetDistance = Distance;
			}
		}
	}

	if(BestTarget)
	{
		UNavigationPath* NavPath = UNavigationSystem::FindPathToActorSynchronously(this, GetActorLocation(), BestTarget);

		GetWorldTimerManager().ClearTimer(TimerHandle_RefreshPath);
		GetWorldTimerManager().SetTimer(TimerHandle_RefreshPath, this, &ASTrackerBot::RefreshPath, 5.0f, false);

		if (NavPath && NavPath->PathPoints.Num() > 1)
		{
			return NavPath->PathPoints[1];
		}
	}

	return GetActorLocation();
}

void ASTrackerBot::SelfDestruct()
{
	if (bExploded)
	{
		return;
	}

	bExploded = true;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	UGameplayStatics::SpawnSoundAtLocation(this, ExplodeSound, GetActorLocation());

	MeshComp->SetVisibility(false, true);
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (Role == ROLE_Authority)
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);

		UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage + (static_cast<float>(PowerLevel) * DamageBoost), GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

		if(DebugTrackerBot > 0)
		{
			DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 2.0f, 0, 1.0f);
		}

		SetLifeSpan(2.0f);
	}
}

void ASTrackerBot::BuffCheck()
{
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	TArray<AActor*> OutActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	if(DebugTrackerBot > 0)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), BuffCheckRadius, 20, FColor::Blue, false, 1.0f, 0, 1.0f);
	}

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), BuffCheckRadius, ObjectTypes, GetClass(), IgnoreActors, OutActors);

	PowerLevel = FMath::Clamp(OutActors.Num(), 0, MaxPowerLevel);
	
	UE_LOG(LogTemp, Log, TEXT("Name: %s, PowerLevel: %s"), *GetName(), *FString::FromInt(PowerLevel));
	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateDynamicMaterialInstance(0, MeshComp->GetMaterial(0));
	}

	if (MatInst)
	{
		float Alpha = PowerLevel / static_cast<float>(MaxPowerLevel);
		MatInst->SetScalarParameterValue("PowerLevelAlpha", Alpha);
	}
}

void ASTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

void ASTrackerBot::RefreshPath()
{
	NextPathPoint = GetNextPathPoint();
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_Authority && !bExploded)
	{
		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

		if (DistanceToTarget <= RequiredDistanceToTarget)
		{
			NextPathPoint = GetNextPathPoint();

			if(DebugTrackerBot > 0)
			{
				DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached");
			}
		}
		else
		{
			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();
			ForceDirection *= MovementForce;
			MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);

			if(DebugTrackerBot > 0)
			{
				DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Yellow, false, 0.0f, 0, 1.0f);
			}
		}

		if(DebugTrackerBot > 0)
		{
			DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 0.0f, 1.0f);
		}
	}
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (!bStartedSelfDestruction && !bExploded)
	{
		ASCharacter* PlayerPawn = Cast<ASCharacter>(OtherActor);
		if (PlayerPawn && !USHealthComponent::IsFriendly(OtherActor, this))
		{
			if(Role == ROLE_Authority)
			{
				GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASTrackerBot::DamageSelf, SelfDamageInterval, true, 0.0f);
			}

			bStartedSelfDestruction = true;

			UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent);
		}
	}
}
