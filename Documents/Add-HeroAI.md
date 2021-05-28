# Add-HeroAI
`Hero` の `AI` 、つまりはプレイヤーが操作しない `Hero` を用意する。
レベルに初期配置用の `Hero` とリスポーン用の `Actor` を用意し、
`Hero` が死亡したらリスポーン用の `Actor` の座標からリスポーンするようにする。


# 方法
* AI コントローラの新規作成
* ヒーローキャラクターの基底クラスのデフォルト AI コントローラで上記を指定
* レベルに初期配置用の `Hero` の配置
* 上記と同じ座標にリスポーン用の `Actor` を配置
* GameMode の `BeginPlay()` 内でリスポーン用の `Actor` の情報取得
* GameMode 内で `PlayerController` ではないヒーローのリスポーン処理実装
* まだ扱わないことは以下の通り
	* ビヘイビアツリーによる制御。（ `GASDocumentation` でも扱っていないので割愛）


# 手順

ソース類

1. `AGHOHeroAIController` を `AAIController` から派生して新規作成
	* 関数
		* `コンストラクタ`
			* `bWantsPlayerState` を `true` に
				* AI に `PlayerState` を持たせるための指定。
				* [参考](https://answers.unrealengine.com/questions/62160/blueprint-do-aicontrollers-characters-have-players.html)
				* `Hero` の `AttributeSet` は `PlayerState` が所持しているのこともあって、必須。
1. `AGHOHeroCharacterBase` で以下を追加
	* 関数
		* `コンストラクタ`
			* `AIControllerClass` を `AGHOHeroAIController::StaticClass()` で初期化

1. `AGHOGameModeBase` で以下を追加
	* プロパティ
		* `EnemySpawnPoint`
			* レベル上に配置された、敵をスポーンさせる位置に置いたアクターへのポインタを所持する変数。
	* 関数
		* `BeginPlay()`
			* レベル内の `Actor` を走査し `EnemyHeroSpawn` という名前のアクタを探し、 `EnemySpawnPoint` に保存しておく。
		* `RespawnHero()`
			* `AI` 制御の `Hero` を `EnemySpawnPoint` の座標にスポーンさせる。


Editor での設定類。


BP 類。
* `Test` マップに以下を設定
	* 以下を配置
		* `BP_DefaultHero` を配置
			* 名前を `Villain` に（特に意味はない）
			* 座標を `1M_Cube_Chamfer` の上に、回転を `0.0, 0.0, 180.0` に（特に意味はない）
		* `空のアクタ` を配置
			* 名前を `EnemyHeroSpawn` に
			* 座標と回転を `BP_DefaultHero` とおなじになるように


以上で、 `Hero` 用の `AI` が用意でき、プレイヤーが操作しない `Hero` が配置でき、リスポーンされるようになります。


-----
おしまい。
