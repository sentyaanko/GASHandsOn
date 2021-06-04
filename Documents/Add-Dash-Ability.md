# Add-Dash-Ability
`Q` を押したときに `Stamina` を消費して、短距離を素早く移動する `Dash` の仕組みを実装する

# 方法
* `Dash` 中に表示する `Montage` を用意する
* コスト用の `GameplayEffect` を用意する
	* `Stamina` を消費するように設定する
* キャラクターに登録するための `GameplayAbility` を用意する
	* `Q` キーで発動するようにする
	* cost で前述の `GameplayEffect` を指定する
	* コストの判定を行い、前述の `Montage` を再生し、移動用の力を加えるようにイベントグラフを組む
* キャラクターに前述の `GameplayAbility` を登録する
* まだ扱わないことは以下の通り
	* `Montage` で使用するアニメーションは仮のものを使用。


# 手順

## ソース類

1. `enum EGHOAbilityInputID` （入力 ID 用）に以下の値を追加
	* `Ability3`
		* `Dash` 用


## GameplayTag の追加

| タグ名                        | 用途                                                                |
|-------------------------------|---------------------------------------------------------------------|
| `Ability.Skill.Ability3`      | アビリティ.スキル.Ability3中(Dash)                                  |

## 入力設定の追加

| 入力名     | 割当       |
|------------|------------|
| `Ability3` | Q          |

## BP 類

1. `AM_Dash` という `Montage` を新規作成
	* モンタージュの設定
		* `DefaultGroup.DefaultSlot` を仮で `ThirdPersonJump_End` に
	* アセット詳細の設定
		* `ブレンドオプション > Blend In > Blend Time` を `0.1` に
			* モーションが短いため、デフォルトの `0.25` は長いため。
		* `ブレンドオプション > Enable Auto Blend Out` を false に
			* 移動が完了するまで最終ポーズを維持したいため
1. `GE_DashCost` を `GameplayEffect` から派生して新規作成
	* クラスのデフォルトの設定
		* `Gameplay Effect > Modifiers` に要素を追加し以下を指定
			* `Attribute` を `Stamina` に
			* `Modifier Magnitude > Scalable Float Magnitude` を `-15.0` に
1. `GA_Dash` を `GHOGameplayAbility` から派生して新規作成
	* クラスのデフォルトの設定
		* `Ability > Ability Input ID` を `Ability3` に
		* `Ability > Ability ID` を `Ability3` に
		* `タグ > Ability Tags` を `Ability.Skill.Ability3` に
		* `タグ > Activation Owned Tagss` を `Ability.Skill.Ability3` に
		* `タグ > Activation Blocked Tags` を `Stae.Dead` `Stae.CrowdControl.Stun` `Ability.Skill` に
		* `高度 > Server Respects Remote Ability Cancellation` を false に
		* `Costs > Cost Gameplay Effect Class` を `GE_DashCost` に
	* 詳細はグラフ参照。
1. `BP_HeroCharacterBase` に以下を追加
	* クラスのデフォルトの設定
		* `GASHands On > Abilities > Character Abilities` に要素を追加し `GA_Dash` を指定


以上で、 キャラクターに `Dash` のアビリティが適用できます。

-----
# 補足

-----
## ApplyRootMotionConstantForce

[Unreal Engine 4 Documentation > Unreal Engine Blueprint API Reference > Ability > Tasks > Apply Root Motion Constant Force](https://docs.unrealengine.com/en-US/BlueprintAPI/Ability/Tasks/ApplyRootMotionConstantForce/index.html)


-----
## UGameplayAbility

### bServerRespectsRemoteAbilityCancellation

[./Add-AimDownSights.md#bServerRespectsRemoteAbilityCancellation](./Add-AimDownSights.md#bServerRespectsRemoteAbilityCancellation) を参照。


-----
おしまい。
