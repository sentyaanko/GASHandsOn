// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GHOPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class GASHANDSON_API AGHOPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGHOPlayerState();
	
	// AActor interface
protected:
	/** Overridable native event for when play begins for this actor. */
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// End of AActor interface

	// IAbilitySystemInterface interface
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// End of IAbilitySystemInterface interface

public:
	class UGHOAttributeSetBase* GetAttributeSetBase()const;

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOPlayerState|UI")
	void ShowAbilityConfirmCancelText(bool bShowText);

private:
	// Attribute changed callbacks
	void HealthChanged(const struct FOnAttributeChangeData& Data);
	void HealthMaxChanged(const struct FOnAttributeChangeData& Data);
	void HealthRegenRateChanged(const struct FOnAttributeChangeData& Data);
	void ManaChanged(const struct FOnAttributeChangeData& Data);
	void ManaMaxChanged(const struct FOnAttributeChangeData& Data);
	void ManaRegenRateChanged(const struct FOnAttributeChangeData& Data);
	void StaminaChanged(const struct FOnAttributeChangeData& Data);
	void StaminaMaxChanged(const struct FOnAttributeChangeData& Data);
	void StaminaRegenRateChanged(const struct FOnAttributeChangeData& Data);

	// Tag change callbacks
	void StunTagChanged(const struct FGameplayTag CallbackTag, int32 NewCount);

private:
	UPROPERTY()
	class UGHOAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UGHOAttributeSetBase* AttributeSetBase;

	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle HealthMaxChangedDelegateHandle;
	FDelegateHandle HealthRegenRateChangedDelegateHandle;
	FDelegateHandle ManaChangedDelegateHandle;
	FDelegateHandle ManaMaxChangedDelegateHandle;
	FDelegateHandle ManaRegenRateChangedDelegateHandle;
	FDelegateHandle StaminaChangedDelegateHandle;
	FDelegateHandle StaminaMaxChangedDelegateHandle;
	FDelegateHandle StaminaRegenRateChangedDelegateHandle;

};
