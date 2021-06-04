# GASHandsOn
Gameplay Ability System Hands On Project.

GASDocumentation で行っている各種機能を一つづつ実装していくハンズオン。


# 更新履歴

* v0.0.1
	* UE4 のテンプレートで自動生成されたファイルの追加
	* GameplayAbility プラグインの有効化
	* Gameplay Modules の有効化
	* Public/Privete フォルダの階層分け
* v0.0.2
	* GameInstance/GameMode/GameState/PlayerState/PlayerController などの基本的なクラスの追加
	* Player 操作用の Character 基底クラスの用意、テンプレートから移動などの最低限の操作用コードの複製
	* 作業用のマップをテンプレートから複製
	* BluePrint のファイル群のフォルダ分け
* v0.0.3
	* AbilitySystemComponent のソース追加
	* AttributeSet のソース追加
	* Character/PlayerState/PlayerController などに AbilitySystemComponent/AttributeSet を設定
	* プロジェクト用 GameplayAbility 基底クラスの追加
* v0.0.4
	* プロジェクト用 CharacterMovementComponent 基底クラスの追加
* v0.0.5
	* Character に Health の Attribute 組み込み
	* 詳細は [./Documents/0.0.5.Attribute-Health.md](./Documents/0.0.5.Attribute-Health.md) を参照
* v0.0.6
	* Character に Jump の Ability 組み込み
	* 詳細は [./Documents/0.0.6.Ability-Jump.md](./Documents/0.0.6.Ability-Jump.md) を参照
* v0.0.7
	* Character に AimDownSights の Ability 組み込み
	* 詳細は [./Documents/0.0.7.Ability-AimDownSights.md](./Documents/0.0.7.Ability-AimDownSights.md) を参照
* v0.0.8
	* Character に MoveSpeed の Attribute 組み込み
	* 詳細は [./Documents/0.0.8.Attribute-MoveSpeed.md](./Documents/0.0.8.Attribute-MoveSpeed.md) を参照
* v0.0.9
	* Character の MoveSpeed にバフをかける GameplayEffect の組み込み
	* 詳細は [./Documents/0.0.9.GameplayEffect-BuffMoveSpeed.md](./Documents/0.0.9.GameplayEffect-BuffMoveSpeed.md) を参照
* v0.0.10
	* Character に起動時に一度だけ GameplayEffect を適用する仕組みの組み込み
	* 詳細は [./Documents/0.0.10.System-StartupEffects.md](./Documents/0.0.10.System-StartupEffects.md) を参照
* v0.0.11
	* Character に HealthRegenRate の Attribute 組み込み
	* 詳細は [./Documents/0.0.11.Attribute-HealthRegenRate.md](./Documents/0.0.11.Attribute-HealthRegenRate.md) を参照
* v0.0.12
	* MaxHealth を HealthMax に変更（ドロップダウンリストの並びの利便性のため）
	* Character に Mana 関連の Attribute 組み込み
	* 詳細は [./Documents/0.0.12.Attribute-Mana.md](./Documents/0.0.12.Attribute-Mana.md) を参照
* v0.0.13
	* Character に Stamina 関連の Attribute 組み込み
	* 詳細は [./Documents/0.0.13.Attribute-Stamina.md](./Documents/0.0.13.Attribute-Stamina.md) を参照
* v0.0.13.1
	* StaminaMax が変更された際に Stamina を Adjust していなかった不具合の修正
* v0.0.14
	* HUD 組み込み
	* 詳細は [./Documents/0.0.14.GUI-HUD.md](./Documents/0.0.14.GUI-HUD.md) を参照
* v0.0.15
	* AsyncTaskAttributeChanged 組み込み
	* 詳細は [./Documents/0.0.15.AsyncTaskAttributeChanged.md](./Documents/0.0.15.AsyncTaskAttributeChanged.md) を参照
* v0.0.16
	* EngineSubsystem 組み込み
	* 詳細は [./Documents/0.0.16.EngineSubsystem.md](./Documents/Add-EngineSubsystem.md) を参照
* v0.0.17
	* ダメージの仕組みの組み込み
	* リスポーンの仕組みの組み込み
	* 詳細は [./Documents/0.0.17.System-Damage.md](./Documents/0.0.17.System-Damage.md) を参照
* v0.0.18
	* スタミナゲージの更新処理を誤ってスタミナ最大値にバインドしていたのを修正
	* Character に Sprint の Ability 組み込み
	* 詳細は [./Documents/0.0.18.Ability-Sprint.md](./Documents/0.0.18.Ability-Sprint.md) を参照
* v0.0.19
	* ダメージボリュームの秒間ダメージがテキストでは 15 だが実際は 20 だったので 15 に統一
	* FloatingStatusBar 組み込み
	* 詳細は [./Documents/0.0.19.GUI-FloatingStatusBar.md](./Documents/0.0.19.GUI-FloatingStatusBar.md) を参照
* v0.0.20
	* FloatingDamageText 組み込み
	* 詳細は [./Documents/0.0.20.GUI-FloatingDamageText.md](./Documents/0.0.20.GUI-FloatingDamageText.md) を参照
* v0.0.21
	* C++ 内のデフォルトクラスの指定を一箇所に集めた。
* v0.0.22
	* AbilityTask_PlayMontageAndWaitForEvent の追加
	* 詳細は [./Documents/0.0.22.AbilityTask_PlayMontageAndWaitForEvent.md](./Documents/0.0.22.AbilityTask_PlayMontageAndWaitForEvent.md) を参照
* v0.0.23
	* このファイルの更新履歴の v0.0.20 ～ v0.0.22 が数字が 1 小さく誤っていたのを修正（ v0.0.19 が二つになっていた）
	* Character に FireGun の Ability 組み込み
	* 詳細は [./Documents/0.0.23.Ability-FireGun.md](./Documents/0.0.23.Ability-FireGun.md) を参照
* v0.0.24
	* Character に Armor の Attribute 組み込み
	* 詳細は [./Documents/0.0.24.Attribute-Armor.md](./Documents/0.0.24.Attribute-Armor.md) を参照
* v0.0.25
	* AbilityTask_WaitReceiveDamage の追加
	* 詳細は [./Documents/0.0.25.AbilityTask-WaitReceiveDamage.md](./Documents/0.0.25.AbilityTask-WaitReceiveDamage.md) を参照
* v0.0.26
	* 過去のドキュメントの誤字修正
	* AsyncTaskEffectStackChanged の追加
	* 詳細は [./Documents/0.0.26.AsyncTaskEffectStackChanged.md](./Documents/0.0.26.AsyncTaskEffectStackChanged.md) を参照
* v0.0.27
	* 過去のドキュメントの誤字修正
	* Character に PassiveArmor の Ability 組み込み
	* 詳細は [./Documents/0.0.27.Ability-PassiveArmor.md](./Documents/0.0.27.Ability-PassiveArmor.md) を参照
* v0.0.28
	* v0.0.23 の不具合修正
		* FireGun で使用するモーションが短いため、 Blend In/Out の時間を 0 に変更
		* AnimBP にて Slot ノードがなかったためモンタージュが再生されていなかったのを修正。
	* Character に Dash の Ability 組み込み
	* 詳細は [./Documents/0.0.28.Ability-Dash.md](./Documents/0.0.28.Ability-Dash.md) を参照
* v0.0.29
	* 状態異常 Stun の組み込み
	* 詳細は [./Documents/0.0.29.Condition-Stun.md](./Documents/0.0.29.Condition-Stun.md) を参照
* v0.0.30
	* Character に Meteor の Ability 組み込み
	* 詳細は [./Documents/0.0.30.Ability-Meteor.md](./Documents/0.0.30.Ability-Meteor.md) を参照
* v0.0.31
	* Document の typo の修正
* v0.0.32
	* AimDownSights 後の CameraBoom の ArmLength を元の状態に戻す際の処理を修正
		* 直値 -> ヒーロークラスが持っている初期値を利用するように変更
	* HeroAI 組み込み
	* 詳細は [./Documents/0.0.32.HeroAI.md](./Documents/0.0.32.HeroAI.md) を参照
* v0.0.33
	* Projectile の不具合修正
		* 状況：ダメージでターゲットが死亡した場合。
		* 原因：AdditionalEffects で無効の AbilitySystemComponent を参照していた。
		* 対応：AdditionalEffects のループ処理の前に AbilitySystemComponent の Valid チェックをおこなように修正
		* 備考：AdditionalEffects でターゲットを死亡させる物があるのであれば、 AbilitySystemComponent の Valid チェック はループ内に移動すること。
	* Minion 組み込み
	* 詳細は [./Documents/0.0.33.Minion.md](./Documents/0.0.33.Minion.md) を参照
* v0.0.34
	* BP_GEVolumeBase のタイマーをクリアする処理の追加
	* RespawnCountdown 組み込み
	* 詳細は [./Documents/0.0.34.GUI-RespawnCountdown.md](./Documents/0.0.34.GUI-RespawnCountdown.md) を参照
* v0.0.35
	* Character に CharacterLevel / XP / XPBounty / Gold / GoldBounty の Attribute 組み込み
	* 詳細は [./Documents/0.0.35.Attributes-Level_XP_Gold.md](./Documents/0.0.35.Attributes-Level_XP_Gold.md) を参照

-----
おしまい。
