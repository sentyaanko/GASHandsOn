// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "GHOASCActorBase.generated.h"

/*
by GASShooter
	Base AActor class that has an AbilitySystemComponent but does not inherit from ACharacter.
和訳
	AbilitySystemComponent を持つが、 ACharacter を継承していないベース AActor クラス。
*/
UCLASS()
class GASHANDSON_API AGHOASCActorBase : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGHOASCActorBase();

	// IAbilitySystemInterface interface
public:
	/*
	by Epic
		Returns the ability system component to use for this actor. 
		It may live on another actor, such as a Pawn using the PlayerState's component
	和訳
		Actor に使用する AbilitySystemComponent を返します。
		これは他の Actor に所有させることが出来ます。例えば、 PlayerState コンポーネントを使用した Pawn のように。
	補足
		実際、プレイヤーが操作するキャラクターは PlayerState(AGHOPlayerState) が所持しているインスタンスを返却します。
		AIが操作するキャラクターは Character(AGHOMinionCharacterBase) が所持しているインスタンスを返却します。
	*/
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// End of IAbilitySystemInterface interface


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


private:
	UPROPERTY()
	class UGHOAbilitySystemComponent* AbilitySystemComponent;
};
