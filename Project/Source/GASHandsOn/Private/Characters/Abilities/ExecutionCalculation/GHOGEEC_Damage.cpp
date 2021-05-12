// (C) Sentya Anko 2021


#include "Characters/Abilities/ExecutionCalculation/GHOGEEC_Damage.h"
#include "Characters/Abilities/GHOAbilitySystemComponent.h"
#include "Characters/Abilities/GHOAttributeSetBase.h"

/*
by GASDocumentation
	Declare the attributes to capture and define how we want to capture them from the Source and Target.
和訳
	キャプチャーする属性を宣言し、ソースとターゲットからどの様にキャプチャーするかを定義します。
*/
struct GHODamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);

	GHODamageStatics()
	{
		/*
		by GASDocumentation
			Snapshot happens at time of GESpec creation

			We're not capturing anything from the Source in this example, but there could be like AttackPower attributes that you might want.
		和訳
			GESpec 作成時のスナップショット

			この例ではソースから何もキャプチャしていませんが、 AtackPower アトリビュートのように必要なものがあるかもしれません。
		*/

		/*
		by GASDocumentation
			Capture optional Damage set on the damage GE as a CalculationModifier under the ExecutionCalculation
		和訳
			ダメージ GameplayEffect の ExecutionCalculation の下の CalculationModifier で設定されているオプションのダメージ値を取り込みます。
		*/
		DEFINE_ATTRIBUTE_CAPTUREDEF(UGHOAttributeSetBase, Damage, Source, true);
	}
};

static const GHODamageStatics& DamageStatics()
{
	static GHODamageStatics DStatics;
	return DStatics;
}

UGHOGEEC_Damage::UGHOGEEC_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
}

void UGHOGEEC_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters & ExecutionParams, OUT FGameplayEffectCustomExecutionOutput & OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	/*
	by GASDocumentation
		Gather the tags from the source and target as that can affect which buffs should be used
	和訳
		ソースとターゲットのタグを収集し、どのバフを使用するのかを決定します。
	*/
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Damage = 0.0f;
	/*
	by GASDocumentation
		Capture optional damage value set on the damage GE as a CalculationModifier under the ExecutionCalculation
	和訳
		ダメージ GameplayEffect の ExecutionCalculation の下の CalculationModifier で設定されているオプションのダメージ値を取り込みます。
	*/
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvaluationParameters, Damage);

	/*
	by GASDocumentation
		Add SetByCaller damage if it exists
	和訳
		SetByCaller ダメージがあれば加算します。
	*/
	Damage += FMath::Max<float>(Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), false, -1.0f), 0.0f);

	/*
	by GASDocumentation
		Can multiply any damage boosters here
	和訳
		ここで任意のダメージブースターを乗算することが出来ます。
	*/
	float UnmitigatedDamage = Damage;

	const float MitigatedDamage = UnmitigatedDamage;

	if (MitigatedDamage > 0.f)
	{
		/*
		by GASDocumentation
			Set the Target's damage meta attribute
		和訳
			ターゲットのメタ・アトリビュートを設定します。
		*/
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().DamageProperty, EGameplayModOp::Additive, MitigatedDamage));
	}
}
