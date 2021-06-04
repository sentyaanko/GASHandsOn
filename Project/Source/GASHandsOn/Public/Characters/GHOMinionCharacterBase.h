// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Characters/GHOCharacterBase.h"
#include "GHOMinionCharacterBase.generated.h"

/*
by GASDocumentation
	An AI controlled minion character.
和訳
	AI が制御するミニオンのキャラクター。
*/
UCLASS()
class GASHANDSON_API AGHOMinionCharacterBase : public AGHOCharacterBase
{
	GENERATED_BODY()
	
public:
	AGHOMinionCharacterBase(const FObjectInitializer& ObjectInitializer);

	// AActor interface
protected:
	/*
	by Epic
		Overridable native event for when play begins for this actor.
	和訳
		このアクタの Play が始まるときのオーバーライド可能なネイティブイベント。
	by Epic
		Called when the game starts or when spawned
	和訳
		ゲーム開始時又はスポーン時に呼び出される。
	*/
	virtual void BeginPlay() override;

	// End of AActor interface

	// IAbilitySystemInterface interface
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// End of IAbilitySystemInterface interface

	// AGHOCharacterBase interface
public:
	virtual class UGHOAttributeSetBase* GetAttributeSet() override;
	virtual const class UGHOAttributeSetBase* GetAttributeSet() const override;

	// End of AGHOCharacterBase interface

private:
	/*
	by Epic
		Attribute changed callbacks
	和訳
		Attribute 変更コールバック
	*/
	void HealthChanged(const struct FOnAttributeChangeData& Data);

	/*
	by Epic
		Tag change callbacks
	和訳
		タグ変更コールバック
	*/
	void StunTagChanged(const struct FGameplayTag CallbackTag, int32 NewCount);

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|Character")
	FText CharacterName;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "GASHandsOn|UI")
	class UWidgetComponent* UIFloatingStatusBarComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASHandsOn|UI")
	TSubclassOf<class UGHOFloatingStatusBarWidget> UIFloatingStatusBarClass;

private:
	/*
	by Epic
		Actual hard pointer to AbilitySystemComponent
	和訳
		AbilitySystemComponent への実際の強参照
	*/
	UPROPERTY()
	class UGHOAbilitySystemComponent* AbilitySystemComponent;

	/*
	by Epic
		Actual hard pointer to AttributeSetBase
	和訳
		AttributeSetBaseへの実際の強参照
	*/
	UPROPERTY()
	class UGHOAttributeSetBase* AttributeSetBase;
	
	UPROPERTY()
	class UGHOFloatingStatusBarWidget* UIFloatingStatusBar;

	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle StunChangedDelegateHandle;

};
