// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASWeapon;
class USHealthComponent;

UCLASS()
class COOPSHOOTER_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void BeginCrouch();

	void EndCrouch();

	// create in C++ once, and then only edit the properties
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	// cannot even change transform without UPROPERTY
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;

	bool bWantsToZoom;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.2f, ClampMax = 100))
	float ZoomInterpSpeed;

	// set during BeginPlay 
	float DefaultFOV;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComp;

	void BeginZoom();

	void EndZoom();

	ASWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ASWeapon> StarterWeaponClass;

	// enforce designer to use default name
	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName;

	void BeginFire();

	void EndFire();

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthComponent, float Health, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	/* Character died previously */
	UPROPERTY(BlueprintReadOnly, Category = "Player")
	bool bDied;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// for firing purposes
	virtual FVector GetPawnViewLocation() const override;
	
};
