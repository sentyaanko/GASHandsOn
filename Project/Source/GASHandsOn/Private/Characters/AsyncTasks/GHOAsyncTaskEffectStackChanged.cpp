// (C) Sentya Anko 2021


#include "Characters/AsyncTasks/GHOAsyncTaskEffectStackChanged.h"

UGHOAsyncTaskEffectStackChanged * UGHOAsyncTaskEffectStackChanged::ListenForGameplayEffectStackChange(UAbilitySystemComponent* InAbilitySystemComponent, FGameplayTag InEffectGameplayTag)
{
	/*
	解説
		GASDocumentation では、まず NewObject() した後に引数チェックを行い、適切でない場合は EndTask() の呼び出しを行っています。
		そうすることに特に意味がないように読み取れたため、ここでは引数チェックを先んじて行うように変更しています。
		また、メンバの初期化に関しても Initialize() 関数で行うように変更しています。
	*/
	if (!IsValid(InAbilitySystemComponent) || !InEffectGameplayTag.IsValid())
	{
		return nullptr;
	}

	UGHOAsyncTaskEffectStackChanged* ListenForGameplayEffectStackChange = NewObject<UGHOAsyncTaskEffectStackChanged>();
	ListenForGameplayEffectStackChange->Initialize(InAbilitySystemComponent, InEffectGameplayTag);
	return ListenForGameplayEffectStackChange;
}

void UGHOAsyncTaskEffectStackChanged::EndTask()
{
	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);
		AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate().RemoveAll(this);
	}

	SetReadyToDestroy();
	MarkPendingKill();
}

void UGHOAsyncTaskEffectStackChanged::Initialize(UAbilitySystemComponent* InAbilitySystemComponent, FGameplayTag InEffectGameplayTag)
{
	AbilitySystemComponent = InAbilitySystemComponent;
	EffectGameplayTag = InEffectGameplayTag;

	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UGHOAsyncTaskEffectStackChanged::OnActiveGameplayEffectAddedCallback);
	AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UGHOAsyncTaskEffectStackChanged::OnRemoveGameplayEffectCallback);
}

void UGHOAsyncTaskEffectStackChanged::OnActiveGameplayEffectAddedCallback(UAbilitySystemComponent * Target, const FGameplayEffectSpec & SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
	//FGameplayTagContainer AssetTags;
	//SpecApplied.GetAllAssetTags(AssetTags);

	//FGameplayTagContainer GrantedTags;
	//SpecApplied.GetAllGrantedTags(GrantedTags);

	//if (AssetTags.HasTagExact(EffectGameplayTag) || GrantedTags.HasTagExact(EffectGameplayTag))
	if (IsTarget(SpecApplied))
	{
		AbilitySystemComponent->OnGameplayEffectStackChangeDelegate(ActiveHandle)->AddUObject(this, &UGHOAsyncTaskEffectStackChanged::GameplayEffectStackChanged);
		OnGameplayEffectStackChange.Broadcast(EffectGameplayTag, ActiveHandle, 1, 0);
	}
}

void UGHOAsyncTaskEffectStackChanged::OnRemoveGameplayEffectCallback(const FActiveGameplayEffect & EffectRemoved)
{
	//FGameplayTagContainer AssetTags;
	//EffectRemoved.Spec.GetAllAssetTags(AssetTags);

	//FGameplayTagContainer GrantedTags;
	//EffectRemoved.Spec.GetAllGrantedTags(GrantedTags);

	//if (AssetTags.HasTagExact(EffectGameplayTag) || GrantedTags.HasTagExact(EffectGameplayTag))
	if (IsTarget(EffectRemoved.Spec))
	{
		OnGameplayEffectStackChange.Broadcast(EffectGameplayTag, EffectRemoved.Handle, 0, 1);
	}
}

void UGHOAsyncTaskEffectStackChanged::GameplayEffectStackChanged(FActiveGameplayEffectHandle EffectHandle, int32 NewStackCount, int32 PreviousStackCount)
{
	OnGameplayEffectStackChange.Broadcast(EffectGameplayTag, EffectHandle, NewStackCount, PreviousStackCount);
}

bool UGHOAsyncTaskEffectStackChanged::IsTarget(const FGameplayEffectSpec& Spec)const
{
	FGameplayTagContainer AssetTags;
	Spec.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	Spec.GetAllGrantedTags(GrantedTags);

	return (AssetTags.HasTagExact(EffectGameplayTag) || GrantedTags.HasTagExact(EffectGameplayTag));
}
