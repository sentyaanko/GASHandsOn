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

-----
おしまい。
