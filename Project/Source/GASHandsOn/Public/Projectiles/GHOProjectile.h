// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "GHOProjectile.generated.h"

UCLASS()
class GASHANDSON_API AGHOProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGHOProjectile();

public:
	void SetRange(float inRange) { Range = inRange; }
	void SetDamageEffectSpecHandle(const FGameplayEffectSpecHandle& inDamageEffectSpecHandle) { DamageEffectSpecHandle = inDamageEffectSpecHandle; }
	void SetAdditionalEffectSpecHandles(const TArray<FGameplayEffectSpecHandle>& inAdditionalEffectSpecHandles) { AdditionalEffectSpecHandles = inAdditionalEffectSpecHandles; }

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ExposeOnSpawn = true))
	float Range;

	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	TArray<FGameplayEffectSpecHandle> AdditionalEffectSpecHandles;
};
