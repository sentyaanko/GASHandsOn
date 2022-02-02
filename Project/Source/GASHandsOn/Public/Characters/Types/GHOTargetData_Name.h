// (C) Sentya Anko 2021

#pragma once

//#include "Abilities/GameplayAbilityTypes.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GHOTargetData_Name.generated.h"


USTRUCT()
struct GASHANDSON_API FGameplayAbilityTargetData_Name : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

	FGameplayAbilityTargetData_Name()
		: Value()
	{
	}
	virtual ~FGameplayAbilityTargetData_Name() {};

	void operator=(const FGameplayAbilityTargetData_Name& rhs)
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
		return FGameplayAbilityTargetData_Name::StaticStruct();
	}

	/*
	by Epic
		Returns a debug string representation
	*/
	virtual FString ToString() const override
	{
		return TEXT("FGameplayAbilityTargetData_Name");
	}

	// end FGameplayAbilityTargetData interface

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);


	UPROPERTY()
	FName Value;
	
};


template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_Name> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_Name>
{
	enum
	{
		WithNetSerializer              = false,                         // by Epic: struct has a NetSerialize function for serializing its state to an FArchive used for network replication.
		WithCopy                       = true                           // by Epic: struct can be copied via its copy assignment operator.
	};
};
