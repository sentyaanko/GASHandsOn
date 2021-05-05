# Add-CharacterJump
キャラクターのジャンプ実装

# 方法

* 基本的には `CharacterMovementComponent` のジャンプ機能を利用する。
* ユーザーからの入力に対して直接呼び出すのではなく、 `AbilitySystemComponent` の入力バインドを経由するようにする。
	* そうすることで、 `GameplayTag` によるアビリティの実行制限（例：スタン中はジャンプ出来ない等）を実現する。

# 手順

1. 以下の `GameplayTag` の追加
	* `Ability.Jump`
		* ジャンプ用
	* `State.Dead`
		* ジャンプ不可のステータスの一つ。キャラクターの死亡判定用。ただしここではキャラクターの死亡は実装しない。
	* `State.CrowdControl.Stun`
		* ジャンプ不可のステータスの一つ。キャラクターのスタン判定用。ただしここではキャラクターのスタンは実装しない。
1. `enum EGHOAbilityInputID` （入力 ID 用）に以下の値を追加
	* `Jump`
		* ジャンプ用
1. `UGHOGameplayAbility` の変更
	* ~~`InstancingPolicy` （インスタンスポリシー）を `InstancedPerActor` （アクター毎）に変更~~
		* 今回は派生クラスで別の値に変更するので必須ではない
	* `ActivationBlockedTags` （有効化阻害タグ）に `State.Dead` と `State.CrowdControl.Stun` を追加
1. ジャンプ用のアビリティクラス `UGHOGA_CharacterJump` の実装
	* `InstancingPolicy` （インスタンスポリシー）を `NonInstanced` （インスタンス化しない）に変更
	* アビリティタグに `Ability.Jump` を指定
	* `AbilityInputID` に `Jump` を設定
		* 実際の `ASC` への関連付けは `UGHOCharacterBase::AddCharacterAbilities()` にて行っている。
	 * `UGameplayAbility` の仮想関数をオーバーライド。
		* 実装内容についてはエンジンソースの `UGameplayAbility_CharacterJump` とほぼ同じである。
			* `Engine\Plugins\Runtime\GameplayAbilities\Source\GameplayAbilities\Private\Abilities\GameplayAbility_CharacterJump.cpp`
			* `Engine\Plugins\Runtime\GameplayAbilities\Source\GameplayAbilities\Public\Abilities\GameplayAbility_CharacterJump.h`
		* 具体的には４つの関数。
			* `CanActivateAbility()`
			* `CancelAbility()`
			* `InputReleased()`
			* `ActivateAbility()`
				* `CommitAbility()` の失敗時の処理が異なる。
					* 元：そのまま `return` 。（ 2014 年ぐらいからこの状態であり、引用後に変わったなどではない模様）
					* 先：`EndAbility()` を呼び出し、処理を継続（初期のコミットからこの状態）
					* TODO: 
						* 変更理由については未調査。
						* `EndAbility()` の呼び出し理由については [GASDocumentation](https://github.com/sentyaanko/GASDocumentation/blob/lang-ja/README.jp.md#concepts-ga-definition) のフローチャートに従った結果と考えられる。
						* `return`しないようにしたのは謎。
							* **ジャンプにコストを追加し、 `CommitAbility()` の失敗時の動作を確認すること。**
1. `BP_HeroCharacterBase` の `CharacterAbilities` に `UGHOGA_CharacterJump` を追加する。

以上で `ASC` に入力をバインドし、そこから `CharacterMovementComponent` の機能を利用してキャラクターのジャンプを行うことができるようになります。

# 補足

手順で示した内容についての補足。
詳しくは割愛しますが、実装方法の確認の参考用に。

## `UGameplayAbility` のオーバーライドする仮想関数について

`Engine\Plugins\Runtime\GameplayAbilities\Source\GameplayAbilities\Public\Abilities\GameplayAbility.h`

ヘッダに説明があるので引用と訳。
オーバーライドしているものはコメント訳の先頭に「★」をつけときます。

```c++
/** Abilities define custom gameplay logic that can be activated by players or external game logic */
/** アビリティは、プレイヤーや外部のゲームロジックにより起動される、カスタムなゲームプレイのロジックを定義します */
UCLASS(Blueprintable)
class GAMEPLAYABILITIES_API UGameplayAbility : public UObject, public IGameplayTaskOwnerInterface
{
//...
	// ----------------------------------------------------------------------------------------------------------------
	//
	//	The important functions:
	//	
	//		CanActivateAbility()	- const function to see if ability is activatable. Callable by UI etc
	//								  ★アビリティが起動可能かどうかを確認するための const 関数です。 UI などで呼び出し可能です。
	//
	//		TryActivateAbility()	- Attempts to activate the ability. Calls CanActivateAbility(). Input events can call this directly.
	//								- Also handles instancing-per-execution logic and replication/prediction calls.
	//								  アビリティの起動を試みます。 CanActivateAbility() を呼び出します。 入力イベントはこれを直接呼び出し可能です。
	//								  また、「実行毎のインスタンス化」ロジックやレプリケーション/ prediction （予測）コールも処理します。
	//		
	//		CallActivateAbility()	- Protected, non virtual function. Does some boilerplate 'pre activate' stuff, then calls ActivateAbility()
	//								  protected の、非仮想関数です。いくつかの定型的な「起動前」の処理を行い、 ActivateAbility() を呼び出します。
	//
	//		ActivateAbility()		- What the abilities *does*. This is what child classes want to override.
	//								  ★アビリティが実行すること。これは小クラスがオーバーライドしたいものです。
	//	
	//		CommitAbility()			- Commits reources/cooldowns etc. ActivateAbility() must call this!
	//								  リソース/クールダウン等をコミットします。 ActivateAbility() はこれを呼び出す必要があります！
	//		
	//		CancelAbility()			- Interrupts the ability (from an outside source).
	//								  ★アビリティを（外部から）割り込み中断する。
	//
	//		EndAbility()			- The ability has ended. This is intended to be called by the ability to end itself.
	//								  アビリティの終了。これはアビリティ自身を終了させるために呼び出されることを意図しています。
	//	
	// ----------------------------------------------------------------------------------------------------------------
//...
};

```

-----
おしまい。


