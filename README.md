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
	* 詳細は [./Documents/Add-CharacterHealth.md](./Documents/Add-CharacterHealth.md) を参照
* v0.0.6
	* Character に Jump の Ability 組み込み
	* 詳細は [./Documents/Add-CharacterJump.md](./Documents/Add-CharacterJump.md) を参照
* v0.0.7
	* Character に AimDownSights の Ability 組み込み
	* 詳細は [./Documents/Add-AimDownSights.md](./Documents/Add-AimDownSights.md) を参照
* v0.0.8
	* Character に MoveSpeed の Attribute 組み込み
	* 詳細は [./Documents/Add-MoveSpeed-Attribute.md](./Documents/Add-MoveSpeed-Attribute.md) を参照
* v0.0.9
	* Character の MoveSpeed にバフをかける GameplayEffect の組み込み
	* 詳細は [./Documents/Add-BuffMoveSpeed-Effect.md](./Documents/Add-BuffMoveSpeed-Effect.md) を参照
* v0.0.10
	* Character に起動時に一度だけ GameplayEffect を適用する仕組みの組み込み
	* 詳細は [./Documents/Add-StartupEffects-System.md](./Documents/Add-StartupEffects-System.md) を参照
* v0.0.11
	* Character に HealthRegenRate の Attribute 組み込み
	* 詳細は [./Documents/Add-HealthRegenRate-Attribute.md](./Documents/Add-HealthRegenRate-Attribute.md) を参照
* v0.0.12
	* MaxHealth を HealthMax に変更（ドロップダウンリストの並びの利便性のため）
	* Character に Mana 関連の Attribute 組み込み
	* 詳細は [./Documents/Add-Mana-Attribute.md](./Documents/Add-Mana-Attribute.md) を参照
* v0.0.13
	* Character に Stamina 関連の Attribute 組み込み
	* 詳細は [./Documents/Add-Stamina-Attribute.md](./Documents/Add-Stamina-Attribute.md) を参照
* v0.0.13.1
	* StaminaMax が変更された際に Stamina を Adjust していなかった不具合の修正
* v0.0.14
	* HUD 組み込み
	* 詳細は [./Documents/Add-HUD.md](./Documents/Add-HUD.md) を参照
* v0.0.15
	* AsyncTaskAttributeChanged 組み込み
	* 詳細は [./Documents/Add-AsyncTaskAttributeChanged.md](./Documents/Add-AsyncTaskAttributeChanged.md) を参照
* v0.0.16
	* EngineSubsystem 組み込み
	* 詳細は [./Documents/Add-EngineSubsystem.md](./Documents/Add-EngineSubsystem.md) を参照
* v0.0.17
	* ダメージの仕組みの組み込み
	* リスポーンの仕組みの組み込み
	* 詳細は [./Documents/Add-DamageSystem.md](./Documents/Add-DamageSystem.md) を参照
* v0.0.18
	* スタミナゲージの更新処理を誤ってスタミナ最大値にバインドしていたのを修正
	* Character に Sprint の Ability 組み込み
	* 詳細は [./Documents/Add-Sprint.md](./Documents/Add-Sprint.md) を参照
* v0.0.19
	* ダメージボリュームの秒間ダメージがテキストでは 15 だが実際は 20 だったので 15 に統一
	* FloatingStatusBar 組み込み
	* 詳細は [./Documents/Add-FloatingStatusBar.md](./Documents/Add-FloatingStatusBar.md) を参照
* v0.0.20
	* FloatingDamageText 組み込み
	* 詳細は [./Documents/Add-FloatingDamageText.md](./Documents/Add-FloatingDamageText.md) を参照
* v0.0.21
	* C++ 内のデフォルトクラスの指定を一箇所に集めた。
* v0.0.22
	* AbilityTask_PlayMontageAndWaitForEvent の追加
	* 詳細は [./Documents/Add-AbilityTask_PlayMontageAndWaitForEvent.md](./Documents/Add-AbilityTask_PlayMontageAndWaitForEvent.md) を参照
* v0.0.23
	* このファイルの更新履歴の v0.0.20 ～ v0.0.22 が数字が 1 小さく誤っていたのを修正（ v0.0.19 が二つになっていた）
	* Character に FireGun の Ability 組み込み
	* 詳細は [./Documents/Add-FireGun.md](./Documents/Add-FireGun.md) を参照
* v0.0.24
	* Character に Armor の Attribute 組み込み
	* 詳細は [./Documents/Add-Armor-Attribute.md](./Documents/Add-Armor-Attribute.md) を参照
* v0.0.25
	* AbilityTask_WaitReceiveDamage の追加
	* 詳細は [./Documents/Add-AbilityTask-WaitReceiveDamage.md](./Documents/Add-AbilityTask-WaitReceiveDamage.md) を参照
* v0.0.26
	* 過去のドキュメントの誤字修正
	* AsyncTaskEffectStackChanged の追加
	* 詳細は [./Documents/Add-AsyncTaskEffectStackChanged.md](./Documents/Add-AsyncTaskEffectStackChanged.md) を参照
* v0.0.27
	* 過去のドキュメントの誤字修正
	* Character に PassiveArmor の Ability 組み込み
	* 詳細は [./Documents/Add-PassiveArmor-Ability.md](./Documents/Add-PassiveArmor-Ability.md) を参照
* v0.0.28
	* v0.0.23 の不具合修正
		* FireGun で使用するモーションが短いため、 Blend In/Out の時間を 0 に変更
		* AnimBP にて Slot ノードがなかったためモンタージュが再生されていなかったのを修正。
	* Character に Dash の Ability 組み込み
	* 詳細は [./Documents/Add-Dash-Ability.md](./Documents/Add-Dash-Ability.md) を参照
* v0.0.29
	* 状態異常 Stun の組み込み
	* 詳細は [./Documents/Add-Stun-Condition.md](./Documents/Add-Stun-Condition.md) を参照
* v0.0.30
	* Character に Meteor の Ability 組み込み
	* 詳細は [./Documents/Add-Meteor-Ability.md](./Documents/Add-Meteor-Ability.md) を参照
* v0.0.31
	* Document の typo の修正
* v0.0.32
	* AimDownSights 後の CameraBoom の ArmLength を元の状態に戻す際の処理を修正
		* 直値 -> ヒーロークラスが持っている初期値を利用するように変更
	* HeroAI 組み込み
	* 詳細は [./Documents/Add-HeroAI.md](./Documents/Add-HeroAI.md) を参照
* v0.0.33
	* Projectile の不具合修正
		* 状況：ダメージでターゲットが死亡した場合。
		* 原因：AdditionalEffects で無効の AbilitySystemComponent を参照していた。
		* 対応：AdditionalEffects のループ処理の前に AbilitySystemComponent の Valid チェックをおこなように修正
		* 備考：AdditionalEffects でターゲットを死亡させる物があるのであれば、 AbilitySystemComponent の Valid チェック はループ内に移動すること。
	* Minion 組み込み
	* 詳細は [./Documents/Add-Minion.md](./Documents/Add-Minion.md) を参照
* v0.0.34
	* BP_GEVolumeBase のタイマーをクリアする処理の追加
	* RespawnCountdown 組み込み
	* 詳細は [./Documents/Add-RespawnCountdown.md](./Documents/Add-RespawnCountdown.md) を参照
* v0.0.35
	* Character に CharacterLevel / XP / XPBounty / Gold / GoldBounty の Attribute 組み込み
	* 詳細は [./Documents/Add-Level_XP_Gold-Attributes.md](./Documents/Add-Level_XP_Gold-Attributes.md) を参照

-----
おしまい。
