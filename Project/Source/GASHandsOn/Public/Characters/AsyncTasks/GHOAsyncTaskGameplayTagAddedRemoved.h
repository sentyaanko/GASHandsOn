// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GHOAsyncTaskGameplayTagAddedRemoved.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameplayTagAddedRemoved, FGameplayTag, Tag);

/*
by GASDocumentation
	Blueprint node to automatically register a listener for changes to a GameplayEffect's stack count based on an Asset or Granted tag on the Effect.
	Useful to use in UI.
	Blueprint node to automatically register a listener for FGameplayTags added and removed.
	Useful to use in Blueprint/UMG.
和訳
	FGameplayTags の追加と削除のリスナーを自動的に登録するブループリントノード
	Blueprint/UI での使用に便利です。
*/
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class GASHANDSON_API UGHOAsyncTaskGameplayTagAddedRemoved : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	// Listens for FGameplayTags added and removed.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UGHOAsyncTaskGameplayTagAddedRemoved* ListenForGameplayTagAddedOrRemoved(UAbilitySystemComponent* AbilitySystemComponent, FGameplayTagContainer InTags);

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
	void Initialize(UAbilitySystemComponent* InAbilitySystemComponent, FGameplayTagContainer InTags);
	void TagChanged(const FGameplayTag Tag, int32 NewCount);

protected:
	UPROPERTY(BlueprintAssignable)
	FOnGameplayTagAddedRemoved OnTagAdded;

	UPROPERTY(BlueprintAssignable)
	FOnGameplayTagAddedRemoved OnTagRemoved;

private:
	UPROPERTY()
	UAbilitySystemComponent* ASC;

	FGameplayTagContainer Tags;

};
