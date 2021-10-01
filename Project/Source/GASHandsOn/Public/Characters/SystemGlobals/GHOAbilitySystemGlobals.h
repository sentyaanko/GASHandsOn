// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "GHOAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class GASHANDSON_API UGHOAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

public:
	/*
	by Epic
		Should allocate a project specific GameplayEffectContext struct. Caller is responsible for deallocation
	翻訳
		プロジェクト固有の GameplayEffectContext 構造体を割り当てます。呼び出し元が割当解除の責任を負います。
	*/
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;

};
