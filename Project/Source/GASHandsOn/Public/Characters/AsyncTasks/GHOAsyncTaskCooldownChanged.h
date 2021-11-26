// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AbilitySystemComponent.h"
#include "GHOAsyncTaskCooldownChanged.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCooldownChanged, FGameplayTag, CooldownTag, float, TimeRemaining, float, Duration);

/*
by GASDocumentation
	Blueprint node to automatically register a listener for changes (Begin and End) to an array of Cooldown tags.
	Useful to use in UI.
和訳
	Cooldown タグの配列に対する変更（開始と終了）に対して、自動的にリスナーを登録する Blueprint ノードです。
	UIでの使用に便利です。
*/
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class GASHANDSON_API UGHOAsyncTaskCooldownChanged : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	/*
	by GASDocumentation
		Listens for changes (Begin and End) to cooldown GameplayEffects based on the cooldown tag.
		UseServerCooldown determines if the Sever's cooldown is returned in addition to the local predicted cooldown.
		If using ServerCooldown, TimeRemaining and Duration will return -1 to signal local predicted cooldown has begun.
	和訳
		cooldown タグに基づいて、 cooldown GameplayEffect の変更（開始と終了）をリッスンします。
		bUseServerCooldown は、ローカルで予測された cooldown に加えて、サーバーの cooldown を返すかどうかを決定します。
		ServerCooldown を使用する場合、 TimeRemaining と Duration は -1 を返し、ローカルで予測された cooldown が始まったことを知らせます。
	*/
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "GASHandsOn")
	static UGHOAsyncTaskCooldownChanged* ListenForCooldownChange(UAbilitySystemComponent* InAbilitySystemComponent, FGameplayTagContainer InCooldownTags, bool bInUseServerCooldown);

	/*
	by GASDocumentation
		You must call this function manually when you want the AsyncTask to end.
		For UMG Widgets, you would call it in the Widget's Destruct event.
	和訳
		AsyncTask を終了させたいときは、この関数を手動で呼び出す必要があります。
		UMG Widgets の場合は、 Widget の Destruct イベントで呼び出します。
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn")
	void EndTask();

private:
	void Initialize(UAbilitySystemComponent* InAbilitySystemComponent, FGameplayTagContainer InCooldownTags, bool bInUseServerCooldown);

	void OnActiveGameplayEffectAddedCallback(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);

	void CooldownTagChanged(const FGameplayTag CooldownTag, int32 NewCount);

	bool GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining, float& CooldownDuration);

protected:
	UPROPERTY(BlueprintAssignable)
	FOnCooldownChanged OnCooldownBegin;

	UPROPERTY(BlueprintAssignable)
	FOnCooldownChanged OnCooldownEnd;

private:
	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent;

	FGameplayTagContainer CooldownTags;

	bool bUseServerCooldown;

};
