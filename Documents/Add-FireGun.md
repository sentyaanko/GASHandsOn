# Add-FireGun
投射物を生成してキャラクターにダメージを与えることができるアビリティの追加。

# 方法

* 投射物の `Actor` クラス作成（ C++ で基底、BluePrint で表示関連）
* 命中時の `ParticleEffect` の作成
* 命中時の `GameplayCue` の作成（ BluePrint ）
* 発砲時の `Montage` の作成
* 発砲用の `GameplayAbility` クラス作成（ C++ で基底、BluePrint で表示関連）
* ダメージ計算は以前作成したものを使用する。
* まだ扱わないことは以下の通り
	* `GASDocumentation` との違い
		* 銃をもたせ銃口から銃弾をスポーンしているが、本線から外れるので扱わない。
		* `Montage` は仮で、腰撃ちと `AimDownSights` で同じものを使う。

# 手順

ソース類
1. `enum EGHOAbilityInputID` （入力 ID 用）に以下の値を追加
	* `Ability1`
		* `FireGun` 用
1. `AGHOProjectile` を `AActor` から派生して新規作成
	* 概要
		* 投射物の基底クラス
		* 移動とダメージ処理と射程のためのメンバのみ持つ。
	* プロパティ
		* `Range`
			* 射程のための変数。
			* 実際の破棄は BluePrint 側で行う。
			* 初期化はこの `Actor` をスポーンする `GHOGA_FireGun` で行う。
		* `DamageEffectSpecHandle`
			* ダメージのための変数。
			* 実際のダメージ処理は BluePrint 側で行う。
			* 初期化はこの `Actor` をスポーンする `GHOGA_FireGun` で行う。
		* `ProjectileMovement`
			* 投射物の移動のためのコンポーネント `UProjectileMovementComponent`
	* 関数
		* コンストラクタ
			* レプリケーションさせるため、 `AActor::bReplicates` を `true` にする。
			* `ProjectileMovement` を初期化する。
1. `AGHOCharacterBase` と `AGHOHeroCharacterBase` の変更
	* 関数
		* `GetProjectileTransform()`
			* 投射物をスポーンさせる Transform を返す。
1. `UGHOGA_FireGun` を `UGHOGameplayAbility` から派生して新規作成
	* プロパティ
		* `FireHipMontage`
			* 腰撃ち時の発砲モンタージュ
		* `FireIronsightsMontage`
			* `AimDownSights` 時の発砲モンタージュ
		* `ProjectileClass`
			* スポーンさせる投射物クラス
		* `DamageGameplayEffect`
			* ダメージを適用する際に使用する `GameplayEffect`
		* `Range`
			* 飛距離
		* `Damage`
			* `Damage Execution Calculation` に渡すダメージ用パラメータ
	* 関数
		* `UGameplayAbility::ActivateAbility()`(override)
		* `OnCancelled()`
			* キャンセル用デリゲート関数
		* `OnCompleted()`
			* 完了用デリゲート関数
		* `EventReceived()`
			* イベントコールバック用デリゲート関数


Editor での設定類。

GameplayTag の追加

| タグ名                                  | 用途                                                                |
|-----------------------------------------|---------------------------------------------------------------------|
| `Event.Montage.EndAbility`              | `AnimNotify` のアビリティ終了通知                                   |
| `Event.Montage.SpawnProjectile`         | `AnimNotify` の投射物のスポーン通知                                 |
| `GameplayCue.Shared.FireGun.Impact`     | `GunProjectile` 命中時の `GameplayCue` の再生用                     |


Collision の追加
* `Object Channels` に `Projectile` をデフォルト応答 `Overrap` で追加


BP 類。

1. `BP_GunProjectile` を `AGHOProjectile` から派生して新規作成
	* コンポーネント
		* `Collision`
			* キャラクターとの重なりチェック用
		* `Sphere`
			* 表示用。
	* クラスデフォルトの設定
		* `BP_GunProjectile`
			* `GHOProjectile > Range` を `1000.0` に
		* `ProjectileMovement`
			* `プロジェクタイル > Initial Speed` を `7000.0` に
			* `プロジェクタイル > Projectile Gravity Scale` を `0.0` に
		* `Colision`
			* `シェイプ > Sphere Radius` を `16.0` に
			* `ナビゲーション > Area Class` を `NavArea_Obstacle` に
			* `コリジョン > コリジョンプリセット` を `Custom` に
			* `コリジョン > コリジョンプリセット > コリジョンレスポンス > トレース応答` の `Visibility` と `Camera` を `無視する` に
			* `コリジョン > コリジョンプリセット > コリジョンレスポンス > オブジェクト応答` の `Ability` と `Projectile` を `無視する` に、それ以外を `オーバーラップ` に
		* `Sphere`
			* `スタティックメッシュ > Static Mesh` を `Sphere` に
			* `コリジョン > コリジョンプリセット` を `NoCollision` に
			* `レンダリング > Override Materials` の 0 要素を `M_GunProjectile` に
	* イベントグラフ
		* `BeginPlay`
			* `ProjectileMovement` の `Initial Speed` と `GHOProjectile` の `Range` を元に `Life Span` の設定
		* `OnComponentBeginOverlap(Collision)`
			* 重なったオブジェクトに対して `GameplayEvent` の適用
1. `M_GunProjectile` を新規作成
	* 概要
		* `GunProjectile` 用のマテリアル。
1. `PS_GunProjectileImpact` を新規作成
	* 概要
		* `GunProjectile` が命中した際のエフェクト。
		* ループしないエフェクトがなかったので仮で作成。
1. `GC_GunProjectileImpact` を `GameplayCueNotify_Static` から派生して新規作成
	* クラスのデフォルトの設定
		* `GameplayCue > Gameplay Cue Tag` を `GameplayCue.Shared.FireGun.Impact` に
	* 関数
		* `OnExecute`
			* 渡された `HitResult` の座標で `GC_GunProjectileImpact` を再生。
1. `AN_GenericEventByTag` を `AnimNotify` から派生して新規作成
	* プロパティ
		* `EventTag`
			* `OwnerActor` に送る `GameplayTag`
	* イベントグラフ
		* `GetNotifyName`
			* 見やすいように `EventTag` を文字列化したものを返すようにする。
		* `ReceivedNotify`
			* `OwnerActor` に `EventTag` を送る。
1. `AM_FireGun` という名前で `AnimationMontage` を新規作成
	* 概要
		* `FireGun` 実行時に使用する `AnimationMontage`
		* `AN_GenericEventByTag` を利用して、タイムライン上の適切なタイミングで以下の通知をアクターに送る
			* `Event.Montage.SpawnProjectile` で投射物の生成
			* `Event.Montage.EndAbility` でアビリティの終了
		* アニメーションは暫定で `ThirdPerson_Jump` を使用する。
1. `GE_GunProjectileDamage` を `GameplayEffect` から派生して新規作成
	* クラスのデフォルトの設定
		* `Gameplay Effect > Executions` の要素を追加し、以下を指定
			* `Calculation Class` を `GHOGEEC_Damage` に
		* `表示 > Gameplay Cue` の要素を追加し、以下を指定
			* `Gameplay Cue Tags` を `GameplayCue.Shared.FireGun.Impact` に
1. `GA_FireGun` を `GHOGA_FireGun` から派生して新規作成
	* クラスのデフォルトの設定
		* `GHOGA FireGun > Fire Hip Montage` を `AM_FireGun` に
		* `GHOGA FireGun > Fire Ironsights Montage` を `AM_FireGun` に
		* `GHOGA FireGun > Projectile Class` を `BP_GunProjectile` に
		* `GHOGA FireGun > Damage Gameplay Effect` を `GE_GunProjectileDamage` に
		* `Ability > Ability Input ID` を `Ability1` に
		* `Ability > Ability ID` を `Ability1` に
1. `BP_HeroCharacterBase` に以下を追加
	* クラスのデフォルトの設定
		* `GASHands On > Abilities > Character Abilities` に要素を追加して `GA_FireGun` に
1. `UE4_Mannequin_Skelton` に以下を追加
	* `Head` の下に ソケットを追加し、名前を `Muzzle` に



-----
# 補足

-----
## ExposeOnSpawn
* 以下などを参照
	* [Unreal Engine 4 ドキュメント > プログラミングとスクリプト処理 > ゲームプレイのアーキテクチャ > プロパティ > プロパティ指定子](https://docs.unrealengine.com/ja/ProgrammingAndScripting/GameplayArchitecture/Properties/Specifiers/index.html)
	* [「PaperSloth’s diary」UE4 C++でのExposeOnSpawnについて](https://papersloth.hatenablog.com/entry/2018/04/13/232533)

-----
## SpawnActorDeferred
* 以下などを参照
	* [Unreal Engine 4 Documentation > Unreal Engine API Reference > Runtime > Engine > Engine > UWorld > UWorld::SpawnActorDeferred](https://docs.unrealengine.com/en-US/API/Runtime/Engine/Engine/UWorld/SpawnActorDeferred/index.html)
	* [「PaperSloth’s diary」E4 SpawnActorDeferredについて](https://papersloth.hatenablog.com/entry/2018/04/14/001607)


-----
おしまい。
