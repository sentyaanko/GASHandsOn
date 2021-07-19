// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GHOCharacterMovementComponent.generated.h"

enum class EGHOMoveFlag : uint8
{
	MF_None = 0x00,
	MF_Sprinting= 0x01,
	MF_AimDownSights = 0x02,
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

		/*
		by Epic
			Clear saved move properties, so it can be re-used.
		by GASDocumentation
			Resets all saved variables.
		和訳
			SavedMove のプロパティをクリアして、再利用できるようにします。
			保存されているすべての変数をリセットします。
		*/
		virtual void Clear() override;

		/*
		by Epic
			Called to set up this saved move (when initially created) to make a predictive correction.
		by GASDocumentation
			Sets up the move before sending it to the server. 
		和訳
			この SavedMove を predictive correction （予測修正） を行うために、（初期作成時に）セットアップのために呼び出されます。
			移動をサーバーに送信する前に、移動をセットアップします。
		*/
		virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;

		/*
		by Epic
			Returns true if this move can be combined with NewMove for replication without changing any behavior
		by GASDocumentation
			This is used to check whether or not two moves can be combined into one.
			Basically you just check to make sure that the saved variables are the same.
		和訳
			この移動がレプリケーションのために NewMove と統合させても動作が変わらない場合は true を返します。
			これは、2つの移動を一つにまとめることができるかどうかをチェックするために使われます。
			基本的には、保存されている変数が同じであるかどうかを確認するだけです。
		*/
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;

		/*
		by Epic
			Called before ClientUpdatePosition uses this SavedMove to make a predictive correction
		by GASDocumentation
			Sets variables on character movement component before making a predictive correction.
		和訳
			ClientUpdatePosition がこの SavedMove を使って predictive correction （予測補正）を行う前に呼び出される。
			predictive correction （予測補正）を行う前に、 CharacterMovementComponent に変数を設定します。
		*/
		virtual void PrepMoveFor(class ACharacter* Character) override;

		/*
		by Epic
			Returns a byte containing encoded special movement information (jumping, crouching, etc.)
		by GASDocumentation
			Store input commands in the compressed flags.
		和訳
			特殊な動き（ジャンプやクラウンチングなど）の情報をエンコードしたバイトを返す。
			入力コマンドを圧縮されたフラグに格納する。
		*/
		virtual uint8 GetCompressedFlags() const override;

		bool IsSprinting()const;
		bool IsAimDownSights()const;

		/*
			基底クラスで定義された enum を直接使わず、用途に合わせた名前を設定した enum を用意し、そちらを利用するようにします。
		*/
		enum CompressedFlags
		{
			FLAG_Custom_Sprinting		= FSavedMove_Character::FLAG_Custom_0,
			FLAG_Custom_AimDownSights	= FSavedMove_Character::FLAG_Custom_1,
		};
		EGHOMoveFlag MoveFlag;
	};

	class FGHONetworkPredictionData_Client : public FNetworkPredictionData_Client_Character
	{
	public:
		FGHONetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		/*
		by Epic
			Allocate a new saved move. Subclasses should override this if they want to use a custom move class.
		by GASDocumentation
			Allocates a new copy of our custom saved move
		和訳
			新しい SavedMove を割り当てます。カスタムされた移動クラスを使用したい場合は、サブクラスでこれをオーバーライドすべきです。
			カスタムされた SavedMove の新しいコピーを割り当てる。
		*/
		virtual FSavedMovePtr AllocateNewMove() override;
	};

public:
	UGHOCharacterMovementComponent();

	// INetworkPredictionInterface interface
public:
	/*
	by Epic
		Unpack compressed flags from a saved move and set state accordingly. See FSavedMove_Character.
		Get prediction data for a client game. Should not be used if not running as a client.
		Allocates the data on demand and can be overridden to allocate a custom override if desired.
		Result must be a FNetworkPredictionData_Client_Character.
		@return FNetworkPredictionData_Client instance used for network prediction.
	和訳
		SavedMove から圧縮されたフラグをアンパックし、それに応じて状態を設定します。 FSavedMove_Character を参照してください。
		クライアントゲームの prediction （予測）データを取得します。
		クライアントとして動作していない場合は、使用しないでください。
		要求に応じてデータを割り当て、必要に応じてカスタムオーバーライドで割り当てることが出来ます。
		結果は FNetworkPredictionData_Client でなくてはなりません。
		@return ネットワーク prediction （予測）に利用される FNetworkPredictionData_Client のインスタンス
	*/
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	// End of INetworkPredictionInterface interface

	// UMovementComponent interface
public:
	virtual float GetMaxSpeed() const override;

	// End of UMovementComponent interface

	// UMovementComponent interface
protected:
	/*
	by Epic
		Unpack compressed flags from a saved move and set state accordingly. See FSavedMove_Character.
	和訳
		SavedMove から圧縮されたフラグをアンパックし、それに応じて状態を設定します。
		FSavedMove_Character を参照してください。
	*/
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	// End of UMovementComponent interface

public:
	const EGHOMoveFlag& GetMoveFlag()const { return MoveFlag; }

protected:
	// Sprint
	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StartSprinting();
	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StopSprinting();

	// Aim Down Sights
	UFUNCTION(BlueprintCallable, Category = "Aim Down Sights")
	void StartAimDownSights();
	UFUNCTION(BlueprintCallable, Category = "Aim Down Sights")
	void StopAimDownSights();

private:
	bool IsSprinting()const;
	void SetSprinting(bool flag);

	bool IsAimDownSights()const;
	void SetAimDownSights(bool flag);

	float GetSpeedMultiplier()const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float SprintingSpeedMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float AimDownSightsSpeedMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float KnockedDownSpeedMultiplier;

private:
	EGHOMoveFlag MoveFlag;
};
