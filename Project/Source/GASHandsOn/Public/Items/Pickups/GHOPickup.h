// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "GHOPickup.generated.h"


class AGHOCharacterBase;

UCLASS()
class GASHANDSON_API AGHOPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGHOPickup();

	// UObject interface
public:
	/*
	by Epic
		Returns properties that are replicated for the lifetime of the actor channel
	和訳
		アクターチャンネルのライフタイム中にレプリケーションされるプロパティを返します。
	*/
	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const override;

	// End of UObject interface

	// AActor interface
public:
	/*
	by Epic
		Event when this actor overlaps another actor, for example a player walking into a trigger.
		For events when objects have a blocking collision, for example a player hitting a wall, see 'Hit' events.
		@note Components on both this and the other Actor must have bGenerateOverlapEvents set to true to generate overlap events.
	和訳
		このアクターが他のアクターと重なった時のイベント、例えばプレイヤーがトリガーの中に入ってきたときなど。
		オブジェクトがブロックコリジョンを起こした時のイベント（例えばプレイヤーが壁にぶつかる）については 'Hit' イベントを参照してください。
		@note オーバーラップイベントとを生成するには、このアクターと他のアクターの両方の bGenerateOverlapEvents が true に設定されている必要があります。
	by GASShooter
		Pickup on touch
	和訳
		触れることでピックアップ
	*/
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	// End of AActor interface

protected:
	/*
	by GASShooter
		Check if pawn can use this pickup
	和訳
		ポーンがこのピックアップを使用できるかチェック
	*/
	UFUNCTION(BlueprintNativeEvent)
	bool CanBePickedUp(AGHOCharacterBase* TestCharacter) const;
	bool CanBePickedUp_Implementation(AGHOCharacterBase* TestCharacter) const;

	/*
	by GASShooter
		Show effects when pickup disappears
	和訳
		ピックアップが消滅したときのエフェクトを表示
	*/
	UFUNCTION(BlueprintNativeEvent)
	void OnPickedUp();
	void OnPickedUp_Implementation();

	/*
	by GASShooter
		Show effects when pickup appears
	和訳
		ピックアップが出現したときのエフェクトを表示
	*/
	UFUNCTION(BlueprintNativeEvent)
	void OnRespawned();
	void OnRespawned_Implementation();

	UFUNCTION()
	void OnRep_IsActive();

private:
	void PickupOnTouch(AGHOCharacterBase* Pawn);
	void GivePickupTo(AGHOCharacterBase* Pawn);
	void RespawnPickup();


protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "GASHandsOn|Pickup")
	class UCapsuleComponent* CollisionComp;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IsActive, Category = "GASHandsOn|Pickup")
	bool bIsActive;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|Pickup")
	bool bCanRespawn;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|Pickup")
	float RespawnTime;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|Pickup")
	FGameplayTagContainer RestrictedPickupTags;

	/*
	by GASShooter
		Sound played when player picks it up
	和訳
		プレイヤーが拾ったときに再生する音
	*/
	UPROPERTY(EditDefaultsOnly, Category = "GASHandsOn|Pickup")
	class USoundCue* PickupSound;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|Pickup")
	TArray<TSubclassOf<class UGHOGameplayAbility>> AbilityClasses;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|Pickup")
	TArray<TSubclassOf<class UGameplayEffect>> EffectClasses;

	/*
	by GASShooter
		The character who has picked up this pickup
	和訳
		このピックアップを拾ったキャラクター
	*/
	UPROPERTY(BlueprintReadOnly, Replicated)
	AGHOCharacterBase* PickedUpBy;

	FTimerHandle TimerHandle_RespawnPickup;

};
