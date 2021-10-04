// (C) Sentya Anko 2021


#include "Characters/SystemGlobals/GHOAbilitySystemGlobals.h"
#include "Characters/Types/GHOGameplayEffectContext.h"

/*
解説
	利用するには Config/DefaultGame.ini の編集が必要です。
*/

FGameplayEffectContext* UGHOAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FGHOGameplayEffectContext();
}
