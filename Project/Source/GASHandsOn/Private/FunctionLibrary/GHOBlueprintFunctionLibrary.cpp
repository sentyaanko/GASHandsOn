// (C) Sentya Anko 2021


#include "FunctionLibrary/GHOBlueprintFunctionLibrary.h"
#include "AbilitySystemComponent.h"
#include "Characters/Types/GHOGameplayEffectContext.h"

void UGHOBlueprintFunctionLibrary::ClearTargetData(FGameplayAbilityTargetDataHandle& TargetData)
{
	TargetData.Clear();
}

bool UGHOBlueprintFunctionLibrary::IsAbilitySpecHandleValid(FGameplayAbilitySpecHandle Handle)
{
	return Handle.IsValid();
}

UGHOGameplayAbility* UGHOBlueprintFunctionLibrary::GetPrimaryAbilityInstanceFromHandle(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle)
{
	if (AbilitySystemComponent)
	{
		if (FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(Handle))
		{
			return Cast<UGHOGameplayAbility>(AbilitySpec->GetPrimaryInstance());
		}
	}

	return nullptr;
}

UGHOGameplayAbility* UGHOBlueprintFunctionLibrary::GetPrimaryAbilityInstanceFromClass(UAbilitySystemComponent* AbilitySystemComponent, TSubclassOf<UGameplayAbility> InAbilityClass)
{
	if (AbilitySystemComponent)
	{
		if (FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(InAbilityClass))
		{
			return Cast<UGHOGameplayAbility>(AbilitySpec->GetPrimaryInstance());
		}
	}

	return nullptr;
}

bool UGHOBlueprintFunctionLibrary::IsPrimaryAbilityInstanceActive(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle)
{
	if (AbilitySystemComponent)
	{
		if (FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(Handle))
		{
			//return Cast<UGHOGameplayAbility>(AbilitySpec->GetPrimaryInstance())->IsActive();
			return AbilitySpec->GetPrimaryInstance()->IsActive();
		}
	}

	return false;
}

void UGHOBlueprintFunctionLibrary::AddTargetsToEffectContainerSpec(FGHOGameplayEffectContainerSpec& ContainerSpec, const TArray<FGameplayAbilityTargetDataHandle>& TargetData, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors)
{
	ContainerSpec.AddTargets(TargetData, HitResults, TargetActors);
}

FGameplayAbilityTargetDataHandle UGHOBlueprintFunctionLibrary::EffectContextGetTargetData(FGameplayEffectContextHandle EffectContextHandle)
{
	if (FGHOGameplayEffectContext* EffectContext = static_cast<FGHOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return EffectContext->GetTargetData();
	}

	return FGameplayAbilityTargetDataHandle();
}

void UGHOBlueprintFunctionLibrary::EffectContextAddTargetData(FGameplayEffectContextHandle EffectContextHandle, const FGameplayAbilityTargetDataHandle& TargetData)
{
	if (FGHOGameplayEffectContext* EffectContext = static_cast<FGHOGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		EffectContext->AddTargetData(TargetData);
	}
}

