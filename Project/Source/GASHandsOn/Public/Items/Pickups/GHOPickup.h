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

	by GASShooter
		Pickup on touch
	和訳
	*/
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	// End of AActor interface


	// Check if pawn can use this pickup
	virtual bool CanBePickedUp(AGHOCharacterBase* TestCharacter) const;

	UFUNCTION(BlueprintNativeEvent, Meta = (DisplayName = "CanBePickedUp"))
	bool K2_CanBePickedUp(AGHOCharacterBase* TestCharacter) const;
	virtual bool K2_CanBePickedUp_Implementation(AGHOCharacterBase* TestCharacter) const;

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

	// Sound played when player picks it up
	UPROPERTY(EditDefaultsOnly, Category = "GASHandsOn|Pickup")
	class USoundCue* PickupSound;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|Pickup")
	TArray<TSubclassOf<class UGHOGameplayAbility>> AbilityClasses;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|Pickup")
	TArray<TSubclassOf<class UGameplayEffect>> EffectClasses;

	// The character who has picked up this pickup
	UPROPERTY(BlueprintReadOnly, Replicated)
	AGHOCharacterBase* PickedUpBy;

	FTimerHandle TimerHandle_RespawnPickup;

	void PickupOnTouch(AGHOCharacterBase* Pawn);

	virtual void GivePickupTo(AGHOCharacterBase* Pawn);

	// Show effects when pickup disappears
	virtual void OnPickedUp();

	// Blueprint implementable effects
	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "OnPickedUp"))
	void K2_OnPickedUp();

	virtual void RespawnPickup();

	// Show effects when pickup appears
	virtual void OnRespawned();

	// Blueprint implementable effects
	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "OnRespawned"))
	void K2_OnRespawned();

	UFUNCTION()
	void OnRep_IsActive();
};
