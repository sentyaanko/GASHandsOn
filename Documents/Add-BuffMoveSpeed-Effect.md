# Add-BuffMoveSpeed-Effect
`MoveSpeed` にバフをかける `GameplayEffect` の追加

# 方法

* 10 秒間、 `MoveSpeed` が 1.5 倍になる `GameplayEffect` を作成する。
* `Volume` を作成し、その中にいるキャラクターに上記を 1 秒ごとに付与する。

# 手順

ソース類

1. `AGHOCharacterBase` に以下を追加
	* 関数
		* `IsAlive()`
			* `BluePrint` からの呼び出し用

Editor での設定類。

プロジェクト設定

1. `Collision` の編集
	* `Object Channels` に `Ability` をデフォルト応答 `Overrap` で追加

BP 類。

1. `GE_BuffMoveSpeed` を新規作成
	* `GameplayEffects > Duration Policy` を `HasD Duration` に
	* `GameplayEffects > Duration Magnitude > Scalable Float Magnitude` を `10.0` に
	* `GameplayEffects > Modifiers` に要素を追加し、以下を指定
		* `Attribute` を `GHOAttributeSetBase.MoveSpeed` に
		* `Modifier Op` を `Multiply` に
		* `Modifier Magnitude/Magnitude Calculation Type` を `ScalableFloat` に
		* `Modifier Magnitude/Scalable Float Magnitude` を `1.5` に
	* `タグ > GameplayEffectAssetTag > Added` を `Effect.RemoveOnDeath` に
	* `Stacking > Stacking Type` を `Aggregate by Target` に
	* `Stacking > Stack Limit Count` を `1` に
* `BP_GEVolumeBase` を `Actor` を基底に新規作成
	* `レプリケーション > Replicates` を `on` に
	* `Box Collision` コンポーネントの追加
		* `レンダリング > Hidden in Game` を `off` に
		* `コリジョン > コリジョンプリセット` を `Custom` に
		* `コリジョン > コリジョンプリセット > Object Type` を `Ability` に
		* `コリジョン > コリジョンプリセット > コリジョンレスポンス > トレース応答` をすべて `無視する` に
		* `コリジョン > コリジョンプリセット > コリジョンレスポンス > オブジェクト応答` を以下のように設定
			* `WorldStatic` を `ブロック` に
			* `Pawn` を `オーバーラップ` に
			* それ以外を `無視する` に
	* `TextRender` コンポーネントの追加
		* `Horizontal Alignment` を適当に（例： `Center`）
		* `World Size`  を適当に（例： `12.0`）
	* `Text` 変数の追加
		* `TextRender` のテキスト書き換え用
	* `TextColor` 変数の追加
		* `TextRender` のテキスト色書き換え用
	* `GameplayEffect` 変数の追加
		* `Volume` 内のキャラクターに適用する `GameplayEffect` 指定用
	* イベントグラフについてはそちらを参照
* `BP_GEVolume_BuffMoveSpeed` を `BP_GEVolumeBase` を基底に新規作成
	* `Text` を適当に（例： `MoveSpeed x 1.5 for 10 seconds.` ）
	* `TextColor` を適当に（例：黄色系）
	* `GameplayEffect` を `GE_BuffMoveSpeed` に



以上で MoveSpeed アトリビュートが追加され、移動速度のバフなどの実装時に利用できます。

-----
# 補足

`BP_GEVolumeBase` では、 1 秒毎に `Box` 内にいるキャラクターに対して `GE_BuffMoveSpeed` を付与しています。
重複して効果がかからないように `Stacking` の設定をしていますが、それ関係の設定項目については以下の通り。

-----
## EGameplayEffectStackingType
```c++
/** Enumeration for ways a single GameplayEffect asset can stack. */
/** １つの GameplayEffect アセットがスタックできる方法の列挙体 */
UENUM()
enum class EGameplayEffectStackingType : uint8
{
	/** No stacking. Multiple applications of this GameplayEffect are treated as separate instances. */
	/** スタックは出来ません。この GameplayEffect を複数回使用した場合、別のインスタンスとして扱われます。 */
	None,
	/** Each caster has its own stack. */
	/** キャスターごとにそれぞれのスタックを所持します。 */
	AggregateBySource,
	/** Each target has its own stack. */
	/** ターゲットごとにそれぞれのスタックを所持します。 */
	AggregateByTarget,
};
```

-----
## EGameplayEffectStackingDurationPolicy

```c++
/** Enumeration of policies for dealing with duration of a gameplay effect while stacking */
/** スタック中の GameplayEffect の持続時間に対処するためのポリシー列挙体 */
UENUM()
enum class EGameplayEffectStackingDurationPolicy : uint8
{
	/** The duration of the effect will be refreshed from any successful stack application */
	/** Effect の持続時間は、成功したスタックの申請から更新されます。 */
	RefreshOnSuccessfulApplication,

	/** The duration of the effect will never be refreshed */
	/** Effect の持続時間が更新されることはありません。 */
	NeverRefresh,
};
```

-----
## EGameplayEffectStackingPeriodPolicy

```c++
/** Enumeration of policies for dealing with the period of a gameplay effect while stacking */
/** スタック中の GameplayEffect の期間に対処するためのポリシー列挙体 */
UENUM()
enum class EGameplayEffectStackingPeriodPolicy : uint8
{
	/** Any progress toward the next tick of a periodic effect is discarded upon any successful stack application */
	/** 周期的な Effect の次の tick に向けての進行状況は、スタックの申請が成功した時点で破棄されます。 */
	ResetOnSuccessfulApplication,

	/** The progress toward the next tick of a periodic effect will never be reset, regardless of stack applications */
	/** 周期的な Effect の次の tick に向けての進行状況は、スタックの申請に関係なく、決してリセットされません。 */
	NeverReset,
};
```

-----
## EGameplayEffectStackingExpirationPolicy

```c++
/** Enumeration of policies for dealing gameplay effect stacks that expire (in duration based effects). */
/** 期限切れの GameplayEffect のスタックを処理するためのポリシー列挙体 */
UENUM()
enum class EGameplayEffectStackingExpirationPolicy : uint8
{
	/** The entire stack is cleared when the active gameplay effect expires  */
	/** アクティブな GameplayEffect が切れると、スタック全体がクリアされる。  */
	ClearEntireStack,

	/** The current stack count will be decremented by 1 and the duration refreshed. The GE is not "reapplied", just continues to exist with one less stacks. */
	/** 現在のスタック数が１減算され、デュレーションが更新されます。 GameplayEffect は「再適用」されず、スタック数が１つ減った状態で存在し続けます。 */
	RemoveSingleStackAndRefreshDuration,

	/** The duration of the gameplay effect is refreshed. This essentially makes the effect infinite in duration. This can be used to manually handle stack decrements via OnStackCountChange callback */
	/** GameplayEffect の持続時間が更新されます。これにより、 Effect の持続時間は実質的に無限になります。 OnStackCountChange コールバックでスタックの減少を手動で処理する場合に使用できます。 */
	RefreshDuration,
};
```

-----
## UGameplayEffect

```c++
/**
 * UGameplayEffect
 *	The GameplayEffect definition. This is the data asset defined in the editor that drives everything.
 *  This is only blueprintable to allow for templating gameplay effects. Gameplay effects should NOT contain blueprint graphs.
 *	GameplayEffect の定義。エディタで定義されたデータアセットで、全てを動かします。
 *  GameplayEffect のテンプレート化するためにブループリント化が可能なだけです。 GameplayEffect はグラフを含めることは出来ません。
 */
UCLASS(Blueprintable, meta = (ShortTooltip="A GameplayEffect modifies attributes and tags."))
class GAMEPLAYABILITIES_API UGameplayEffect : public UObject, public IGameplayTagAssetInterface
```

[「GASDocumentation」の「4.5 Gameplay Effects」](https://github.com/tranek/GASDocumentation#concepts-ge) [(和訳)](https://github.com/sentyaanko/GASDocumentation/blob/lang-ja/README.jp.md#concepts-ge) も参照。

### StackingType

```c++
	/** How this GameplayEffect stacks with other instances of this same GameplayEffect */
	/** どのようにこの GameplayEffect と同じ種類の GameplayEffect の他のインスタンスがスタックするか */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stacking)
	EGameplayEffectStackingType	StackingType;
```

### StackLimitCount

```c++
	/** Stack limit for StackingType */
	/** StackingType のスタック数制限 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stacking)
	int32 StackLimitCount;
```

### StackDurationRefreshPolicy

```c++
	/** Policy for how the effect duration should be refreshed while stacking */
	/** スタック時に硬化時間を更新する方法のポリシー */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stacking)
	EGameplayEffectStackingDurationPolicy StackDurationRefreshPolicy;
```

### StackPeriodResetPolicy

```c++
	/** Policy for how the effect period should be reset (or not) while stacking */
	/** スタック時に効果機関をリセットする（しない）方法に関するポリシー */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stacking)
	EGameplayEffectStackingPeriodPolicy StackPeriodResetPolicy;
```

### StackExpirationPolicy

```c++
	/** Policy for how to handle duration expiring on this gameplay effect */
	/** この GameplayEffect で期限切れとなる期間を動圧家のポリシー */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stacking)
	EGameplayEffectStackingExpirationPolicy StackExpirationPolicy;

```



-----
おしまい。
