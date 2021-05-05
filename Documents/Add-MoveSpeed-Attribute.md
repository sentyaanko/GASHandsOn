# Add-MoveSpeed-Attribute
`MoveSpeed` アトリビュートの実装

# 方法

* `UGHOAttributeSetBase` を拡張し、 `MoveSpeed` アトリビュートを追加する。

# 手順

ソース類

1. `UGHOAttributeSetBase` に以下を追加
	* プロパティ
		* `MoveSpeed`
			* 移動速度用のアトリビュート
			* 併せてマクロによるアクセサ定義も行う
	* 関数
		* `GetLifetimeReplicatedProps()`
			* レプリケーション設定用のマクロ呼び出し
		* `PreAttributeChange()`
			* 上限と下限のクランプを行う。
		* `OnRep_MoveSpeed()`
			* レプリケーション用の関数。
			* マクロの呼び出しを行う。
1. `UGHOCharacterMovementComponent` に以下を追加
	* 関数
		* `GetMaxSpeed()`
			* 暫定で直値で書いていた移動速度をアトリビュートから取得するように修正


Editor での設定類。

BP 類。

1. `GE_HeroAttributes` を更新
	* `GameplayEffects` の `Modifiers` に要素を追加し、以下を指定
		* `Attribute` に `GHOAttributeSetBase.MoveSpeed`
		* `Modifier Op` に `Override`
		* `Modifier Magnitude/Magnitude Calculation Type` に `ScalableFloat`
		* `Modifier Magnitude/Scalable Float Magnitude` に `600`


以上で MoveSpeed アトリビュートが追加され、移動速度のバフなどの実装時に利用できます。

-----
おしまい。
