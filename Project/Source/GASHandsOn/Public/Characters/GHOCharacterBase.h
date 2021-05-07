// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GASHandsOn.h"
#include "GHOCharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, AGHOCharacterBase*, Character);

UCLASS()
class GASHANDSON_API AGHOCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGHOCharacterBase(const FObjectInitializer& ObjectInitializer);

//	// AActor interface
//public:
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;
//
//	// End of AActor interface

//	// APawn interface
//protected:
//	// Called to bind functionality to input
//	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
//
//	// End of APawn interface

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
		AIが操作するキャラクターは Character(AGHOMinionCharacterBase) が所持しているインスタンスを返却する予定です。
		TODO: 実装したらコメントを修正すること。
	*/
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// End of IAbilitySystemInterface interface

public:
	virtual class UGHOAttributeSetBase* GetAttributeSet();
	virtual const class UGHOAttributeSetBase* GetAttributeSet()const;

public:
	void Die();

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GDCharacter")
	void FinishDying();

	/*
	by GASDocumentation
		Switch on AbilityID to return individual ability levels. 
		Hardcoded to 1 for every ability in this project.
	和訳	
		AbilityID をオンにして、個々のアビリティレベルを返します。
		このプロジェクトではすべてのアビリティに 1 がハードコーディングされれています。
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GDCharacter")
	virtual int32 GetAbilityLevel(EGHOAbilityInputID AbilityID) const;


	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GDCharacter")
	virtual bool IsAlive() const;


	const TArray<TSubclassOf<class UGHOGameplayAbility>>& GetCharacterAbilities()const { return CharacterAbilities; }

	const TSubclassOf<class UGameplayEffect>& GetDefaultAttributes()const { return DefaultAttributes; }

	const TArray<TSubclassOf<class UGameplayEffect>>& GetStartupEffects()const { return StartupEffects; }

	bool IsMovable()const;

protected:
	/*
	by GASDocumentation
		Initialize the Character's attributes. 
		Must run on Server but we run it on Client too so that we don't have to wait.
		The Server's replication to the Client won't matter since the values should be the same.
	和訳
		キャラクターノア地理ビュートを初期化します。
		サーバーで実行する必要がありますが、待たされないようにクライアントでも実行します。
		値は同じはずなので、サーバーからクライアントへのレプリケーションは問題になりません。
	*/
	void InitializeAttributes();

	//
	void AddStartupEffects();

protected:
	/*
	by GASDocumentation
		Grant abilities on the Server. 
		The Ability Specs will be replicated to the owning client.
	和訳
		サーバー上でアビリティを付与する。
		Ability Specs は Owning Client にレプリケーションされます。
	*/
	void AddCharacterAbilities();

private:
	/*
	by GASDocumentation
		Removes all CharacterAbilities.
		Can only be called by the Server.
		Removing on the Server will remove from Client too.
	和訳
		すべての CharacterAbilities を削除します。
		サーバーからのみ呼び出すことが出来ます。
		サーバーから削除されると、クライアントからも同様に削除されます。
	*/
	void RemoveCharacterAbilities();


protected:
	////TODO: Implement Death Animation.
	//// Death Animation
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASHandsOn|Animation")
	//UAnimMontage* DeathMontage;

	/*
	by GASDocumentation
		Default attributes for a character for initializing on spawn/respawn.
		This is an instant GE that overrides the values for attributes that get reset on spawn/respawn.
	和訳
		スポーン/リスポーン の際に初期化されるキャラクターのデフォルトアトリビュートです。
		これはインスタント GameplayEffect で、スポーン/リスポーン の際にリセットされるアトリビュートの値を上書きします。
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	/*
	by GASDocumentation
		These effects are only applied one time on startup
	和訳
		これらの効果は起動維持に一度だけ適用されます。
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;

	/*
	by GASDocumentation
		Default abilities for this Character.
		These will be removed on Character death and regiven if Character respawns.
	和訳
		キャラクターのデフォルトアトリビュートです。
		これはキャラクターが死亡した際に削除され、リスポーンした際に再付与されます。
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|Abilities")
	TArray<TSubclassOf<class UGHOGameplayAbility>> CharacterAbilities;

	UPROPERTY(BlueprintAssignable, Category = "GASHandsOn|GDCharacter")
	FCharacterDiedDelegate OnCharacterDied;

};
