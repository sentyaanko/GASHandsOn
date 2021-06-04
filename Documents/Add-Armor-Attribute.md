# Add-Armor-Attribute
`Character` にダメージを軽減する `Armor` アトリビュートを追加する

# 方法

* `AttributeSet` にアトリビュート `Armor` を追加する。
* `UGHOGEEC_Damage` のダメージ計算で `Armor` を利用するようにする。
* まだ扱わないことは以下の通り
	* `Armor` の値を設定する `GameplayEffect` がないので、このままだと効果がない。
	* 後に `PassiveArmor` の対応をする際に利用することになる。


# 手順

## ソース類

1. `UGHOAttributeSetBase` に以下を追加
	* プロパティ
		* `Armor`
			* ダメージを軽減するためのプロパティ
			* 具体的な値は `GameplayEffect` 側で設定（まだないので常に初期値）
	* 関数
		* `OnRep_Armor()`
			* レプリケーション用の関数。
			* マクロの呼び出しを行う。
		* `GetLifetimeReplicatedProps()`
			* レプリケーション設定用のマクロ呼び出し
1. `GHODamageStatics` に以下を追加
	* プロパティ
		* `Armor`
			* `Armor` のキャプチャ用。マクロを利用する。
	* 関数
		* `コンストラクタ`
			* `Armor` のキャプチャ用メンバの初期化。
			* マクロの呼び出しを行う。
1. `UGHOGEEC_Damage` に以下を追加
	* 関数
		* `コンストラクタ`
			* `RelevantAttributesToCapture` に `Armor` の情報を追加
		* `Execute_Implementation()`
			* キャプチャした `Armor` を取得し、ダメージ値算出の際に軽減に利用する。


以上で、 キャラクターに `Armor` アトリビュートが追加され、ダメージ計算時に利用されるようになります。


-----
おしまい。
