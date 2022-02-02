// (C) Sentya Anko 2021

#include "Characters/Types/GHOTargetData_Name.h"
#include "Characters/GHOCharacterBase.h"

bool FGameplayAbilityTargetData_Name::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << Value;
	bOutSuccess = true;
	return true;
}
