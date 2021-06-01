# Add-Minion
リスポーン時に `HUD` 上にリスポーンまでのカウントダウンの表示を行う。


# 方法
* `GameMode` 内のキャラクター死亡処理の中で、死亡したキャラクターの `PlayerControler` クラスに対して Client への RPC を行う。
* そうすることで、死亡したキャラクターを操作していたユーザーの HUD のみカウントダウンタイマーの表示を行う。


# 手順

ソース類

1. `UGHOHUDWidget` に、以下を追加する
	* 関数
		* `SetRespawnCountdown()`
			* 実装は Blueprint で行う。
1. `AGHOPlayerController` に、以下を追加する
	* 関数
		* `SetRespawnCountdown()`
			* `GameMode` から `Client` への RPC 呼び出しをするための関数。
			* 所持している `HUD` に対して `SetRespawnCountdown()` の呼び出しを行う。
1. `AGHOGameModeBase` に、以下を追加する
	* 関数
		* `HeroDied()`
			* 死亡したキャラクターの `AGHOPlayerController` に対して `SetRespawnCountdown()` の `Client` への RPC 呼び出しをする


BP 類。

1. `WBP_HUD` に、以下を追加する
	* デザイナー
		* カウントダウン用のテキストブロックを追加する
	* グラフ
		* `SetRespawnCountdown`
			* タイマーイベントを利用し、指定された時間、カウントダウンを行う。
			* 詳細はグラフ参照。



以上で、 リスポーンカウントダウンタイマーが実装されます。


-----
おしまい。
