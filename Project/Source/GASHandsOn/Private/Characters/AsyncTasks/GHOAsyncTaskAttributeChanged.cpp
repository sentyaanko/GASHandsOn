// (C) Sentya Anko 2021


#include "Characters/Abilities/GHOAsyncTaskAttributeChanged.h"

/*
解説
	GASDocumentation と作りが異なるので参考にする際は注意が必要です。
*/


UGHOAsyncTaskAttributeChanged* UGHOAsyncTaskAttributeChanged::ListenForAttributesChange(UAbilitySystemComponent* InAbilitySystemComponent, TArray<FGHOAttributeChangedParam> InAttributeChangedParams)
{
	if (!IsValid(InAbilitySystemComponent) || InAttributeChangedParams.Num() < 1 )
	{
		return nullptr;
	}
	UGHOAsyncTaskAttributeChanged* WaitForAttributeChangedTask = NewObject<UGHOAsyncTaskAttributeChanged>();
	WaitForAttributeChangedTask->Initialize(InAbilitySystemComponent, InAttributeChangedParams);
	return WaitForAttributeChangedTask;
}

void UGHOAsyncTaskAttributeChanged::Initialize(UAbilitySystemComponent* InAbilitySystemComponent, TArray<FGHOAttributeChangedParam> InAttributeChangedParams)
{
	AbilitySystemComponent = InAbilitySystemComponent;
	AttributeChangedParams = InAttributeChangedParams;

	for (int32 i = 0; i < AttributeChangedParams.Num(); ++i )
	{
		auto Delegate = AttributeChangedParams[i].OnAttributeChanged;
		if (Delegate.IsBound())
		{
			AttributeChangedParams[i].Handle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeChangedParams[i].Attribute).AddLambda([Delegate](const FOnAttributeChangeData& Data) {Delegate.ExecuteIfBound(Data.NewValue, Data.OldValue); });
		}
	}
}

void UGHOAsyncTaskAttributeChanged::EndTask()
{
	if (IsValid(AbilitySystemComponent))
	{
		for (int32 i = 0; i < AttributeChangedParams.Num(); ++i)
		{
			if (AttributeChangedParams[i].Handle.IsValid())
			{
				AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeChangedParams[i].Attribute).Remove(AttributeChangedParams[i].Handle);
				AttributeChangedParams[i].Handle.Reset();
			}
		}
		AttributeChangedParams.Reset();
		AbilitySystemComponent = nullptr;
	}

	SetReadyToDestroy();
	MarkPendingKill();
}
