# Add-AbilityTask-WaitReceiveDamage
ダメージを受けるのを待つ `AbilityTask` を追加します。

# 方法

* `UGHOAbilitySystemComponent` にデリゲートを持たせ、ダメージを受けた際に実行するようにする。
* `AbilityTask` を新規に作成し、デリゲートを持たせて Blueprint から設定できるようにし、 `UGHOAbilitySystemComponent` にはそれを呼び出す関数を登録するようにする。
* `UGHOGEEC_Damage` でダメージ算出後、ターゲットの `AbilitySystemComponent` に受けたダメージを知らせる。
* 処理の流れとしては以下のような感じ。
	* 初期化時
		* Blueprint でデリゲートを指定 -> `UGHOAbilityTask_WaitReceiveDamage` -> `UGHOAbilitySystemComponent`
	* ダメージ時
		* `UGHOGEEC_Damage` -> `UGHOAbilitySystemComponent` -> `UGHOAbilityTask_WaitReceiveDamage` -> Blueprint で設定したデリゲート

# 手順

## ソース類

1. `UGHOAbilitySystemComponent` に以下を追加
	* プロパティ
		* `ReceivedDamage`
			* ダメージを受けた際のデリゲート
	* 関数
		* `GetReceivedDamage()`
			* `ReceivedDamage` のアクセサ
		* `ReceiveDamage()`
			* `GHODamageExecCalculation` でダメージを受けたときに呼び出す。
			* `ReceivedDamage` に登録されたデリゲートにブロードキャストする。
1. `UGHOAbilityTask_WaitReceiveDamage` を `UAbilityTask` から派生して新規作成
	* プロパティ
		* `OnDamage`
			* Blueprint から設定する、ダメージを受けた際に呼び出すデリゲート
		* `bTriggerOnce`
			* Blueprint から設定する、デリゲートの呼び出しを一度のみにするかのフラグ
	* 関数
		* コンストラクタ
		* `WaitReceiveDamage()`
		* `Activate()`
			* `UGDAbilitySystemComponent::ReceivedDamage` に `UGHOAbilityTask_WaitReceiveDamage::OnDamageReceived()` を登録
		* `OnDestroy()`
			* `UGDAbilitySystemComponent::ReceivedDamage` から `UGHOAbilityTask_WaitReceiveDamage::OnDamageReceived()` の登録を解除
		* `OnDamageReceived()`
			* Blueprint から設定された `OnDamage` デリゲートを呼び出す。
			* `bTriggerOnce` が true ならばそのままタスクを止める。
1. `UGHOGEEC_Damage` に以下を追加
	* 関数
		* `Execute_Implementation()`
			* ターゲットの `AbilitySystemComponent` に受けたダメージを `ReceiveDamage()` で知らせる。


以上で、 Blueprint でダメージを受けるのを待つタスクを使用できるようになります。


-----
おしまい。
