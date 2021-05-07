# Add-Mana-Attribute
`Mana Attribute` の追加

# 方法

* `ATtributeSet` に以下のアトリビュートを追加する。
	* `Mana`
		* 現在値
	* `ManaMax`
		* 最大値
	* `ManaRegenRate`
		* 秒間回復値
* `Mana` の初期化コードの追加
* `BluePrint` で初期値の設定
* `BluePrint` で `Regen` の設定

# 手順

ソース類

1. `UGHOAttributeSetBase` に以下を追加
	* プロパティ
		* `Mana`
			* マナの現在値
		* `ManaMax`
			* マナの最大値
		* `ManaRegenRate`
			* `Mana` を増加させるためのプロパティ
			* 具体的な値や頻度は `GameplayEffect` 側で設定
	* 関数
		* `UObject::GetLifetimeReplicatedProps()` (override)
			* レプリケーション設定用のマクロ呼び出し
		* `UAttributeSet::PostGameplayEffectExecute()` (override)
			* `Mana` の変更を 0 から `ManaMax` になるようにクランプ処理
		* `UAttributeSet::PreAttributeChange()` (override)
			* `ManaMax` が変更された場合に `AdjustAttributeForMaxChange()` を利用して `Mana` の調整
    	* `InitializeAttributesOnSpawned()`
			* スポーン時のアトリビュートの初期化。
			* 具体的には `Mana` を `ManaMax` で初期化している。
		* `OnRep_Mana()`
			* `GAMEPLAYATTRIBUTE_REPNOTIFY()` マクロを利用する
		* `OnRep_ManaMax()`
			* `GAMEPLAYATTRIBUTE_REPNOTIFY()` マクロを利用する
		* `OnRep_ManaRegenRate()`
			* `GAMEPLAYATTRIBUTE_REPNOTIFY()` マクロを利用する
1. `AGHOPlayerState` に以下を追加
	* プロパティ
		* `ManaChangedDelegateHandle`
			* `Mana` 変更時のデリゲートハンドル
		* `ManaMaxChangedDelegateHandle`
			* `ManaMax` 変更時のデリゲートハンドル
		* `ManaRegenRateChangedDelegateHandle`
			* `ManaRegenRate` 変更時のデリゲートハンドル
	* 関数
		* `AActor::BeginPlay()` (override)
			* `ManaChangedDelegateHandle` の初期化
			* `ManaMaxChangedDelegateHandle` の初期化
			* `ManaRegenRateChangedDelegateHandle` の初期化
		* `ManaChanged()`
			* `Mana` 変更時のデリゲートの実体
			* 変更結果を UI に反映するのもここで行うが、今回は省略
		* `ManaMaxChanged()`
			* `ManaMax` 変更時のデリゲートの実体
			* 変更結果を UI に反映するのもここで行うが、今回は省略
		* `ManaRegenRateChanged()`
			* `ManaRegenRate` 変更時のデリゲートの実体
			* 変更結果を UI に反映するのもここで行うが、今回は省略

BP 類。

1. `GE_HeroAttributes` の変更
	* `GameplayEffects > Modifiers` に要素を２つ追加し、以下を指定
		* `ManaMax` 用
			* `Attribute` に `GHOAttributeSetBase.ManaMax`
			* `Modifier Op` に `Override`
			* `Modifier Magnitude/Magnitude Calculation Type` に `ScalableFloat`
			* `Modifier Magnitude/Scalable Float Magnitude` に `100`
		* `ManaRegenRate` 用
			* `Attribute` に `GHOAttributeSetBase.ManaRegenRate`
			* `Modifier Op` に `Override`
			* `Modifier Magnitude/Magnitude Calculation Type` に `ScalableFloat`
			* `Modifier Magnitude/Scalable Float Magnitude` に `2` （これが一度に加算する初期値になります）
1. `GE_ManaRegen` の追加
	* `Gameplay Effect > Duration Policy` に `Infinite`
	* `GameplayEffects > Modifiers` に要素を追加し、以下を指定
		* `Attribute` に `GHOAttributeSetBase.Mana`
		* `Magnitude Calculation Type` に `Attribute Based`
		* `Backing Attribute > Attribute to Capture` に `GHOAttributeSetBase.Mana`
	* `Period > Period` に `1.0` （これが加算する頻度になります。）
	* `タグ > Ongoing Tag Requirements > Ignore Tags` に `State.Dead`
1. `BP_HeroCharacterBase` の変更
	* クラスのデフォルトの設定
		* `GASHandsOn > Abilities > Startup Effects`  に要素を追加し、 `GE_ManaRegen` を指定



以上で `Mana` アトリビュートが追加されます。

-----
おしまい。
