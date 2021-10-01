// (C) Sentya Anko 2021

#pragma once

#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GHOGameplayEffectContext.generated.h"

/*
by GASShooter
	Data structure that stores an instigator and related data, such as positions and targets
	Games can subclass this structure and add game-specific information
	It is passed throughout effect execution so it is a great place to track transient information about an execution
和訳
	instigator と、座標やターゲットのような関連データを格納するデータ構造体。
	ゲームはこの構造体をサブクラス化し、ゲーム固有の情報を追加することができます。
	エフェクトの実行中に渡されるので、実行に関する一時的な兵法を追跡するのに適しています。
*/
USTRUCT()
struct GASHANDSON_API FGHOGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
public:
	virtual FGameplayAbilityTargetDataHandle GetTargetData()
	{
		return TargetData;
	}

	virtual void AddTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
	{
		TargetData.Append(TargetDataHandle);
	}

	/*
	by GASShooter
		Functions that subclasses of FGameplayEffectContext need to override
	和訳
		FGameplayEffectContext のサブクラスがオーバーライドする必要がある関数
	*/

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGHOGameplayEffectContext::StaticStruct();
	}

	virtual FGHOGameplayEffectContext* Duplicate() const override
	{
		FGHOGameplayEffectContext* NewContext = new FGHOGameplayEffectContext();
		*NewContext = *this;
		NewContext->AddActors(Actors);
		if (GetHitResult())
		{
			/*
			by GASShooter
				Does a deep copy of the hit result
			和訳
				hit result のディープコピーを行う
			*/
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		/*
		by GASShooter
			Shallow copy of TargetData, is this okay?
		和訳
			TargetData のシャローコピーを行う。これで大丈夫？
		*/
		NewContext->TargetData.Append(TargetData);
		return NewContext;
	}

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

protected:
	FGameplayAbilityTargetDataHandle TargetData;
};

template<>
struct TStructOpsTypeTraits<FGHOGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FGHOGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		/*
		by GASShooter
			Necessary so that TSharedPtr<FHitResult> Data is copied around
		和訳
			TSharedPtr<FHitResult> Data がコピーされるために必要です。
		*/
		WithCopy = true
	};
};
