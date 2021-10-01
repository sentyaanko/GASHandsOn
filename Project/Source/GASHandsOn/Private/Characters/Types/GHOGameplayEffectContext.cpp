// (C) Sentya Anko 2021

#include "Characters/Types/GHOGameplayEffectContext.h"

bool FGHOGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	return Super::NetSerialize(Ar, Map, bOutSuccess) && TargetData.NetSerialize(Ar, Map, bOutSuccess);
}
