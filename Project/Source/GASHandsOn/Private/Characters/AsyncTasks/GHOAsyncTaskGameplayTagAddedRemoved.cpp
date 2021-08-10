// (C) Sentya Anko 2021


#include "Characters/AsyncTasks/GHOAsyncTaskGameplayTagAddedRemoved.h"

UGHOAsyncTaskGameplayTagAddedRemoved* UGHOAsyncTaskGameplayTagAddedRemoved::ListenForGameplayTagAddedOrRemoved(UAbilitySystemComponent * AbilitySystemComponent, FGameplayTagContainer InTags)
{
	if (!IsValid(AbilitySystemComponent) || InTags.Num() < 1)
	{
		return nullptr;
	}

	UGHOAsyncTaskGameplayTagAddedRemoved* ListenForGameplayTagAddedRemoved = NewObject<UGHOAsyncTaskGameplayTagAddedRemoved>();
	ListenForGameplayTagAddedRemoved->Initialize(AbilitySystemComponent, InTags);

	return ListenForGameplayTagAddedRemoved;
}

void UGHOAsyncTaskGameplayTagAddedRemoved::Initialize(UAbilitySystemComponent* InAbilitySystemComponent, FGameplayTagContainer InTags)
{
	ASC = InAbilitySystemComponent;
	Tags = InTags;

	TArray<FGameplayTag> TagArray;
	InTags.GetGameplayTagArray(TagArray);

	for (FGameplayTag Tag : TagArray)
	{
		ASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UGHOAsyncTaskGameplayTagAddedRemoved::TagChanged);
	}
}

void UGHOAsyncTaskGameplayTagAddedRemoved::EndTask()
{
	if (IsValid(ASC))
	{
		TArray<FGameplayTag> TagArray;
		Tags.GetGameplayTagArray(TagArray);

		for (FGameplayTag Tag : TagArray)
		{
			ASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
		}
	}
	ASC = nullptr;
	Tags.Reset();

	SetReadyToDestroy();
	MarkPendingKill();
}

void UGHOAsyncTaskGameplayTagAddedRemoved::TagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		OnTagAdded.Broadcast(Tag);
	}
	else
	{
		OnTagRemoved.Broadcast(Tag);
	}
}
