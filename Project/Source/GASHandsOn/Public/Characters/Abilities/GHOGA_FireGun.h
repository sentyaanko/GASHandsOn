// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "Characters/Abilities/GHOGameplayAbility.h"
#include "Projectiles/GHOProjectile.h"
#include "GHOGA_FireGun.generated.h"

/**
 * 
 */
UCLASS()
class GASHANDSON_API UGHOGA_FireGun : public UGHOGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGHOGA_FireGun();

	// UGameplayAbility interface
public:
	/*
	by Epic
		Actually activate ability, do not call this directly
	和訳
		実際にアビリティを起動するときはこれを直接呼ばないでください。
	by GASDocumentation
		We'll call it from APAHeroCharacter::ActivateAbilitiesWithTags().
	和訳
		APAHeroCharacter::ActivateAbilitiesWithTags() から呼び出すことにしましょう。
	*/
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// End of UGameplayAbility interface

private:
	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* FireHipMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* FireIronsightsMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<AGHOProjectile> ProjectileClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Range;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Damage;

};
