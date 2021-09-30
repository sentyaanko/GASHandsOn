// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GHOGEEC_Damage.generated.h"

/**
 * 
 */
UCLASS()
class GASHANDSON_API UGHOGEEC_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UGHOGEEC_Damage();

	// UGameplayEffectExecutionCalculation interface
public:
	/*
	by Epic
		Called whenever the owning gameplay effect is executed. 
		Allowed to do essentially whatever is desired, including generating new modifiers to instantly execute as well.
	
		@note: Native subclasses should override the auto-generated Execute_Implementation function and NOT this one.
	
		@param ExecutionParams		Parameters for the custom execution calculation
		@param OutExecutionOutput	[OUT] Output data populated by the execution detailing further behavior or results of the execution
	和訳
		所有する GameplayEffect が実行されるたびに呼び出されます。
		新しい modifier を生成して即座に実行するなど、本質的に望むことは何でも出来ます。

		@note: ネイティブのサブクラスではこの関数ではなく自動生成された Execute_Implementation 関数をオーバーライドスべきです。

		@param ExecutionParams		カスタム実行計算のパラメータ
		@param OutExecutionOutput	[OUT] 実行によって生成された、実行のさらなる動作や結果を示す出力データ
	*/
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

	// End of UGameplayEffectExecutionCalculation interface

protected:
	float HeadShotMultiplier;
};
