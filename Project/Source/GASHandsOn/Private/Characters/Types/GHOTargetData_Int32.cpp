// (C) Sentya Anko 2021

#include "Characters/Types/GHOTargetData_Int32.h"

bool FGameplayAbilityTargetData_Int32::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << Value;
	bOutSuccess = true;
	return true;
}
