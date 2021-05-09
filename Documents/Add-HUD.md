# Add-HUD
アトリビュートの現在値がわかりにくいため、 `HUD` を追加する。

# 方法

* `widget` 関連の実装方法については `AbilitySystem` が絡む部分のみ記述します。
* まだ扱わないことは以下の通り
	* `AsyncTaskAttributeChanged` の実装
		* `Health` などの現在値の更新で使用する予定。
		* そのため、今回の更新では `Health` 等の更新があっても `HUD` に反映されない。

# 手順

ソース類


1. `UGHOGaugeWidget` を新規作成
	* 概要
		* `HUD` 上のゲージのパラメータ。
		* 現在値、最大値、リジェネ値を持つ。
1. `UGHOHUDWidgetParam` を新規作成
	* 概要
		* `HUD` のパラメータ。
		* `HeroLevel` `MoveSpeed` `Health` `Mana` `Stamina` を持つ。
1. `UGHOHUDWidget` を新規作成
	* 概要
		* `HUD` 用の基底クラス。
		* 各アトリビュートを設定するためのインターフェイスを定義するためのクラス。
	* 関数
		* `SetParamter()`
			* 初期化用関数
		* 個々のパラメータの更新関数
			* `SetHeroLevel()`
			* `SetMoveSpeed()`
			* `SetHealth()`
			* `SetHealthMax()`
			* `SetHealthRegenRate()`
			* `SetMana()`
			* `SetManaMax()`
			* `SetManaRegenRate()`
			* `SetStamina()`
			* `SetStaminaMax()`
			* `SetStaminaRegenRate()`
			* ただし、以下の3つは `AsyncTaskAttributeChanged` を使用するため通常の更新処理では利用しない予定
				* `SetHealth()`
				* `SetMana()`
				* `SetStamina()`
1. `AGHOPlayerController` に以下を追加
	* プロパティ
		* `UIHUDWidgetClass`
			* ブループリント側で `HUD` クラスを指定するためのプロパティ。
		* `UIHUDWidget`
			* `HUD` のインスタンス保持用
	* 関数
		* `CreateHUD()`
			* `HUD` 構築用関数
			* `LocalPlayer` のみ処理する。
			* `PlayerState` と `AttributeSet` が取得できた時のみ処理する。
		* `GetHUD()`
			* `HUD` 取得用関数
		* `OnRep_PlayerState()`
			* `CreateHUD()` を呼び出す。
1. `AGHOPlayerState` に以下を追加
	* 関数
		* 以下のアトリビュート更新時のコールバック内で `HUD` の更新処理を呼び出す。
			* `HealthMaxChanged()`
			* `HealthRegenRateChanged()`
			* `ManaMaxChanged()`
			* `ManaRegenRateChanged()`
			* `StaminaMaxChanged()`
			* `StaminaRegenRateChanged()`
1. `UGHOAttributeSetBase` に以下を追加
	* 関数
		* `GetHUDParameter()`
			* `HUD` の表示に必要なパラメータ一式を返す関数。
1. `AGHOHeroCharacterBase` に以下を追加
	* 関数
		* `InitializeAfterAbilitySystem()`
			* `AGHOPlayerController::CreateHD()` の呼び出し

BP 類。

1. `WBP_AttributeGauge` の新規作成
	* ゲージ用の `widget` 部品の集まり。
1. `WBP_HUD` の新規作成
	* `HUD` 用のウィジェット

以上で、プレイヤー自身のアトリビュートを `HUD` に表示させることが出来ます。


-----
おしまい。
