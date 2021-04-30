# Add-CharacterHealth
キャラクターのヘルス実装

# 方法

* `AttributeSet` に `Health` を追加し、 `GameplayEffect` で初期化する。
* まだ扱わないことは以下の通り
	* 死亡時の `Montage` の再生
	* 各種 UI
	* 死亡を発生させる操作
	* キャラクターのレベル

# 手順

1. `UGHOAttributeSetBase` に以下を追加
	* マクロ
		* `ATTRIBUTE_ACCESSORS()` （アクセサ定義用）
	* プロパティ
		* `Health`
		* `MaxHealth`
	* 関数
		* コンストラクタ
			* 今回は用意のみ
		* `UObject::GetLifetimeReplicatedProps()` (override)
			* 追加した `Health` `MaxHealth` プロパティのレプリケーション設定用のマクロ呼び出し
		* `UAttributeSet::PostGameplayEffectExecute()` (override)
			* `Health` の変更を 0 から `MaxHealth` になるようにクランプ処理
		* `UAttributeSet::PreAttributeChange()` (override)
			* `MaxHealth` が変更された場合に `AdjustAttributeForMaxChange()` を利用して `Health` の調整
		* `GetCharacterLevel()`
			* 仮で常に 0 を返すようにしておく。 `CharacterLevel` の実装の際に対応する
    	* `InitializeAttributesOnSpawned()`
			* スポーン時のアトリビュートの初期化。
			* 具体的には `Health` を `MaxHealth` で初期化している。
    	* `AdjustAttributeForMaxChange()`
			* 現在地と最大値を持つプロパティの最大値を変更したときに、現在値が以前の最大値との比と変わらないように調整する
    	* `OnRep_Health()`
			* `GAMEPLAYATTRIBUTE_REPNOTIFY()` マクロを利用する
    	* `OnRep_MaxHealth()`
			* `GAMEPLAYATTRIBUTE_REPNOTIFY()` マクロを利用する
1. `AGHOPlayerState` に以下を追加
	* プロパティ
		* `HealthChangedDelegateHandle`
			* `Health` 変更時のデリゲートハンドル
		* `MaxHealthChangedDelegateHandle`
			* `MaxHealth` 変更時のデリゲートハンドル
	* 関数
		* `AActor::BeginPlay()` (override)
			* `HealthChangedDelegateHandle` の初期化
			* `MaxHealthChangedDelegateHandle` の初期化
		* `HealthChanged()`
			* `Health` 変更時のデリゲートの実体
			* `Health` 変更の結果死亡した場合、 `Pawn` に死亡を通知
			* 変更結果を UI に反映するのもここで行うが、今回は省略
		* `MaxHealthChanged()`
			* `MaxHealth` 変更時のデリゲートの実体
			* 変更結果を UI に反映するのもここで行うが、今回は省略
1. `UGHOAbilitySystemComponent` に以下を追加
	* プロパティ
		* `DeadTag`
			* `GameplayTag` のキャッシュ、死亡用
		* `EffectRemoveOnDeathTag`
			* `GameplayTag` のキャッシュ、死亡時にこのタグを持つ `GameplayEffect` を削除する際に使用
	* 関数
		* コンストラクタ
			* `GameplayTag` のキャッシュ `DeadTag` `EffectRemoveOnDeathTag` の作成
		* `InitializeAttributes()`
			* `AGHOCharacterBase::GetDefaultAttributes()` で取得した `GameplayEffect` を適用する
				* 意味としては `AttributeSet` の初期化
		* `ClearDead()`
			* `DeadTag` のカウントを 0 にする
		* `IsDead()`
			* `DeadTag` が含まれているかを返す
		* `Die()`
			* アビリティのキャンセル
			* `EffectRemoveOnDeathTag` で指定されたタグを含んでいる `GameplayEffect` を削除
			* `DeadTag` の追加
				* 詳しくは使用している API のリファレンス参照
1. `AGHOCharacterBase` に以下を追加
	* マクロ
		* `FCharacterDiedDelegate`
			* キャラクター死亡用デリゲート
	* プロパティ
		* `DefaultAttributes`
			* `AttributeSet` の初期化する `GameplayEffect` の設定用
		* `OnCharacterDied`
			* BP で設定する、キャラクター死亡時のデリゲートで、今回は使用していない
	* 関数
		* `GetAttributeSet()`
			* `AttributeSet` を取得する
			* 派生側でオーバーライドさせる前提で、このクラスでは `unimplemented()` を記述している
			* `UGHOAbilitySystemComponent::InitializeAttributes()`から利用する
		* `FinishDying()`
			* `Destroy()` の呼び出し
			* 死亡時のモンタージュ再生終了時にキャラクターを `Destroy()` するための関数だが、今回はモンタージュの対応をしていないので、死亡時にすぐ呼び出す
		* `GetDefaultAttributes()`
			* 外部から `DefaultAttributes` の取得用
			* `UGHOAbilitySystemComponent::InitializeAttributes()`から利用する
		* `Die()`
			* `CapsuleComponent` のコリジョン変更
			* `CharacterMovementComponent` の重力や速度の変更
			* `OnCharacterDied` のブロードキャスト
			* `UGHOAbilitySystemComponent::Die()` の呼び出し
			* `FinishDying()` の呼び出し
1. `` に以下を追加
	* 関数
		* `AGHOCharacterBase::GetAttributeSet()` (override)
			* `PlayerState` から取得しておいた `AttributeSet` を返す
		* `InitializeAfterAbilitySystem()`
			* `UGHOAbilitySystemComponent::ClearDead()` の呼び出し
			* `UGHOAttributeSetBase::InitializeAttributesOnSpawned()` の呼び出し
1. `` に以下を追加
	* プロパティ
	* 関数
1. `` に以下を追加
	* プロパティ
	* 関数
1. `GE_HeroAttributes` を新規作成
	* `GameplayEffects` の `Modifiers` に要素を追加死、以下を指定
		* `Attribute` に `GHOAttributeSetBase.MaxHealth`
		* `Modifier Op` に `Override`
		* `Modifier Magnitude/Magnitude Calculation Type` に `ScalableFloat`
		* `Modifier Magnitude/Scalable Float Magnitude` に `100`
1. `BP_HeroCharacterBase` の `DefaultAttributes` に `GE_HeroAttributes` を指定

以上で キャラクターのアトリビュートとして `Health` を持ち、 0 になると死亡するようになります。


-----
おしまい。
