# Add-Meteor-Ability
`Meteor` のアビリティの実装を行う。
`R` で地点を指定できるようになり、そのまま地点を指定するとその地点に隕石を落とす。
地形に着弾時に範囲内に居たキャラクターにダメージとスタンを与える。
（そのため、地面より上で直撃しても何も起きない。）


# 方法
* 新規で作るもの
	* `Cooldown` の変更を受け取る `AsyncTask`
* 使用する仕組み
	* `WaitTargetData`
		* `AbilityTask` として用意されている、地点を選択するための仕組み。
		* `Meteor` 実行時、投下地点を指定するために使用。
		* 使用時には選択中の地点を表示するための `Actor` を渡す必要がある。
	* `GameplayEffect`
		* コスト、クールダウン、スタン、ダメージ用の4つ
	* `GameplayAbility`
		* `Character` に登録する。上記の `GameplayEffect` が設定されている。
* まだ扱わないことは以下の通り
	* `Meteor` で表示は仮で `Sphere` を利用している。


# 手順

ソース類

1. `enum EGHOAbilityInputID` （入力 ID 用）に以下の値を追加
	* 詳細
		* `Meteor` 用の `Ability5` に加え、投下地点指定時に使用する `Confirm` と `Cancel` も追加します。
	* `Confirm`
		* `Meteor` のターゲット指定確定用
	* `Cancel`
		* `Meteor` のターゲット指定キャンセル用
	* `Ability5`
		* `Meteor` 用
1. `AGHOHeroCharacterBase` に、以下を追加する
	* 関数
		* `BindASCInput()`
			* `AbilitySystemComponent` の `BindAbilityActivationToInputComponent()` に渡す引数を変更する。
			* 具体的には `Confirm` と `Cancel` の `InputID` を指定するようにする。
			* [「 GASDocumentatation 」 の「4.6.2 Binding Input to the ASC （ASC に入力をバインド）」](https://github.com/sentyaanko/GASDocumentation/blob/lang-ja/README.jp.md#462-binding-input-to-the-asc-asc-%E3%81%AB%E5%85%A5%E5%8A%9B%E3%82%92%E3%83%90%E3%82%A4%E3%83%B3%E3%83%89) を参照。
1. `UGHOHUDWidget` に、以下を追加する
	* 関数
		* `ShowAbilityConfirmCancelText()`
			* 主に `Meteor` のアビリティの実行確認に用いる、 `HUD` に確認用のテキストの表示を On/Off するための関数。
			* `PlayerState` から呼び出される。
			* 実装は `Blueprint` で行う。
1. `AGHOPlayerState` に、以下を追加する
	* 関数
		* `ShowAbilityConfirmCancelText()`
			* 主に `Meteor` のアビリティの実行確認に用いる、 `HUD` に確認用のテキストの表示を On/Off するための関数。
			* `Blueprint` から呼び出される。
				* `GameplayAbility` -> `OwningActor` -> `PlayerState` -> `PlayerController` -> `HUD` と流す必要がある。
				* この関数が受け持つのは `PlayerState` -> `PlayerController` -> `HUD` の部分。
1. `UGHOAsyncTaskCooldownChanged` を `UBlueprintAsyncActionBase` から派生して新規作成
	* 概要
		* `GameplayEffect` の `Cooldown` を監視するタスク。
		* `Meteor` のクールダウンの表示に利用。
		* 詳細はコード参照。
	* プロパティ
		* `OnCooldownBegin` / `OnCooldownEnd`
			* エフェクトクールダウンの開始と終了のデリゲート
			* `ListenForCooldownChange` のアウトプットノードとなる。
	* 関数
		* `ListenForCooldownChange()`
			* イベントグラフのノードとなる関数。
		* `EndTask()`
			* 破棄関数。
		* `Initialize()`
			* 初期化関数。
			* `ListenForCooldownChange()` から呼び出される。
		* `OnActiveGameplayEffectAddedCallback()`
			* `AbilitySystemComponent` にクールダウン用のエフェクトが初めて足された際のデリゲートとして登録する関数。
		* `CooldownTagChanged()`
			* `AbilitySystemComponent` に指定した `GameplayTag` の追加と削除をされた際のデリゲートとして登録する関数。
		* `GetCooldownRemainingForTag()`
			* 指定された `GameplayTag` を持つアクティブな `GameplayEffect` を探し、最も残り時間が長いものの情報を得る



Editor での設定類。

GameplayTag の追加

| タグ名                        | 用途                                                                |
|-------------------------------|---------------------------------------------------------------------|
| `Ability.Skill.Ability5`      | アビリティ.スキル.Ability5中（ `Meteor` 用）                        |
| `Cooldown.Skill.Ability5`     | Ability5 のクールダウン用                                           |

入力設定の追加

| 入力名          | 割当       |
|-----------------|------------|
| `Ability5`      | R          |
| `ConfrimTarget` | 左クリック |
| `CancelTarget`  | 左Ctrl     |

コリジョンプリセットの追加

* 投下地点を指定する際に使用する。
* 以下を追加
	* `名前` を `TargetActorGroundLocation` に
	* `コリジョンEnabled` を `Query Only` に
	* `オブジェクトタイプ` を `Ability` に
	* `トレース型` をすべて `無視する` に
	* `オブジェクト型` を以下のように設定
		* `Pawn` と `Projectile` を `無視する` に
		* `Ability` を `オーバーラップ` に
		* それ以外を `ブロック` に




BP 類。
* `Test` マップに以下を設定
	* 詳細
		* `Meteor` のコリジョンイベントは地形に対して行うため、地形のコリジョン設定を変更する。
	* 地形用のスタティックメッシュ、具体的には以下
		* `Floor`
		* `Wall6`
		* `Wall7`
		* `Wall8`
		* `Wall9`
		* `Bump_StaticMesh`
		* `LeftArm_StaticMesh`
		* `Linear_Stair_StaticMesh`
		* `Ramp_StaticMesh`
		* `RightArm_StaticMesh`
		* `1M_Cube_Chamfer`
	* 以下を設定
		* `コリジョン > Generate Overlap Event` を `true` に
1. `GE_MeteorStun` を `GE_Stun` から派生して新規作成
	* クラスのデフォルトの設定
		* `Gameplay Effect > Duration Magnitude > Scalable Float Magnitude` を `2.0` に
1. `GE_MeteorDamage` を `GameplayEffect` から派生して新規作成
	* クラスのデフォルトの設定
		* `Gameplay Effect > Executions` に要素を追加して以下を指定
			* `Calculation Class` を `GHOGEEC_Damage` に
1. `GE_MeteorCost` を `GameplayEffect` から派生して新規作成
	* クラスのデフォルトの設定
		* `GameplayEffects > Modifiers` に要素を追加し、以下を指定
			* `Attribute` を `GHOAttributeSetBase.Mana` に
			* `Modifier Magnitude/Scalable Float Magnitude` を `-30.0` に
1. `GE_MeteorCoooldown` を `GameplayEffect` から派生して新規作成
	* クラスのデフォルトの設定
		* `タグ > GrantedTags` を `Cooldown.Ability.Ability5` に
		* `GameplayEffects > Duration Policy` を `HasD Duration` に
		* `GameplayEffects > Duration Magnitude > Scalable Float Magnitude` を `5.0` に
1. `M_MeteorTargetDecal` を新規作成
	* 概要
		* `BP_MeteorTargetActor` のデカール用のマテリアル。
		* 輪を描く。詳細はグラフ参照
1. `BP_MeteorTargetActor` を `GameplayAbilityTargetActor_GroundTrace` から派生して新規作成
	* コンポーネントの追加
		* `DefaultSceneRoot` の下に `Decal Component` を追加し、名前を `Decal` とする
			* 詳細設定
				* `デカール > Decal Materila` を `M_MeteorTargetDecal` に
				* `トランスフォーム > 回転` を `-180.0, 90.0, -180.0` に
				* `トランスフォーム > 拡大／縮小` を `10.0, 1.0, 1.0` に
1. `BP_MeteorProjectile` を `GHOProjectile` から派生して新規作成
	* コンポーネントの追加
		* `DefaultSceneRoot` の下に `Sphere Collision Component` を追加し、名前を `SphereCollision` とする
			* 詳細設定
				* `シェイプ Sphere Radius` を `250.0` に
				* `ナビゲーション >Area Class` を `NavArea_Obstacle` に
				* `コリジョン > コリジョンプリセット` を `Custom` に
				* `コリジョン > コリジョンプリセット > Object Type` を `Ability` に
				* `コリジョン > コリジョンプリセット > コリジョンレスポンス > トレース応答` をすべて `無視する` に
				* `コリジョン > コリジョンプリセット > コリジョンレスポンス > オブジェクト応答` を以下のように設定
					* `Projectile` と `Ability` を `無視する` に
					* それ以外を `オーバーラップ` に
				* `` を `` に
		* `DefaultSceneRoot` の下に `Sphere` を追加し、名前を `Sphere` とする（実際は `Static Mesh Component` ）
			* 詳細設定
				* `トランスフォーム > 拡大／縮小` を `5.0, 5.0, 5.0` に
				* `コリジョン > コリジョンプリセット` を `NoCollision` に
	* `Projectile Movement` の 詳細設定
		* `プロジェクタイル > Max Speed` を `2000.0` に
		* `プロジェクタイル > Projectile Gravity` を `0.0` に
		* `ベロシティ > Velocity` を `1.0, 0.0, -2000.0` に
	* イベントグラフ
		* `On Component Begin Overlap (SohereCollision)`
			* 地形と衝突したら、 `Collision` 内のキャラクターに対してダメージとスタンを与える。
			* `GASDocumentation` では変数を追加し、そこにスタン用の `EffectSpecHandle` をスポーン時に与えている。
			* `GASHandsOn` では `Stun` 実装時に `Projectile` の基底に追加の `GameplayEffect` を登録できるようにしているため、そちらを流用する。
1. `GA_Meteor_HeroBase` を `GHOGameplayAbility` から派生して新規作成
	* クラスのデフォルトの設定
		* `Ability > Ability Input ID` を `Ability5` に
		* `Ability > Ability ID` を `Ability5` に
		* `タグ > Ability Tags` を `Ability.Skill.Ability5` に
		* `タグ > Cancel Ability with Tag` を `Ability.AimDownSights` に
		* `タグ > Activation Owned Tags` を `Ability.Skill.Ability5` に
		* `タグ > Activation Blocked Tags` を `State.Dead` `State.CrowdControl.Stun` `Ability.AimDownSights` `Ability.Skill` に
		* `高度 > Server Respects Remove Ability Cancellation` を `false` に
		* `Costs > Cost Gameplay Effect Class` を `GE_MeteorCost` に
		* `Cooldowns > Cooldown Gameplay Effect Class` を `GE_MeteorCooldown` に
	* イベントグラフ
		* 詳細はグラフ参照
		* `ActivateAbility`
			* `CameraBoom` の `RelativeLocation` を上方向にずらす
			* ターゲット選択用の UI を表示する
			* `WaitTargetData` を利用して `Meteor` を落とす地点を指定する。
			* 正常に指定したら、その地点の上部に `Meteor` をスポーンさせる。
		* `OnEndAbility`
			* `CameraBoom` の `RelativeLocation` を戻す
			* ターゲット選択用の UI を消す
1. `BP_HeroCharacterBase` に以下を追加
	* プロパティ
		* `StartingCameraBoomTargetArmLength`
			* `CameraBoom` の `TargetArmLength` の初期値
		* `StartingCameraBoomRelativeLocation`
			* `CameraBoom` の `RelativeLocation` の初期値
	* クラスのデフォルトの設定
		* `コリジョン > コリジョンプリセット` の設定変更（ `Meteor` に直接関係はないです。 ）
		* `コリジョン > コリジョンプリセット` を `Custom` に
		* `コリジョン > コリジョンプリセット > コリジョンレスポンス > トレース応答 > Visibility` を `オーバーラップ` に
		* `コリジョン > コリジョンプリセット > コリジョンレスポンス > トレース応答 > Camera` を `無視する` に
		* `GASHands On > Abilities > Character Abilities` に要素を追加して `GA_Meteor_HeroBase` に
1. `WBP_HUD` に以下を追加
	* デザイナー
		* `WaitTargetData` 中の操作ヘルプの追加
		* `Meteor` の使用可能状態の表示
	* グラフ
		* `Construct`
			* 末尾に `Listen for CoolDown Change` の呼び出しを追加
		* `OnMeteorCooldownBegin` / `OnMeteorCooldownEnd`
			* `Meteor` のクールダウンの開始と終了通知。画面への表示の反映を行う
		* `Event Show Ability Confirm Cancel Text`
			* `WaitTargetData` 中の操作ヘルプの `On/Off` を行う。



以上で、 `Meteor` が実装されます。


-----
# 補足

-----
## WaitTargetData
[Unreal Engine 4 Documentation > Unreal Engine Blueprint API Reference > Ability > Tasks > Wait Target Data](https://docs.unrealengine.com/en-US/BlueprintAPI/Ability/Tasks/WaitTargetData/index.html)

>	Spawns target actor and waits for it to return valid data or to be canceled.
>	Target is Ability Task Wait Target Data

和訳
>	ターゲットアクタを生成し、有効なデータを返すか、キャンセルするまで待ちます。
>	ターゲットは `Ability Task Wait Target Data` です。

-----
## UAbilitySystemComponent

### RegisterGameplayTagEvent()

```c++
	/*
	by Epic
		Allow events to be registered for specific gameplay tags being added or removed
	和訳
		特定の GameplayTag の追加や削除をイベントとして登録できる。
	*/
	FOnGameplayEffectTagCountChanged& RegisterGameplayTagEvent(FGameplayTag Tag, EGameplayTagEventType::Type EventType=EGameplayTagEventType::NewOrRemoved);
```

### GetActiveEffectsTimeRemainingAndDuration()

```c++
	/*
	by Epic
		Gets both time remaining and total duration  for all effects that match query
	和訳
		クエリにマッチしたすべての effect の残り時間と総経過時間の両方を返します。
	*/
	TArray<TPair<float,float>> GetActiveEffectsTimeRemainingAndDuration(const FGameplayEffectQuery& Query) const;
```


-----
おしまい。
