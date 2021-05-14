# Add-FloatingStatusBar
他者のアトリビュートの現在値がわかりにくいため、 `FloatingStatusBar` を追加する。

# 方法

* `widget` 関連の実装方法については `AbilitySystem` が絡む部分のみ記述します。
* まだ扱わないことは以下の通り
	* キャラクターの名前の表示（取り急ぎ不要であり、 `GASDocument` でも表示していない為）

# 手順

ソース類
1. `UGHOFloatingStatusBarWidget` を `UUserWidget` から派生して新規作成
	* 概要
		* `FloatingStatusBar` 用の基底クラス。
		* 各アトリビュートを設定するためのインターフェイスを定義するためのクラス。
	* 関数
		* 個々のパラメータの更新関数
			* `SetCharacterName()`
			* `SetHealthPercentage()`
			* `SetManaPercentage()`
1. `AGHOHeroCharacterBase` に以下を追加
	* プロパティ
		* `UIFloatingStatusBarClass`
			* ブループリント側で `FloatingStatusBar` クラスを指定するためのプロパティ。
		* `UIFloatingStatusBarComponent`
			* `UWidgetComponent` のインスタンス保持用
		* `UIFloatingStatusBar`
			* `FloatingStatusBar` のインスタンス保持用
	* 関数
		* コンストラクタ
			* `UWidgetComponent` のインスタンス構築
		* `AActor::BeginPlay()`(override)
			* `InitializeFloatingStatusBar()` を呼び出す。
			* 詳しくはコメント参照。
		* `InitializeFloatingStatusBar()`
			* `FloatingStatusBar` 構築用関数
		* `GetFloatingStatusBar()`
			* `FloatingStatusBar` 取得用関数
		* `InitializeAfterAbilitySystem()`
			* `InitializeFloatingStatusBar()` を呼び出す。
			* 詳しくはコメント参照。
1. `AGHOPlayerState` に以下を追加
	* 関数
		* 以下のアトリビュート更新時のコールバック内で `FloatingStatusBar` の更新処理を呼び出す。
			* `HealthChanged()`
			* `HealthMaxChanged()`
			* `ManaChanged()`
			* `ManaMaxChanged()`
1. `UGHOAttributeSetBase` に以下を追加
	* 関数
		* `PreAttributeChange()`
			* `FloatingStatusBar` と直接は関係ありません。
			* クランプ処理を追加しています。
			* 詳しくはコメント参照。
1. `AGHOHeroCharacterBase` に以下を追加
	* 関数
		* `InitializeAfterAbilitySystem()`
			* `AGHOPlayerController::CreateHD()` の呼び出し

BP 類。

1. `WBP_FloatingGauge` の新規作成
	* ゲージ用の `widget` 部品の集まり。
1. `WBP_FloatingStatusBar` の新規作成
	* `FloatingStatusBar` 用のウィジェット

以上で、キャラクター上部に `FloatingStatusBar` を表示させることが出来ます。


-----
おしまい。
