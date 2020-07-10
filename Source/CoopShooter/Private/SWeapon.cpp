// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SceneComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CoopShooter/CoopShooter.h"

int32 DebugWeaponDrawing = 0;
// ECVF_Cheat is disabled for release build
FAutoConsoleVariableRef CVARDebugTrackBulletDrawing(TEXT("COOP.DebugWeapons"), DebugWeaponDrawing, TEXT("Draw Debug Lines from eye for Weapons"), ECVF_Cheat);


// Sets default values
ASWeapon::ASWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleFlashSocket";
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
		QueryParams.bReturnPhysicalMaterial = true;

		// to know what we hit
		FHitResult Hit;
		
		// particle "BeamEnd" parameter
		FVector TrackBulletEndPoint = TraceEnd;

		// anything that is visible will block our trace and returns Hit.
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
		{
			// Blocking hit! Process damage
			UE_LOG(LogTemp, Log, TEXT("Blocking hit!"));
			AActor* HitActor = Hit.GetActor();

		//	FPointDamageEvent pointDamageEvent(0.0f, Hit, ShootDirection, DamageType);

			//HitActor->TakeDamage(20.0f, pointDamageEvent, MyOwner->GetInstigatorController(), this);
			// Instigator is WHO caused the damage. Weapon is damage causer
			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShootDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);
		
			// TWeakObject point 
			UPhysicalMaterial* HitPhysMat = Hit.PhysMaterial.Get();
			UE_LOG(LogTemp, Log, TEXT("HitPhysMat: %s"), *(HitPhysMat->GetFullName()));

			EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysMat);
			
			UParticleSystem* ImpactEffect = nullptr;

			const UEnum* WeaponStateEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPhysicalSurface"));

			switch (HitSurfaceType)
			{
			case SurfaceType1:
				UE_LOG(LogTemp, Log, TEXT("SurfaceType1: %s"), *(WeaponStateEnum ? WeaponStateEnum->GetEnumName(HitSurfaceType) : TEXT("<Invalid Enum>")));
				ImpactEffect = FleshImpactEffect;
				break;
			case SurfaceType2:

				UE_LOG(LogTemp, Log, TEXT("SurfaceType2: %s"), *(WeaponStateEnum ? WeaponStateEnum->GetEnumName(HitSurfaceType) : TEXT("<Invalid Enum>")));
				ImpactEffect = VulnerableFleshImpactEffect;
				break;
			default:
				UE_LOG(LogTemp, Error, TEXT("SurfaceType0: %s"), *(WeaponStateEnum ? WeaponStateEnum->GetEnumName(HitSurfaceType) : TEXT("<Invalid Enum>")));
				ImpactEffect = DefaultImpactEffect;
				break;
			}

			if (ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}

			TrackBulletEndPoint = Hit.ImpactPoint;
		}

		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Black, false, 1.0f, 0, 1.0f);
		}


		PlayFireEffects(TrackBulletEndPoint);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot fire because the Owner of SWeapon is nullptr. Please, use `CurWeapon->SetOwner(MyPawn)` when spawning a weapon."));
	}


}

void ASWeapon::PlayFireEffects(FVector TraceEnd)
{

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
			TracerComp->SetVectorParameter("BeamEnd", TraceEnd);
		}
	}

	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(FireCamShake);
		}
	}
}
