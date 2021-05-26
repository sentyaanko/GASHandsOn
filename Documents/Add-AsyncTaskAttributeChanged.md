# Add-AsyncTaskAttributeChanged
`AsyncTaskAttributeChanged` を新規に作成し、アトリビュートの変更時の処理を `GUI` の実装時にしやすくする。
「エンジニアが `Attribute` を追加したが、変更コールバックを用意していない」というケースでも変更通知を受け取れるようになる。

# 方法

* `UBlueprintAsyncActionBase` の派生クラスを作る
	* 構築時に `AbilitySystemCopmponent` と 「 `Attribute` とデリゲートを含んだ構造体」の配列を受け取り、 `AbilitySystemCopmponent` に変更通知用のデリゲートとして登録する。
* `HUD` の `Construct` 時に変更時のデリゲートを指定して `Task` を作り、 `Destruct` 時に `Task` を破棄する。

# 手順

ソース類

1. `UGHOAsyncTaskAttributeChanged` を `UBlueprintAsyncActionBase` から派生して新規作成
	* 概要
		* `AbilitySystem` とはほとんど関係ないので、詳細については割愛します。
		* 詳細はソース参照


BP 類。

1. `WBP_HUD` に以下を追加
	* `Construct` で `ASyncTask` 作成
	* `Destruct` で `ASyncTask` 破棄
	* 詳細はグラフ参照

以上で、 `AbilitySystemCopmponent` が所持する `Attribute` の変更通知を `BluePrint` のみで処理できるようになります。


-----
おしまい。
