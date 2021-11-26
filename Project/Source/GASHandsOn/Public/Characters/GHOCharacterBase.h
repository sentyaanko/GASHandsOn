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

public:
	virtual class UGHOAttributeSetBase* GetAttributeSet();
	virtual const class UGHOAttributeSetBase* GetAttributeSet()const;

public:
	void Die();

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOCharacter")
	virtual void FinishDying();

	virtual FTransform GetProjectileTransform(float Range)const;

	/*
	by GASDocumentation
		Switch on AbilityID to return individual ability levels. 
		Hardcoded to 1 for every ability in this project.
	和訳	
		AbilityID をオンにして、個々のアビリティレベルを返します。
		このプロジェクトではすべてのアビリティに 1 がハードコーディングされれています。
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOCharacter")
	virtual int32 GetAbilityLevel(EGHOAbilityInputID AbilityID) const;


	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOCharacter")
	virtual bool IsAlive() const;

	/**
	* Getters for attributes from GHOAttributeSetBase
	**/

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOCharacter|Attributes")
	int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOCharacter|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOCharacter|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOCharacter|Attributes")
	float GetMana() const;

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOCharacter|Attributes")
	float GetMaxMana() const;

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOCharacter|Attributes")
	float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOCharacter|Attributes")
	float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOCharacter|Attributes")
	float GetShield() const;

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOCharacter|Attributes")
	float GetMaxShield() const;


	const TArray<TSubclassOf<class UGHOGameplayAbility>>& GetCharacterAbilities()const { return CharacterAbilities; }

	const TSubclassOf<class UGameplayEffect>& GetDefaultAttributes()const { return DefaultAttributes; }

	const TArray<TSubclassOf<class UGameplayEffect>>& GetStartupEffects()const { return StartupEffects; }

	bool IsMovable()const;

	bool IsKnockedDown()const;

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
	// Death Animation
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASHandsOn|GHOCharacter|Animation")
	UAnimMontage* DeathMontage;

	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|GHOCharacter|Audio")
	//class USoundCue* DeathSound;
	//UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GASHandsOn|GHOCharacter|Audio")
	//FGameplayTag DeathSoundTag;

	/*
	by GASDocumentation
		Default attributes for a character for initializing on spawn/respawn.
		This is an instant GE that overrides the values for attributes that get reset on spawn/respawn.
	和訳
		スポーン/リスポーン の際に初期化されるキャラクターのデフォルトアトリビュートです。
		これはインスタント GameplayEffect で、スポーン/リスポーン の際にリセットされるアトリビュートの値を上書きします。
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|GHOCharacter|Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	/*
	by GASDocumentation
		These effects are only applied one time on startup
	和訳
		これらの効果は起動維持に一度だけ適用されます。
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|GHOCharacter|Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;

	/*
	by GASDocumentation
		Default abilities for this Character.
		These will be removed on Character death and regiven if Character respawns.
	和訳
		キャラクターのデフォルトアトリビュートです。
		これはキャラクターが死亡した際に削除され、リスポーンした際に再付与されます。
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|GHOCharacter|Abilities")
	TArray<TSubclassOf<class UGHOGameplayAbility>> CharacterAbilities;

	UPROPERTY(BlueprintAssignable, Category = "GASHandsOn|GHOCharacter")
	FCharacterDiedDelegate OnCharacterDied;

};
