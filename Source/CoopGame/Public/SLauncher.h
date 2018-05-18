// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SLauncher.generated.h"


/**
 * 
 */
UCLASS()
class COOPGAME_API ASLauncher : public ASWeapon
{
	GENERATED_BODY()

class ASProjectile;
protected:
	UFUNCTION(BlueprintCallable, Category = "Launcher")
	virtual void Fire() override;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Launcher")
	ASProjectile* Projectile;
};
