// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;

UCLASS()
class COOPSHOOTER_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

	void StartFire();

	void EndFire();

protected:

	// if UFUNCTION(BlueprintCallable, Category = "Weapon") it is "blueprint public" by default

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Componets")
	USkeletalMeshComponent* MeshComp;

	// we don't need an instance because we don't change the damage type
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UParticleSystem* VulnerableFleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UParticleSystem* TracerEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShake> FireCamShake;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage;

	/* RPM amount of bullets per minute*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire;

private:

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Fire();

	void PlayFireEffects(FVector TraceEnd);
	
	// make global, so only a timer exists at a time
	FTimerHandle TimerHandle_AutomaticFire;

	float LastFireTime;

	// derived from RateOfFire in seconds
	float TimeBetweenShots;
};
