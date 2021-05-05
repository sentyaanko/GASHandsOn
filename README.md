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

-----
おしまい。
