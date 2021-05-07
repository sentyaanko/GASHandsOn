# Add-StartupEffects-System
`Character` に起動時に一度だけ `GameplayEffect` を適用する仕組み

# 方法

* `AGHOCharacterBase` と `UGHOAbilitySystemComponent` を拡張します。
* 適用する `GameplayEffect` はエディタ上で指定することになります。
* ここでは仕組みだけ作成し、エディタ上での `GameplayEffect` の設定は扱いません。

# 手順

ソース類

1. `AGHOCharacterBase` に以下を追加
	* 関数
		* `GetStartupEffects()`
			* `UGHOAbilitySystemComponent` からのアクセス用
		* `AddStartupEffects()`
			* 実装は `UGHOAbilitySystemComponent` に移譲
	* プロパティ
		* `StartupEffects`
			* エディタ上で起動時に一度だけ適用する `GameplayEffect` を設定するためのプロパティ
1. `UGHOAbilitySystemComponent` に以下を追加
	* 関数
		* `AddStartupEffects()`
			* `AGHOCharacterBase::GetStartupEffects()` で取得した `GameplayEffect` を `Character` に適用する関数
	* プロパティ
		* `bStartupEffectsApplied`
			* 処理が行われたかの判定用


以上で `Character` に起動時に一度だけ `GameplayEffect` を適用する仕組みが追加されます。
主に `Regen` で利用することになりますが、今はまだ `Regen` 関連のアトリビュートがありません。
`Regen` アトリビュートの追加の際に今回の仕組みを利用することになります。


-----
おしまい。
