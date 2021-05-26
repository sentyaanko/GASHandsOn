# Add-PassiveArmor-Ability
`GameplayEffect` のスタックを使用して、スタックが増える毎に `Armor` が増える `PassiveArmor` の仕組みを実装する

# 方法
* 1 スタック毎に `Armor` を 10 加算し、最大 4 までスタックする `GameplayEffect` を用意する。
* 1 秒毎に上記の `GameplayEffect` を適用する `GameplayAbility` を用意する。
* 上記の `GameplayAbility` をキャラクターに適用する。
* まだ扱わないことは以下の通り
	* スタン時に無効化させないようにしますが、スタンの実装はまだ先です。


# 手順

ソース類

1. `enum EGHOAbilityInputID` （入力 ID 用）に以下の値を追加
	* `Ability4`
		* `PassiveArmor` 用
1. `UGHOGameplayAbility` に以下を追加
	* プロパティ
		* `bActivateAbilityOnGranted`
			* アビリティ付与直後に発動させるかどうかのフラグ
	* 関数
		* `OnAvatarSet()`
			* `bActivateAbilityOnGranted` を見て
			* マクロの呼び出しを行う。


Editor での設定類。

GameplayTag の追加

| タグ名                        | 用途                                                                |
|-------------------------------|---------------------------------------------------------------------|
| `Ability.NotCanceledByStun`   | アビリティ.Stun時でも中断されないアビリティフラグ                   |
| `Effect.Shared.PassiveArmor`  | GameplayEffect の PassiveArmor 用                                   |

* `Ability.NotCanceledByStun`
	* `PassiveArmor` はスタンしても効果が切れないようにしたいので、そのためのタグ。
	* まだスタンさせるアビリティがないですが、このアビリティ側の準備だけを行います。


BP 類。

1. `GE_PassiveArmor` を `GameplayEffect` から派生して新規作成
	* クラスのデフォルトの設定
		* `Gameplay Effect > Duration Policy` を `Infinite` に
		* `Gameplay Effect > Modifier` に要素を追加し以下を指定
			* `Attribute` を `Armor` に
			* `Modifier Magnitude > Scalable Float Magnitude` を `10.0` に
		* `タグ > GameplayEffectAssetTag` を `Effect.RemoveOnDeath` に
		* `タグ > GrantedTags` を `Effect.Shared.PassiveArmor` に
		* `Stacking > Stacking Type` を `Aggregate by Target` に
		* `Stacking > Stack Limit Count` を `4` に
		* `Stacking > Stack Expiration Policy` を `Remove Single Stack and Refresh Duration` に
		* `Stacking` の設定については [./Add-BuffMoveSpeed-Effect.md](./Add-BuffMoveSpeed-Effect.md) の補足を参照
1. `GA_PassiveArmor` を `GHOGameplayAbility` から派生して新規作成
	* クラスのデフォルトの設定
		* `Ability > AbilityID` を `Ability4` に
		* `Activate Ability on Granted` を `true` に
		* `タグ > Ability Tags` を `Ability.NotCanceledByStun` に
		* `タグ > Activation Blocked Tags` を `Stae.Dead` に
		* `高度 > Net Execution Policy` を `Server Only` に
	* 詳細はグラフ参照。
1. `WBP_HUD` に以下を追加
	* `Armor` の表示
		* `Listen for Attributes Change` を利用
	* `PassiveArmor` の表示
		* `Listen for Gameplay Effect Stack Change` を利用
	* 詳細はグラフ参照。
1. `BP_HeroCharacterBase` に以下を追加
	* クラスのデフォルトの設定
		* `GASHands On > Abilities > Character Abilities` に要素を追加し `GA_PassiveArmor` を指定


以上で、 キャラクターに `PassiveArmor` のアビリティが適用できます。

-----
おしまい。
