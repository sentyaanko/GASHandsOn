# Add-FloatingDamageText
ダメージ値がわかりにくいため、 `FloatingDamageText` を追加する。

# 方法

* `widget` 関連の実装方法については `AbilitySystem` が絡む部分のみ記述します。
* `UGHOAttributeSetBase` で `Damage` を受け取った際、 `Source` の `Controller` に対して表示のための関数を呼び出す。
	* つまりはダメージを与えたクライアントのみ呼び出すようにしている。
	* つまりは各クライアントで、自分が与えたダメージのみ表示している。
* `GASDocumentation` と比較すると、関数名が異なるので注意が必要です。
	* `DamageText` と `DamageNumber` が混在し、使い分け方も不明だったため。

# 手順

## ソース類

1. `EGHOFloatingTextType` を新規作成
	* 概要
		* テキストカラーの切り替え用。準備のみ。
1. `UGHOFloatingDamageTextComponent` を `UWidgetComponent` から派生して新規作成
	* 概要
		* `FloatingDamageText` 用の基底クラス。
		* 3D 上に表示するので `UWidgetComponent` を使用する。
	* 関数
		* `SetDamage()`
			* テキスト内容設定用関数。
1. `AGHOPlayerController` に以下を追加
	* プロパティ
		* `UIFloatingDamageTextComponentClass`
			* ブループリント側で `FloatingDamageTextComponent` クラスを指定するためのプロパティ。
	* 関数
		* コンストラクタ
			* `UIFloatingDamageTextComponentClass` のデフォルト値の設定
		* `RPCClientShowDamageText()` / `RPCClientShowDamageText_Implementation()`
			* ダメージ表示登録関数。
			* `RPC` 関数。
				* `Client` で `Reliable` 。
				* `WithValidation` は昔は必須だったようだが今はなくてもコンパイルできる。
					* まだ意味のある実装がないのでコメントアウトしている。
			* `UIFloatingDamageTextComponentClass` で指定したコンポーネントを渡された `TargetCharacter` に追加する。
			* 破棄は Blueprint 側でおこなう。　
1. `UGHOAttributeSetBase` に以下を追加
	* 関数
		* `PostGameplayEffectExecute_Damage()`
			* ダメージが渡された際に表示登録関数を呼び出す。

## BP 類

1. `WBP_FloatingText` の新規作成
	* テキスト用の `widget` 部品の集まり。
	* カラーとテキストを受け取るのみ。テキストの内容は使う側で制御する。
	* 詳細は Blueprint を参照。
1. `BP_FloatingDamageTextComponent` を `UGHOFloatingDamageTextComponent` から派生して新規作成
	* `FloatingDamageText` 用の `WidgetComponent`
	* クラスのデフォルトの `ユーザーインターフェイス > Widget Class` で `WBP_FloatingText` を指定している。
	* 詳細は Blueprint を参照。


以上で、キャラクターがダメージを受け取った際に `FloatingDamageText` を表示させることが出来ます。


-----
おしまい。
