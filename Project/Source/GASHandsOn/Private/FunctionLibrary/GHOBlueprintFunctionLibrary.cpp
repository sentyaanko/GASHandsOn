// (C) Sentya Anko 2021


#include "FunctionLibrary/GHOBlueprintFunctionLibrary.h"
#include "AbilitySystemComponent.h"
#include "Characters/Types/GHOGameplayEffectContext.h"
#include "Characters/Types/GHOTargetData_Int32.h"
#include "Characters/Types/GHOTargetData_Name.h"
#include "Characters/Components/GHOAbilitySystemComponent.h"

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

UGHOGameplayAbility* UGHOBlueprintFunctionLibrary::GetPrimaryAbilityInstanceFromClass(UAbilitySystemComponent* AbilitySystemComponent, TSubclassOf<UGameplayAbility> InAbilityClass, bool bAllowInheritedType)
{
	if (UGHOAbilitySystemComponent* ASC = Cast<UGHOAbilitySystemComponent>(AbilitySystemComponent))
	{
		if (FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromClassWithAllowInheritedTypeFlag(InAbilityClass, bAllowInheritedType))
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

TArray<FActiveGameplayEffectHandle> UGHOBlueprintFunctionLibrary::ApplyExternalEffectContainerSpec(const FGHOGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;

	// Iterate list of gameplay effects
	for (const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		if (SpecHandle.IsValid())
		{
			// If effect is valid, iterate list of targets and apply to all
			for (TSharedPtr<FGameplayAbilityTargetData> Data : ContainerSpec.TargetData.Data)
			{
				AllEffects.Append(Data->ApplyGameplayEffectSpec(*SpecHandle.Data.Get()));
			}
		}
	}
	return AllEffects;
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

FGameplayAbilityTargetDataHandle UGHOBlueprintFunctionLibrary::AbilityTargetDataFromInt32(int32 Value)
{
	// Construct TargetData
	FGameplayAbilityTargetData_Int32*	NewData = new FGameplayAbilityTargetData_Int32();
	NewData->Value = Value;

	// Give it a handle and return
	FGameplayAbilityTargetDataHandle	Handle;
	Handle.Data.Add(TSharedPtr<FGameplayAbilityTargetData_Int32>(NewData));
	return Handle;
}

FGameplayAbilityTargetDataHandle UGHOBlueprintFunctionLibrary::AbilityTargetDataFromName(FName Value)
{
	// Construct TargetData
	FGameplayAbilityTargetData_Name*	NewData = new FGameplayAbilityTargetData_Name();
	NewData->Value = Value;

	// Give it a handle and return
	FGameplayAbilityTargetDataHandle	Handle;
	Handle.Data.Add(TSharedPtr<FGameplayAbilityTargetData_Name>(NewData));
	return Handle;
}

int32 UGHOBlueprintFunctionLibrary::GetInt32FromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int32 Index)
{
	if (TargetData.Data.IsValidIndex(Index))
	{
		if (auto Data = TargetData.Data[Index].Get())
		{
			if (Data->GetScriptStruct()->IsChildOf(FGameplayAbilityTargetData_Int32::StaticStruct()))
			{
				auto p = static_cast<FGameplayAbilityTargetData_Int32*>(Data);
				return p->Value;
			}
		}
	}

	return 0;
}

FName UGHOBlueprintFunctionLibrary::GetNameFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int32 Index)
{
	if (TargetData.Data.IsValidIndex(Index))
	{
		if (auto Data = TargetData.Data[Index].Get())
		{
			if (Data->GetScriptStruct()->IsChildOf(FGameplayAbilityTargetData_Name::StaticStruct()))
			{
				auto p = static_cast<FGameplayAbilityTargetData_Name*>(Data);
				return p->Value;
			}
		}
	}

	return FName();
}

