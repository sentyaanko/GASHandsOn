// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GASHandsOn.h"
#include "GHOCharacterBase.generated.h"

UCLASS()
class GASHANDSON_API AGHOCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGHOCharacterBase();

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
	/** Returns the ability system component to use for this actor. It may live on another actor, such as a Pawn using the PlayerState's component */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// End of IAbilitySystemInterface interface


public:
	void Die();

protected:
	// Initialize the Character's attributes. Must run on Server but we run it on Client too
	// so that we don't have to wait. The Server's replication to the Client won't matter since
	// the values should be the same.
	void InitializeAttributes();

	//
	void AddStartupEffects();

protected:
	// Grant abilities on the Server. The Ability Specs will be replicated to the owning client.
	void AddCharacterAbilities();

private:
	// Removes all CharacterAbilities. Can only be called by the Server. Removing on the Server will remove from Client too.
	void RemoveCharacterAbilities();

	// Switch on AbilityID to return individual ability levels. Hardcoded to 1 for every ability in this project.
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GDCharacter")
	virtual int32 GetAbilityLevel(EGHOAbilityInputID AbilityID) const;


protected:
	// Default abilities for this Character. These will be removed on Character death and regiven if Character respawns.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|Abilities")
	TArray<TSubclassOf<class UGHOGameplayAbility>> CharacterAbilities;

private:
	bool bCharacterAbilitiesGiven = false;
};
