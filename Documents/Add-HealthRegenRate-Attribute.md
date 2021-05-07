# Add-HealthRegenRate-Attribute
`Character` に定期的に `Health` を増加させる `HealthRegenRate` アトリビュートを追加する

# 方法

* `AttributeSet` に定期的に `Health` を加算するためのアトリビュート `HealthRegenRate` を追加する。
* 毎秒 `Health` に `HealthRegenRate` を加算させる `GameplayEffect` を追加する。
* `Character` の `StartupEffects` に上記のエフェクトを追加する。

# 手順

ソース類

1. `UGHOAttributeSetBase` に以下を追加
	* プロパティ
		* `HealthRegenRate`
			* `Health` を増加させるためのプロパティ
			* 具体的な値や頻度は `GameplayEffect` 側で設定
	* 関数
		* `OnRep_HealthRegenRate()`
			* レプリケーション用の関数。
			* マクロの呼び出しを行う。
		* `GetLifetimeReplicatedProps()`
			* レプリケーション設定用のマクロ呼び出し
1. `AGHOPlayerState` に以下を追加
	* プロパティ
		* `HealthRegenRateChangedDelegateHandle`
			* `HealthRegenRate` 変更時のデリゲートハンドル
	* 関数
		* `AActor::BeginPlay()` (override)
			* `HealthRegenRateChangedDelegateHandle` の初期化
		* `HealthRegenRateChanged()`
			* `HealthRegenRate` 変更時のデリゲートの実体
			* 変更結果を UI に反映するのもここで行うが、今回は省略

BP 類。

1. `GE_HeroAttributes` の変更
	* `GameplayEffects > Modifiers` に要素を追加し、以下を指定
		* `Attribute` に `GHOAttributeSetBase.HealthRegenRate`
		* `Modifier Op` に `Override`
		* `Modifier Magnitude/Magnitude Calculation Type` に `ScalableFloat`
		* `Modifier Magnitude/Scalable Float Magnitude` に `2` （これが一度に加算する初期値になります）
1. `GE_HealthRegen` の追加
	* `Gameplay Effect > Duration Policy` に `Infinite`
	* `GameplayEffects > Modifiers` に要素を追加し、以下を指定
		* `Attribute` に `GHOAttributeSetBase.Health`
		* `Magnitude Calculation Type` に `Attribute Based`
		* `Backing Attribute > Attribute to Capture` に `GHOAttributeSetBase.Health`
	* `Period > Period` に `1.0` （これが加算する頻度になります。）
	* `タグ > Ongoing Tag Requirements > Ignore Tags` に `State.Dead`
1. `BP_HeroCharacterBase` の変更
	* クラスのデフォルトの設定
		* `GASHandsOn > Abilities > Startup Effects`  に要素を追加し、 `GE_HealthRegen` を指定

以上で、 キャラクターに毎秒 `Health` が 2 増加する `GameplayEffect` が追加されます。


-----
おしまい。
