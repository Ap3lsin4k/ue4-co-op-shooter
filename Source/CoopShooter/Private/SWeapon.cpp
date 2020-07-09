// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SceneComponent.h"
#


// Sets default values
ASWeapon::ASWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleFlashSocket";
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASWeapon::Fire()
{
	//Trace the world, from pawn eyes to crosshair location (center of the screen)
	
	// find out who is the owner of the weapon
	AActor* MyOwner = GetOwner();

	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;

		// Location of the head
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShootDirection = EyeRotation.Vector();

		// End location of the ray is EyeLocation the direction where we looking and a big number. Every weapon has its max shooting length
		FVector TraceEnd = EyeLocation + (ShootDirection * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);

		// trace against each individual triangle (more expensive)
		// we want to be very accurate
		QueryParams.bTraceComplex = true;

		// to know what we hit
		FHitResult Hit;
		
		// particle "BeamEnd" parameter
		FVector TraceEndPoint = TraceEnd;

		// anything that is visible will block our trace and returns Hit.
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility))
		{
			// Blocking hit! Process damage
			UE_LOG(LogTemp, Log, TEXT("Blocking hit!"));
			AActor* HitActor = Hit.GetActor();

			FPointDamageEvent pointDamageEvent(0.0f, Hit, ShootDirection, DamageType);

			HitActor->TakeDamage(20.0f, pointDamageEvent, MyOwner->GetInstigatorController(), this);
			// Instigator is WHO caused the damage. Weapon is damage causer
			//UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShootDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);
			if (ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}

			TraceEndPoint = Hit.ImpactPoint;
		}

		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Black, false, 1.0f, 0, 1.0f);

		if (MuzzleEffect)
		{
			// effect to follow the weapon
			UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
		}

		if (TracerEffect)
		{
			FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

			UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
			if (TracerComp)
			{
				TracerComp->SetVectorParameter("BeamEnd", TraceEndPoint);
			}
		}

	}

}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

