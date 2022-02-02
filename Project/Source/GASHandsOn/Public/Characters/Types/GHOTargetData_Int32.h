// (C) Sentya Anko 2021

#pragma once

//#include "Abilities/GameplayAbilityTypes.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GHOTargetData_Int32.generated.h"


USTRUCT()
struct GASHANDSON_API FGameplayAbilityTargetData_Int32: public FGameplayAbilityTargetData
{
	GENERATED_BODY()

	FGameplayAbilityTargetData_Int32()
		: Value(0)
	{
	}
	virtual ~FGameplayAbilityTargetData_Int32() {};

	void operator=(const FGameplayAbilityTargetData_Int32& rhs)
	{
		Value = rhs.Value;
	}

	// begin FGameplayAbilityTargetData interface
	/*
	by Epic
		Returns the serialization data, must always be overridden
	*/
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_Int32::StaticStruct();
	}

	/*
	by Epic
		Returns a debug string representation
	*/
	virtual FString ToString() const override
	{
		return TEXT("FGameplayAbilityTargetData_Int32");
	}

	// end FGameplayAbilityTargetData interface

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);


	UPROPERTY()
	int32 Value;
	
};


template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_Int32> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_Int32>
{
	enum
	{
		WithNetSerializer              = false,                         // by Epic: struct has a NetSerialize function for serializing its state to an FArchive used for network replication.
		WithCopy                       = true                           // by Epic: struct can be copied via its copy assignment operator.
	};
};
