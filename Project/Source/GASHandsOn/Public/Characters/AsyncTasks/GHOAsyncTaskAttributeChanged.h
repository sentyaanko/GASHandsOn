// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AbilitySystemComponent.h"
#include "GHOAsyncTaskAttributeChanged.generated.h"


UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnAttributeChanged, float, NewValue, float, OldValue);


USTRUCT(BlueprintType)
struct GASHANDSON_API FGHOAttributeChangedParam
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "GASHandsOn")
	FGameplayAttribute Attribute;

	UPROPERTY(BlueprintReadWrite, Category = "GASHandsOn")
	FOnAttributeChanged OnAttributeChanged;

	FDelegateHandle Handle;
};

/*
by Epic(ReleaseNote)[https://dq8iqaixvew1d.cloudfront.net/ja/WhatsNew/Builds/ReleaseNotes/4_23/index.html]
	New: Added support for setting the display name of the AsyncTaskProxy pin on async BP nodes by setting ExposedAsyncProxy = "DisplayName".
	Changed the display name of the pin for GameplayTasks to AsyncTask instead of AsyncTaskProxy.
和訳
	New: Async BP ノードの AsyncTaskProxy ピンの表示名を 「 ExposedAsyncProxy = "DisplayName" 」とすることで設定できるようになりました。
	GameplayTask のピンの表示名を AsyncTaskProxy ではなく AsyncTask に変更しました。
解説
	下記 UCLASS で 「 ExposedAsyncProxy = AsyncTask 」をしているのは上記と合わせる目的です。
*/

/**
by GASDocumentation
	Blueprint node to automatically register a listener for all attribute changes in an AbilitySystemComponent.
	Useful to use in UI.
和訳
	AbilitySystemComponent のすべてのアトリビュート変更に対してリスナーを自動的に登録するブループリントノードです。
	UI での使用に便利です。
*/
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class GASHANDSON_API UGHOAsyncTaskAttributeChanged : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "GASHandsOn")
	static UGHOAsyncTaskAttributeChanged* ListenForAttributesChange(UAbilitySystemComponent* InAbilitySystemComponent, TArray<FGHOAttributeChangedParam> InAttributeChangedParams);

	/*
	by GASDocumentation
		You must call this function manually when you want the AsyncTask to end.
		For UMG Widgets, you would call it in the Widget's Destruct event.
	和訳
		ASynkTask を終了させたいときは、この関数を手動で呼び出す必要があります。
		UMG Widgets の場合は、 Widget の Destruct イベントで呼び出します。
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn")
	void EndTask();

private:
	void Initialize(UAbilitySystemComponent* InAbilitySystemComponent, TArray<FGHOAttributeChangedParam> InAttributeChangedParams);

private:
	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	TArray<FGHOAttributeChangedParam> AttributeChangedParams;
};
