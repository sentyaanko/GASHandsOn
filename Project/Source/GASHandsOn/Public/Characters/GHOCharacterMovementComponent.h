// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GHOCharacterMovementComponent.generated.h"

enum class EGHOMoveFlag : uint8
{
	MF_None = 0x00,
	MF_AimDownSights = 0x01,
};
ENUM_CLASS_FLAGS(EGHOMoveFlag)

/**
 * 
 */
UCLASS()
class GASHANDSON_API UGHOCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
private:
	class FGHOSavedMove : public FSavedMove_Character
	{
	public:

		typedef FSavedMove_Character Super;

		/** Clear saved move properties, so it can be re-used. */
		///@brief Resets all saved variables.
		virtual void Clear() override;

		/** Called to set up this saved move (when initially created) to make a predictive correction. */
		///@brief Sets up the move before sending it to the server. 
		virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;

		/** Returns true if this move can be combined with NewMove for replication without changing any behavior */
		///@brief This is used to check whether or not two moves can be combined into one.
		///Basically you just check to make sure that the saved variables are the same.
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;

		/** Called before ClientUpdatePosition uses this SavedMove to make a predictive correction	 */
		///@brief Sets variables on character movement component before making a predictive correction.
		virtual void PrepMoveFor(class ACharacter* Character) override;

		/** Returns a byte containing encoded special movement information (jumping, crouching, etc.)	 */
		///@brief Store input commands in the compressed flags.
		virtual uint8 GetCompressedFlags() const override;

#if 0
		// Sprint
		uint8 SavedRequestToStartSprinting : 1;

		// Aim Down Sights
		uint8 SavedRequestToStartADS : 1;
#else
		EGHOMoveFlag MoveFlag;
#endif
	};

	class FGHONetworkPredictionData_Client : public FNetworkPredictionData_Client_Character
	{
	public:
		FGHONetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		/** Allocate a new saved move. Subclasses should override this if they want to use a custom move class. */
		///@brief Allocates a new copy of our custom saved move
		virtual FSavedMovePtr AllocateNewMove() override;
	};

public:
	UGHOCharacterMovementComponent();

	// INetworkPredictionInterface interface
public:
	/** Unpack compressed flags from a saved move and set state accordingly. See FSavedMove_Character. */
	/** Get prediction data for a client game. Should not be used if not running as a client. Allocates the data on demand and can be overridden to allocate a custom override if desired. Result must be a FNetworkPredictionData_Client_Character. */
	/** @return FNetworkPredictionData_Client instance used for network prediction. */
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	// End of INetworkPredictionInterface interface

	// UMovementComponent interface
public:
	virtual float GetMaxSpeed() const override;

	// End of UMovementComponent interface

	// UMovementComponent interface
protected:
	/** Unpack compressed flags from a saved move and set state accordingly. See FSavedMove_Character. */
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	// End of UMovementComponent interface

public:
	const EGHOMoveFlag& GetMoveFlag()const { return MoveFlag; }

protected:
#if 0
	// Sprint
	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StartSprinting();
	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StopSprinting();
#endif

	// Aim Down Sights
	UFUNCTION(BlueprintCallable, Category = "Aim Down Sights")
	void StartAimDownSights();
	UFUNCTION(BlueprintCallable, Category = "Aim Down Sights")
	void StopAimDownSights();

private:
	float GetSpeedMultiplier(const EGHOMoveFlag& Flag)const;

protected:
#if 0
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	float SprintSpeedMultiplier;
#endif
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Down Sights")
	float AimDownSightsSpeedMultiplier;

private:
#if 0
	uint8 RequestToStartSprinting : 1;
	uint8 RequestToStartADS : 1;
#else
	EGHOMoveFlag MoveFlag;
#endif
};
