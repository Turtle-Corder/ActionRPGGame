// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ActionRPGGame.h"

#include "../Effects/AREffectPeriodic.h"
#include "../Effects/AREffectPeriodicO.h"
#include "../Componenets/ARAttributeComponent.h"
#include "../Types/AREffectTypes.h"
#include "ARTraceStatics.h"

#include "../ARProjectile.h"
#include "AREffectStatics.h"

UAREffectStatics::UAREffectStatics(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

FEffectSpec UAREffectStatics::CreatePeriodicEffect(AActor* EffectTarget, AActor* EffectCauser, float Duration, TSubclassOf<class AAREffectPeriodic> EffectType)
{
	FEffectSpec PeriodicEffect;
	if (!EffectTarget && !EffectCauser)
		return PeriodicEffect;
	UARAttributeComponent* attrComp = EffectTarget->FindComponentByClass<UARAttributeComponent>();

	if (attrComp->ActiveEffects.Effects.Num() > 0)
	{
		for (FEffectSpec& effect : attrComp->ActiveEffects.Effects)
		{
			if (effect.ActorEffect && effect.ActorEffect->GetClass() == EffectType)
			{
				//reality. Do check if effect is stackable or something.
				return PeriodicEffect;
			}
		}
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.bNoCollisionFail = true;
	SpawnInfo.Owner = EffectTarget;
	//SpawnInfo.Instigator = EffectCauser;

	AAREffectPeriodic* effecTemp = EffectTarget->GetWorld()->SpawnActor<AAREffectPeriodic>(EffectType, SpawnInfo);
	effecTemp->EffectCauser = EffectCauser;
	effecTemp->EffectTarget = EffectTarget;
	effecTemp->MaxDuration = Duration;
	PeriodicEffect.ActorEffect = effecTemp;
	PeriodicEffect.MaxDuration = Duration;
	PeriodicEffect.CurrentDuration = 0;

	attrComp->AddPeriodicEffect(PeriodicEffect);

	return PeriodicEffect;
}

void UAREffectStatics::ActivatePeriodicEffect(FPeriodicEffect PeriodicEffect)
{
	//PeriodicEffect.PeriodicEffect->Initialze();
	UARAttributeComponent* attrComp = PeriodicEffect.PeriodicEffect->GetOwner()->FindComponentByClass<UARAttributeComponent>();
	PeriodicEffect.IsEffectActive = true;

	//attrComp->AddPeriodicEffect(PeriodicEffect);
}

void UAREffectStatics::ApplyPeriodicEffect(AActor* EffectTarget, AActor* EffectCauser, float Duration, TSubclassOf<class UAREffectPeriodicO> EffectType)
{
	//if (!EffectCauser && !EffectTarget)
	//	return;

	//UARAttributeBaseComponent* attrTarget = EffectTarget->FindComponentByClass<UARAttributeBaseComponent>();

	//UAREffectPeriodicO* effect = ConstructObject<UAREffectPeriodicO>(EffectType);

	//effect->EffectCausedBy = EffectCauser;
	//effect->EffectTarget = EffectTarget;
	//effect->MaxDuration = Duration;
	//FEffectSpec Spec;
	//Spec.Effect = effect;
	//Spec.MaxDuration = Duration;
	//Spec.CurrentDuration = 0;
	////attrTarget->ActiveEffects.Effects.Add(Spec);
	//attrTarget->ApplyPeriodicEffect(Spec);
}

void UAREffectStatics::ChangeAttribute(AActor* Target, AActor* CausedBy, float ModVal, FName AttributeName, TEnumAsByte<EAttrOp> OpType)
{
	if (!Target && !CausedBy)
		return;

	TWeakObjectPtr<UARAttributeBaseComponent> attrComp = Target->FindComponentByClass<UARAttributeBaseComponent>();

	if (!attrComp.IsValid())
		return;
	attrComp->ChangeAttribute(AttributeName, ModVal, OpType);
}

void UAREffectStatics::ApplyDamage(AActor* DamageTarget, float BaseDamage, FName AttributeName, AActor* EventInstigator, AActor* DamageCauser, FGameplayTagContainer DamageTag, TSubclassOf<class UDamageType> DamageType)
{
	if (!DamageTarget)
		return;

	TWeakObjectPtr<UARAttributeBaseComponent> attr = DamageTarget->FindComponentByClass<UARAttributeBaseComponent>();

	FAttribute Attribute;
	Attribute.AttributeName = AttributeName;
	Attribute.ModValue = BaseDamage;
	Attribute.OperationType = EAttrOp::Attr_Subtract;

	FARDamageEvent DamageEvent;
	DamageEvent.Attribute = Attribute;
	DamageEvent.DamageTypeClass = DamageType;
	DamageEvent.DamageTag = DamageTag;
	attr->DamageAttribute(DamageEvent, EventInstigator, DamageCauser);
}

void UAREffectStatics::ApplyPointDamage(AActor* DamageTarget, float AttributeMod, FName AttributeName, const FVector& HitFromLocation, const FHitResult& HitInfo, AActor* EventInstigator, AActor* Causer, TSubclassOf<class UDamageType> DamageType)
{
	if (!DamageTarget)
		return;

	TWeakObjectPtr<UARAttributeBaseComponent> attrComp = DamageTarget->FindComponentByClass<UARAttributeBaseComponent>();

	if (!attrComp.IsValid())
		return;

	FAttribute Attribute;
	Attribute.AttributeName = AttributeName;
	Attribute.ModValue = AttributeMod;
	Attribute.OperationType = EAttrOp::Attr_Subtract;

	//FPointAttributeChangeEvent AttributeEvent(Attribute, HitInfo, HitFromLocation, DamageType);
	FARPointDamageEvent AttributeEvent;
	AttributeEvent.Attribute = Attribute;
	AttributeEvent.HitInfo = HitInfo;
	AttributeEvent.ShotDirection = HitFromLocation;
	AttributeEvent.DamageTypeClass = DamageType;
	attrComp->DamageAttribute(AttributeEvent, EventInstigator, Causer);
}

void UAREffectStatics::ApplyRadialDamageWithFalloff(UObject* WorldContextObject, float BaseDamage, float MinimumDamage, const FVector& Origin, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, AActor* DamageCauser, AActor* Instigator)
{

}

void UAREffectStatics::ApplyRadialDamage(UObject* WorldContextObject, float BaseDamage, const FVector& Origin, float DamageRadius, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, AActor* DamageCauser, AActor* Instigator, bool bDoFullDamage)
{

}

void UAREffectStatics::ShootProjectile(TSubclassOf<class AARProjectile> Projectile, FVector Origin, FVector ShootDir, AActor* Causer, FName StartSocket, FHitResult HitResult)
{
	APawn* pawn = Cast<APawn>(Causer);
	if (!pawn)
		return;

	if (HitResult.bBlockingHit)
	{
		const FVector dir = (HitResult.ImpactPoint - Origin).SafeNormal();
		FTransform SpawnTM(FRotator(0,0,0), Origin);

		AARProjectile* proj = Cast<AARProjectile>(UGameplayStatics::BeginSpawningActorFromClass(Causer, Projectile, SpawnTM));


		if (proj)
		{
			//proj->Instigator = Causer;
			proj->SetOwner(Causer);
			proj->Movement->Velocity = dir * proj->Movement->InitialSpeed;
			UGameplayStatics::FinishSpawningActor(proj, SpawnTM);
		}
	}

}

void UAREffectStatics::SpawnProjectileInArea(TSubclassOf<class AARProjectile> Projectile, AActor* Causer, APawn* Instigator, FHitResult HitResult, float InitialVelocity, float MaxRadius, float MaxHeight, float ImpactDirection, int32 Amount)
{
	if (HitResult.bBlockingHit)
	{
		for (int32 CurAmount = 0; CurAmount < Amount; CurAmount++)
		{
			FVector Location = HitResult.ImpactPoint;
			Location.Z += MaxHeight;
			Location.Y += FMath::RandRange(-MaxRadius, MaxRadius);
			Location.X += FMath::RandRange(-MaxRadius, MaxRadius);
			FTransform SpawnTM(FRotator(0, 0, 0), Location);

			AARProjectile* proj = Cast<AARProjectile>(UGameplayStatics::BeginSpawningActorFromClass(Causer, Projectile, SpawnTM));

			FVector FallDirection = FVector(FMath::FRandRange(-ImpactDirection, ImpactDirection), FMath::FRandRange(-ImpactDirection, ImpactDirection), -1);

			if (proj)
			{
				//proj->Instigator = Causer;
				proj->SetOwner(Causer);
				proj->Instigator = Instigator;
				proj->Movement->Velocity = FallDirection * InitialVelocity; // proj->Movement->InitialSpeed;
				UGameplayStatics::FinishSpawningActor(proj, SpawnTM);
			}
		}
	}
}