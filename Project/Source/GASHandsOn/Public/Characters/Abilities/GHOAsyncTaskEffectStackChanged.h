// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GHOAsyncTaskEffectStackChanged.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnGameplayEffectStackChanged, FGameplayTag, EffectGameplayTag, FActiveGameplayEffectHandle, Handle, int32, NewStackCount, int32, OldStackCount);

/*
by GASDocumentation
	Blueprint node to automatically register a listener for changes to a GameplayEffect's stack count based on an Asset or Granted tag on the Effect.
	Useful to use in UI.
和訳
	Effect のアセット又は Granted タグに基づいて GameplayEffect のスタックカウントが変更された場合に自動的にリスナーを登録するブループリントノード
	UI での使用に便利です。
*/
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class GASHANDSON_API UGHOAsyncTaskEffectStackChanged : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UGHOAsyncTaskEffectStackChanged* ListenForGameplayEffectStackChange(UAbilitySystemComponent* InAbilitySystemComponent, FGameplayTag InEffectGameplayTag);

	/*
	by GASDocumentation
		You must call this function manually when you want the AsyncTask to end.
		For UMG Widgets, you would call it in the Widget's Destruct event.
	和訳
		ASynkTask を終了させたいときは、この関数を手動で呼び出す必要があります。
		UMG Widgets の場合は、 Widget の Destruct イベントで呼び出します。
	*/
	UFUNCTION(BlueprintCallable)
	void EndTask();

private:
	void Initialize(UAbilitySystemComponent* InAbilitySystemComponent, FGameplayTag InEffectGameplayTag);

	UFUNCTION()
	void OnActiveGameplayEffectAddedCallback(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);

	UFUNCTION()
	void OnRemoveGameplayEffectCallback(const FActiveGameplayEffect& EffectRemoved);

	UFUNCTION()
	void GameplayEffectStackChanged(FActiveGameplayEffectHandle EffectHandle, int32 NewStackCount, int32 PreviousStackCount);

	bool IsTarget(const FGameplayEffectSpec& Spec)const;

protected:
	UPROPERTY(BlueprintAssignable)
	FOnGameplayEffectStackChanged OnGameplayEffectStackChange;

private:
	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent;

	FGameplayTag EffectGameplayTag;

};
