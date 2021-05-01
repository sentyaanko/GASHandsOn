# Add-AimDownSights
`AimDownSighs` の実装

# 方法

* `UGHOCharacterMovementComponent` を拡張し、 `AimDownSighs` 機能を実装する。
* まだ扱わないことは以下の通り
	* `Sprint` や `Skill` との競合対応
	* カメラ位置の調整
	* 移動速度を `AttributeSet` に追加

# 手順

ソース類

1. `enum EGHOAbilityInputID` （入力 ID 用）に以下の値を追加
	* `Ability2`
		* `AimDownSights` 用
1. `AGHOCharacterBase` に以下を追加
	* 関数
		* `GetAttributeSet()`
			* const 関数を作るために const 版の用意
		* `IsMovable()`
			* 移動可能化の判定
			* `AttributeSet` に対して `IsAlive()` のチェック
			* `AbilitySystemComponent` に対して `IsStun()` のチェック
1. `AGHOHeroCharacterBase` に以下を追加
	* 関数
		* `GetAttributeSet()`
			* const 関数を作るために const 版の用意
1. `AGHOHeroCharacterBase` に以下を追加
	* プロパティ
		* `StunTag`
			* スタン用の `GameplayTag` のキャッシュ
	* 関数
		* コンストラクタ
			* `StunTag` の初期化
		* `IsStun()`
			* スタン判定
1. `UGHOAttributeSetBase` に以下を追加
	* 関数
		* `IsAlive()`
			* `Health` が 0 より大きいか判定
1. `UGHOCharacterMovementComponent`
	* 複雑なため、ソースを参照。


Editor での設定類。

GameplayTag の追加

| タグ名                        | 用途                                                                |
|-------------------------------|---------------------------------------------------------------------|
| `Ability.AimDownSights`       | アビリティ.AimDownSights中                                          |
| `Ability.Skill`               | アビリティ.スキルの実行中（判定でのみ使用）                         |
| `Ability.Skill.Ability1`      | アビリティ.スキル.Ability1中（未使用、ショットを打つ際に利用予定）  |
| `Ability.Skill.Ability2`      | 未使用                                                              |
| `Ability.Sprint`              | アビリティ.Sprint中（判定でのみ使用）                               |
| `State.AimDownSights`         | キャラクターの状態.AimDownSights中                                  |
| `State.AimDownSights.Removal` | キャラクターの状態.AimDownSights解除中                              |
| `State.CrowdControl.Stun`     | キャラクターの状態.CC.スタン中（判定でのみ使用）                    |

入力設定の追加

| 入力名     | 割当       |
|------------|------------|
| `Ability1` | 左クリック |
| `Ability2` | 右クリック |

BP 類。

1. `GE_AimDownSights` の追加
	* クラスのデフォルトの設定
		* `Gameplay Effect > Duration Policy` を `Infinite` に
			* 効果が永続されるように設定。（解除は `GE_AimDownSightsRemoval` によって行う想定）
		* `タグ > GameplayEffectAssetTag > Added` を `Effect.RemoveOnDeath` に
			* 死亡時にアビリティが削除されるように設定。
		* `タグ > GrantedTags > Added` を `State.AimDownSights` に
			* キャラクターの状態に `State.AimDownSights` が追加されるように設定
1. `GE_AimDownSightsRemoval` の追加
	* クラスのデフォルトの設定
		* `Gameplay Effect > Duration Policy` を `Infinite` に
			* 効果が永続されるように設定。
		* `タグ > GameplayEffectAssetTag > Added` を `Effect.RemoveOnDeath` に
			* 死亡時にアビリティが削除されるように設定。
		* `タグ > GrantedTags > Added` を `State.AimDownSights.Removal` に
			* キャラクターの状態に `State.AimDownSights.Removal` が追加されるように設定
1. `GA_AimDownSights` の追加
	* クラスのデフォルトの設定
		* `Ability > Ability Input ID` を `Ability2` に
		* `Ability > Ability ID` を `Ability2` に
		* `タグ > Ability Tags` を `Ability.AimDownSights` に
		* `タグ > Cancel Abilities with Tags` を `Ability.Sprint` に
			* 未実装であるが、 `Sprint` 中にこのアビリティを使うと `Sprint` が解除される
		* `タグ > Activation Owned Tags` を `Ability.AimDownSights` に
		* `タグ > Activation Blocked Tags` を `State.CrowdControl.Stun` `State.Dead` `Ability.Skill` に
			* 未実装であるが、このアビリティはスキル発動中はブロックされる。
		* `高度 > Server Respects Remote Ability ` を `false` に
			* コストが掛からないことと、 `Removal` を使用することで、この設定をしても問題が発生しないようにしている。
	* `ActivateAbility`
		* `GE_AimDownSights` を自身に付与
		* カメラを寄せる
		* `GHOCharacterMovementComponent::StartAimDownSights()` の呼び出し
		* ボタンが離されたら `EndAbility` の呼び出し
	* `OnEndAbility`
		* `GE_AimDownSightsRemoval` を自身に付与
		* `GE_AimDownSights` を自身から削除
		* `GE_AimDownSightsRemoval` を自身から削除
		* カメラを戻す
		* `GHOCharacterMovementComponent::StopAimDownSights()` の呼び出し

以上で CharacterMovementComponent を自作のものに差し替え、右クリック中に `AimDownSights` 扱いとなり、
移動速度が下がり、カメラがキャラクターに寄るようになります。


-----
おしまい。
