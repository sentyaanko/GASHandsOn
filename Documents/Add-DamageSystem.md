# Add-DamageSystem
ダメージを扱う仕組みの組み込み

# 方法

* `AttributeSet` に `Damage` アトリビュートを追加する。
* ダメージ計算を行う `GameplayEffectExecutionCalculation` 派生クラスを新規作成する。
* ダメージの確認用に、範囲内に居るとダメージを受けるボリューム `BP_GEVolume_Damage` を用意する。
* 死亡後のリスポーンの処理を `GameMode` に追加する。
* まだ扱わないことは以下の通り
	* `UGHOAttributeSetBase::PostGameplayEffectExecute_Damage()` で行う予定のこと
		* ダメージ時のモンタージュ再生
		* ダメージ値のフローティングテキストの表示
		* 死亡時のモンタージュ再生
		* 死亡した場合に死亡させたキャラクターに経験値やゴールドを増やす処理
	* `UGHOGEEC_Damage` で行う予定のこと
		* `Armor` アトリビュートによるダメージ軽減処理
		* ダメージのターゲットの `AbilitySystemComponent` に対して 軽減前後のダメージ値の `Broadcast` する
			* 軽減前後のダメージ値はユーザーに提示するには有用だが、 `GASDocumentation` では利用していない

# 手順

ソース類


1. `UGHOAttributeSetBase` に以下を追加
	* プロパティ
		* `Damage`
			* 他のアトリビュートと異なり、 `UPROPERTY` で `ReplicatedUsing = OnRep_StaminaRegenRate` の指定は行わない。
	* 関数
		* `PostGameplayEffectExecute_Damage()`
			* `PostGameplayEffectExecute()` で `Damage` が渡された際の処理。長いので関数に分割。
			* 渡された `Damage` をターゲットの `Health` から減じる。
			* 詳細はコード参照。
1. `UGHOGEEC_Damage` を `UGameplayEffectExecutionCalculation` から派生し新規作成
	* 概要
		* `GEEC` は `GameplayEffectExecutionCalcuration` の頭文字。
		* 詳細はコード参照。
1. `AGHOGameModeBase` に以下を追加
	* プロパティ
		* `RespawnDelay`
			* リスポーンまでの待機時間
		* `HeroClass`
			* リスポーンで構築するクラス
	* 関数
		* コンストラクタ
			* 追加した以下のプロパティの初期化
				* `RespawnDelay`
				* `HeroClass`
		* `HeroDied()`
			* 死亡時の処理。
			* `Controller` から現在の `Pawn` を `UnPossess` し、スペクターポーン新規にスポーンし、 `Possess` する。
			* `RespawnDelay` 秒後に `RespawnHero()` を呼び出す。
			* 詳しくはコード参照。
		* `RespawnHero()`
			* マップ上から `PlayerStart` を探し、その座標に `HeroClass` をスポーンし、 `Contorller` に `Possess` する。
			* 詳しくはコード参照。
1. `AGHOCharacterBase` の変更
	* 関数
		* `FinishDying()`
			* オーバーライドしたいので `virtual` に。
1. `AGHOHeroCharacterBase` の変更
	* 関数
		* `FinishDying()`
			* `ROLE_Authority` の場合は `GameMode` を取得し、 `AGHOGameModeBase::HeroDied()` を呼び出す。
			* 要は死亡時の処理でリスポーンの登録処理を呼び出す。



Editor での設定類。

GameplayTag の追加

| タグ名                        | 用途                                                                |
|-------------------------------|---------------------------------------------------------------------|
| `Data.Damage`                 | ダメージ用のメタ・アトリビュート                                    |


BP 類。

1. `GE_Damage` を `GameplayEffect` から派生し新規作成
	* `Gameplay Effect > Executions`  に要素を追加し、以下を指定
		* `Calculation Class` に `GHOGEE_Damage`
1. `BP_GEVolumeBase` を変更
	* 概要
		* ダメージ処理で `ApplyGameplayEffectSpecToSelf` を使用するが、そこ以外は `BP_GEVolumeBase` とほぼ同じ。
		* 流用できるようにリファクタリング。
	* `ApplyGameplayEffectToSelf` を呼び出していた箇所を派生クラスで買えられるように抽象化
	* 詳しくは `BluePrint` 参照。
1. `BP_GEVolumeApplyGEToSelf` を `BP_GEVolumeBase` から派生し新規作成
	* `BP_GEVolumeBase` で以前行っていた `ApplyGameplayEffectToSelf` の呼び出し実装
	* 詳しくは `BluePrint` 参照。
1. `BP_GEVolume_BuffMoveSpeed` を変更
	* 親クラスを `BP_GEVolumeApplyGEToSelf` に
	* 詳しくは `BluePrint` 参照。
1. `BP_GEVolume_Damage` を `BP_GEVolumeBase` から派生し新規作成
	* プロパティ
		* `Damage`
			* `AssignTagSetByCallerMagnitude` で指定するダメージ値を指定する変数。
		* `GameplayEffect`
			* 適用する `GameplayEffect` を指定する変数。 `GE_Damage` を指定する。
	* 詳しくは `BluePrint` 参照。


以上で、プレイヤーにダメージを与える仕組みと死亡時にリスポーンする仕組みが作成できます。


-----
おしまい。
