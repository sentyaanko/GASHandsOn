# GameplayPrediction

このファイルは
`Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/GameplayPrediction.h`
の冒頭に記載されたドキュメントを和訳したものです。
バージョンは 4.26.2 のものです。
4.27.1 でも変更はありません。

## 和訳

Gameplay Ability Prediction の概要

ハイレベルな目標:  
GameplayAbility レベル（アビリティの実装）では、 prediction （予測）は透明である（わかりやすい）。アビリティは 「 X->Y->Z を行え」と言い、可能な部分を自動的に predict （予測）します。
我々は、アビリティ自身に「もし Authority （権利者）ならば X を行い、そうでないならば X の predictive （予測）バージョンを行え」のようなロジックをもたせることを避けたい。

現時点では、全てのケースが解決したわけではありませんが、クライアントサイドの prediction （予測）を扱うための非常に強固なフレームワークができました。

「クライアントサイドの prediction （予測）」とは、ゲームシミュレーションのステートをクライアントが predicting （予測）することを意味します。 prediction （予測）システムと連動せず、「完全なクライアントサイド」にすることも可能です。
例えば、足音は完全にクライアントサイドであり、このシステムと連動しません。しかし、クライアントが、呪文を唱えた時にマナが 100 から 90 になると predicting （予測）するのは「クライアントサイドの prediction （予測）」です。

現在 predict （予測）しているものは何？
 - Ability activation （アビリティの発動）
 - イベントのトリガー
 - GameplayEffect の適用:
	- アトリビュートの変更	(例外: Executions は現在 predict （予測）されず、attribute modifiers のみ行う)
	- GameplayTag の変更
 - Gameplay Cue イベント ( predictive （予測）される GameplayEffect 内でのものと、単独のものがある)
 - モンタージュ
 - 移動 (UE4 の UCharacterMovement に組み込まれています。)


predict （予測）していないもの （このらの殆どは潜在的には可能ですが、現在は行っていません）:
 - GameplayEffect の除去
 - GameplayEffect の periodic effects (dots ticking)


我々が解決しようとする問題:
1. 「できるか？」 prediction （予測）のための基本プロトコル。
2. 「アンドゥ」 prediction （予測）が失敗したときの副作用をもとにもどす方法。
3. 「リドゥ」 ローカルで predicted （予測）した副作用が、サーバーからもレプリケーションされた場合に、それを再生しないようにする方法。
4. 「完全性」 全ての副作用を本当に predicted （予測）したかどうかを確認する方法。
5. 「依存」 依存性のある prediction （予測）と predicted （予測）されたイベントの連鎖を管理する方法。
6. 「上書き」 サーバーがレプリケーション/所有する状態を predictively （予測的）に上書きする方法。

---------------------------------------------------------

実装の詳細

***PredictionKey （予測キー）***

このシステムの基本的なコンセプトは、 Prediction Key (FPredictionKey) （予測キー）です。 prediction key （予測キー）はそれ自体はクライアントの内部で生成されるユニークな ID に過ぎません。クライアントは自身の prediction key （予測キー）をサーバーに送信し、 predictive （予測）アクションと副作用をこのキーに関連付けます。サーバーは prediction key （予測キー）の受け入れ/拒否で応答することができ、サーバー側で作成された副作用をこの prediction key （予測キー）に関連付けます。

（重要） FPredictionKey は常にクライアント -> サーバーにレプリケーションしますが、サーバー -> クライアントにレプリケーションする際には、最初に prediction key （予測キー）をサーバーに送信したクライアント **のみ** にレプリケーションします。
これは FPredictionKey::NetSerialize で発生します。クライアントから送られた prediction key （予測キー）がレプリケーションされたプロパティを介してレプリケーションされた場合、その他の全てのクライアントは無効な (0) の prediction key （予測キー）を受け取ります。


***Ability Activation （アビリティの発動）***

アビリティの発動は第一級の predictive （予測）行動です。クライアントが predictively （予測的）にアビリティを発動する時はいつでも、明示的にサーバーに質問し、サーバーが明示的に応答します。ひとたびアビリティが predictively （予測的）に発動すると、クライアントは有効な「 prediction window （予測ウィンドウ）」を持ち、そこで明示的に「質問」されていない predictive （予測的）な副作用が起こりうる。
（例えば、我々は「マナを減少させることができますか、このアビリティをクールダウンにすることはできますか？」とは明示的に尋ねません。これらの行為は、アビリティを発動することと論理的に矛盾しないと考えられています。）


AbilitySystemComponent はクライアントとサーバー間でアビリティの発動を通信するための関数群を提供します。: TryActivateAbility -> ServerTryActivateAbility ->  ClientActivateAbility(Failed/Succeed).

1. クライアントが TryActivateAbility を呼び出すと、新しい FPredictionKey が生成され、ServerTryActivateAbility が呼び出されます。
2. クライアントは (サーバーからの返事を聞く前に) 処理を続行し、 アビリティの ActivationInfo に関連付けられた、生成された PredictionKey をともに ActivateAbility を呼び出します。
3. ActivatAbility の呼び出しが終了する前に発生した副作用には、生成された FPredictionKey が関連付けられています。
4. サーバーは ServerTryActivateAbility でアビリティが本当に起こったのかどうかを判断し、 ClientActivateAbility(Failed/Succeed)  を呼び出し、 UAbilitySystemComponent::ReplicatedPredictionKey を送信された生成されたキーに設定します。
5. クライアントが ClientAbilityFailed を受け取った場合、直ちにアビリティをキルし、 prediction key （予測キー）に関連していた副作用をロールバックします。
	1. 「ロールバック」は FPredictionKeyDelegates と FPredictionKey::NewRejectedDelegate / NewCaughtUpDelegate / NewRejectOrCaughtUpDelegate を介して行われます。
	>
	>
	>
	>TryActivateAbility にコールバックを登録する:
	>```c++
	>// この PredictionKey （予測キー）が拒否された場合、 OnClientActivateAbilityFailed を呼び出します。
	>ThisPredictionKey.NewRejectedDelegate().BindUObject(this, &UAbilitySystemComponent::OnClientActivateAbilityFailed, Handle, ThisPredictionKey.Current);
	>```
	>ClientActivateAbilityFailed_Implementation におけるコールバックの呼び出し:
	>```c++
	>FPredictionKeyDelegates::BroadcastRejectedDelegate(PredictionKey);
	>```
6. 受け取った場合、クライアントはプロパティのレプリケーションが追いつくまで待たなくてはなりません（ Succeed RPC はすぐに送信され、プロパティのレプリケーションは勝手に行われます）。 ひとたび ReplicatedPredictionKey が前のステップで使用したキーに追いつくと、クライアントはクライアントは predictive （予測）による副作用をもとに戻すことができます。詳細は UAbilitySystemComponent::OnRep_PredictionKey を参照してください。



***GameplayEffect Prediction （ GameplayEffect 予測）***

GameplayEffects は prediction （予測）の副作用と見なされ、明示的に問われることはありません。

1. GameplayEffects は有効な prediction key （予測キー）がある場合のみクライアントに適用されます。（もし prediction key （予測キー）がない場合、クライアントへの適用は単にスキップされます）
2. GameplayEffect が predicted （予測）される場合、 Attributes 、GameplayCues 、 GameplayTags は全て predicted （予測）されます。
3. FActiveGameplayEffect が作成された時、 prediction key (FActiveGameplayEffect::PredictionKey) （予測キー）が保存されます。 
	1. Instant エフェクトについては以下の「Attribute Prediction （アトリビュート予測）」で説明します。
4. サーバーでは、同一の prediction key （予測キー）がサーバーの FActiveGameplayEffect にもセットされ、レプリケーションされます。
5. クライアントでは、有効な prediction key （予測キー）とともに FActiveGameplayEffect を受け取った場合、同じキーを持った ActiveGameplayEffect があるかを確認し、
もし一致したのならば、「 on applied 」タイプのロジックを適用しません、たとえば GameplayCues などの。これにより「リドゥ」問題が解決します。ただし、一時的に ActiveGameplayEffects コンテナに「同じ」 GameplayEffects が 2 つ存在することになります。 :
6. 同時に、 UAbilitySystemComponent::ReplicatedPredictionKey が追いつき、 predictive （予測）エフェクトが除去されます。
このケースで除去されると、 PredicitonKey を再びチェックし、 GameplayCue などの「 On Remove 」ロジックを実行しないべきかを判断します。

この時点で、副作用として GameplayEffect を効果的に predicted （予測）し、「アンドゥ」や「リドゥ」の問題を処理しました。


***Attribute Prediction （アトリビュート予測）***

アトリビュートは標準的な UProperty 群としてレプリケーションされるため、それらの変更の predicting （予測）は困難です（「上書き」問題）。瞬間的な変更はさらに困難です。なぜならアトリビュートは本来状態を持っていないからです。
（例えば、アトリビュートの変更をロールバックするのは過去の変更の履歴がないと困難です）。これは「アンドゥ」と「リドゥ」の問題も困難にします。

基本的な攻略プランは、アトリビュートの prediction （予測）を絶対値の prediction （予測）ではなく変化値の prediction （予測）として扱うことです。
「マナが 90 ある」と predict （予測）するのではなく、サーバーが我々の prediction key （予測キー）を確認するまでは、サーバーの値から「マナを -10 する」と predict （予測）するのです。
基本的に、「 instant modifications 」は、 predictively （予測的）に扱われている間は、「 infinite duration modifications 」としてアトリビュートに対して扱います。「アンドゥ」と「リドゥ」の解です。

「上書き」問題は、レプリケーションされた（サーバーの）値を「最終値」ではなく「基本値」として扱い、レプリケーションが起こった後に「最終値」を再計算することによって、我々はプロパティの OnRep でこれを対処することができます。


1. predictive （予測可能）な instant gameplay effects を infinite duration gamepaly effects として扱います。詳細は UAbilitySystemComponent::ApplyGameplayEffectSpecToSelf を参照してください。
2. 我々はアトリビュートの RepNotify 呼び出しを **常に** 受け取らなければなりません（最後のローカル力の変更があったときだけではありません、我々は事前に変更を predict （予測）するので）。 REPNOTIFY_Always で完了です。
3. アトリビュートの RepNotify では、 AbilitySystemComponent::ActiveGameplayEffects を呼び出して、「最終値」を更新し、新しい「基本値」を与えます。 GAMEPLAYATTRIBUTE_REPNOTIFY がこれを行う事ができます。
4. それ以外では、（ GameplayEffect prediction（予測） ）のように動作します : prediction key （予測キー）が追いついた時、 predictive （予測）された GameplayEffect は除去され、サーバーから与えられた値に戻ります。


例 :

```c++
void UMyHealthSet::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UMyHealthSet, Health, COND_None, REPNOTIFY_Always);
}

void UMyHealthSet::OnRep_Health()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyHealthSet, Health);
}
```

***Gameplay Cue Events***

既に説明した GameplayEffects の他に、 Gameplay Cues を単独で発動することができます。これらの機能（ UAbilitySystemComponent::ExecuteGameplayCue 等）はネットワークロールと prediction keys （予測キー）を考慮します。

1. UAbilitySystemComponent::ExecuteGameplayCue では、 Authority （権利者）ならイベントをマルチキャストします（レプリケーションキー付きで）。もし Authority （権利者）ではないが、有効な prediction key （予測キー）がある場合、 GameplayCue を predict （予測）します。
2. 受信側（ NetMulticast_InvokeGameplayCueExecuted 等）では、レプリケーションキーがあれば、イベントを実行しません（ predicted （予測）していたと推測します）。

FPredictionKeys は元のオーナーのみにレプリケーションされることに注意してください。これは FPredictionKeys の本質的な特性です。

***Triggered Data Prediction***

Triggered Data は現在、アビリティを発動するのに使用されています。基本的に、これは ActivateAbility と同じコードパスを通ります。アビリティは入力によって発動されるのではなく、別のゲームコード駆動型イベントから発動されます。クライアントは これらのイベントを predictively （予測的）に実行し、アビリティを predictively （予測的）に発動できます。

これにはいくつかのニュアンスがあります。サーバーがイベントをトリガーするコードを実行するためです。サーバーはクライアントからの連絡を待つだけではありません。サーバーは、 predictive （予測）されるアビリティから発動されたトリガーされたアビリティのリストを保持します。トリガーされたアビリティから TryActivate を受け取ると、サーバーは 「彼」が既にアビリティを実行したかを調べ、その情報を応答します。

Triggered Events とレプリケーションについてはまだ作業が残っています。（最後に説明）。

---------------------------------------------------------	

上級者向けのトピック！

***Dependencies （依存関係）***

このような状況があります「アビリティ X を発動し、即座にイベントが発生し、それがアビリティ Y を発動させ、それがアビリティ Z をトリガーする」。依存関係の連鎖は X->Y->Z となります。
これらのアビリティはサーバーによってリジェクトされる可能性があります。もし Y がリジェクトされた場合、 Z も起こらなかったことになりますが、サーバーでは Z を実行しようとしていないため、「 Z は実行できない」と判断することはありません。

これを処理するため、 FPredictionKey のメンバーに Base PredictionKey （ベース予測キー）という概念があります。 TryActivateAbility を呼び出す際に、現在の PredictionKey （予測キー）を渡します（該当する場合）。この prediction key （予測キー）は新しい prediction key （予測キー）を生成する際のベースとして使用されます。このようにしてキーの連鎖を構築し、 Y がリジェクトされた場合に Z を無効にすることができます。

しかし、これは少しニュアンスが違います。 X->Y->Z のケースでは、サーバーは X の PredictionKey （予測キー）を受け取ってから、自分で連鎖を実行しようとします。例えば、サーバーは、クライアントから送られてきた original prediction key （オリジナルの予測キー）を使って、 Y と Z を TryActivate します。一方、クライアントは、 TryActivateAbility を呼び出すたびに新しい PredictionKey （予測キー）を生成します。クライアントは各アビリティの発動毎に新しい PRedictionKey （予測キー） を生成 **しなければなりません** 、なぜなら各発動は論理的にアトミックではないからです。イベントの連鎖で生じる各副作用は、一意の PredictionKey （予測キー）を持たなければなりません。 X で生成された GameplayEffects が Z で生成されたのと同じ PredictionKey を持つことはできません。

これを回避するため、 X の prediction key （予測キー）は Y と Z のベースキーと見なされます。 Y から Z への依存関係はクライアントサイドで完全に保持され、それは FPredictionKeyDelegates::AddDependancy で行われます。 Y がリジェクト/確認された場合に Z をリジェクト/捕捉するためのデリゲートを追加します。

この依存システムにより、 1 つの prediction window/scope （予測ウィンドウ/スコープ）の中で論理的にアトミックではない複数の predictive （予測）アクションを持つことができます。



***Additional Prediction Windows (within an Ability) （追加の予測ウィンドウ（アビリティ内））***

前述のとおり、 prediction key （予測キー）は 1 つの論理スコープの中でのみ使用可能です。ActivateAbility が戻ってきたら、基本的にそのキーの使用は終了します。アビリティが外部のイベントやタイマーを待っている場合、それが戻ってくるまでに、サーバーから確認/リジェクトを得ていることになります。この後に発生するどんな副作用も、もはやオリジナルキーの寿命とは関係ありません。

これはそれほど悪いことではありません、アビリティがプレイヤーの入力に反応したくなる時を除いて。例えば、「長押ししてチャージする」アビリティは、ボタンが離された時に何かを瞬時に predict （予測）したいとします。 FScopedPredictionWindow を使って、アビリティの中に新しい prediction window （予測ウィンドウ）を作成することができます。

FScopedPredictionWindows は、サーバーに新しい prediction key （予測キー）を送信し、サーバーが同じ論理スコープ内でそのキーをピックアップして使用する方法を提供します。

UAbilityTask_WaitInputRelease::OnReleaseCallback は良い例です。イベントの流れは以下のとおりです:
1. クライアントは UAbilityTask_WaitInputRelease::OnReleaseCallback に入り、新しい FScopedPredictionWindow を開始します。これにより、このスコープの新しい prediction key （予測キー）が作成されます (FScopedPredictionWindow::ScopedPredictionKey) 。
2. クライアントは AbilitySystemComponent->ServerInputRelease を呼び出し、 ScopedPrediction.ScopedPredictionKey をパラメータとして渡します。
3. サーバーは ServerInputRelease_Implementation を実行し、渡された PredictionKey （予測キー）を受け取り、それを FScopedPredictionWindow として UAbilitySystemComponent::ScopedPredictionKey に設定します。
4. サーバーは UAbilityTask_WaitInputRelease::OnReleaseCallback を実行し、「同一のスコープで実行します」。
5. サーバーが ::OnReleaseCallback で FScopedPredictionWindow にヒットすると、 UAbilitySystemComponent::ScopedPredictionKey から prediction key （予測キー）を取得します。これで、この論理スコープ内の祖全ての副作用で使用されるようになります。
6. サーバーがこの scoped prediction window （スコープ付き予測ウィンドウ）を終えると、使用していた prediction key （予測キー）は終了し、 ReplicatedPredictionKey に設定されます。
7. これにて、このスコープで作成された全ての副作用は、クライアントとサーバーの間でキーを共有されます。

この動作の鍵は、 ::OnReleaseCallback が ::ServerInputRelease を呼び出し、それがサーバー上の ::OnReleaseCallback を呼び出すことにあります。他に何かが起こる余地がなく、与えられた prediction key （予測キー）を使用します。

この例では、「 Try/Failed/Succeed 」の呼び出しはありませんが、全ての副作用は手続き的にグループ化/アトミック化されています。これにより、サーバーとクライアントで実行される任意の関数呼び出しの「アンドゥ」と「リドゥ」問題が解決されます。


---------------------------------------------------------

Unsupported / Issues/ Todo

Triggered events は明示的にはレプリケーションされません。例えば、サーバー上のみで triggered event が実行される場合、クライアントはその事を知ることがありません。これにより、クロスプレイヤー/AI などのイベントを行うこともできません。これは、 GameplayEffect や GameplayCues と同じパターン（ triggered event を prediction key （予測キー）で　predict （予測）し、 prediction key （予測キー）を持っている場合は RPC イベントを無視する）でサポートされるべきです。


***Predicting "Meta" Attributes such as Damage/Healing vs "real" attributes such as Health***

メタアトリビュートを predictively （予測的）に適用することはできません。メタアトリビュートは GameplayEffect のバックエンドである instant effects のみで機能します（ UAttributeSet の Pre/Post Modify Attribute ）。これらのイベントは duration-based の gameplay effects 適用時には呼び出されません。例えば、  5 秒間ダメージを変更する GameplayEffect は意味がありません。

これらをサポートするには、 duration based なメタアトリビュートの限定的なサポートを追加し、 instant gameplay effect の変換をフロントエンド（ UAbilitySystemComponent::ApplyGameplayEffectSpecToSelf ）からバックエンド（ UAttributeSet::PostModifyAttribute ）に移すことになるでしょう。


***Predicting ongoing multiplicitive GameplayEffects （進行中の乗算的な GameplayEffects の Predicting （予測））***

また、 % ベースの gameplay effects を predicting （予測）する際にも制限があります。サーバーはアトリビュートの「最終値」をレプリケーションしますが、それを変更しているアグリゲーター連鎖全体はレプリケーションしないため、クライアントが新しい gameplay effects を正確に predict （予測）できない場合があります。

例: 

 - クライアントは永続的な +10% の移動速度のバフを持ち、基本移動速度が 500 -> 550 がこのクライアントの最終移動速度となります。
 - クライアントは追加で 10% の移動速度のバフを与えるアビリティを持ちます。 % ベースの倍率を **合算** することが期待されます。すなわち、  最後に 500 に 20% のボーナスを加え -> 600 の移動速度です。
 - しかしながら、クライアントでは、 550 に 10% のバフを適用し -> 605 となるだけです。

これは、アトリビュートのためのアグリゲーター連鎖をレプリケーションするように修正する必要があります。このデータの一部は既にレプリケーションされていますが、全ての modifier のリストはされていません。いずれ対応を検討する必要があります。


***"Weak Prediction （弱い予測）"***

おそらく、このシステムにうまく当てはまらないケースも出てくるでしょう。 prediction key （予測キー）の交換が実現できない状況もあるでしょう。例えば、プレイヤーが衝突/接触すると、それらを遅くし、それらのマテリアルを青くする GameplayEffect を受けるようなアビリティです。これが発生するたびにサーバー RPC を送ることはできないので（また、サーバーはシミュレーション内でその時点でメッセージ処理をできるとは限らないので）、クライアントとサーバーの間で gameplay effect の副作用を相関させる方法がありません。

ここでの1つのアプローチは、より弱い形の prediction （予測）を考えることかもしれません。
ひとつは生の prediction key （予測キー）を使用せず、代わりにサーバーが、クライアントが全体のアビリティの全ての副作用を predict （予測）すると仮定することです。
これにより、少なくとも「リドゥ」問題は解決しますが、「完全性」の問題が解決しません。
もしクライアント側の prediction （予測）を可能な限り小さくすることができれば - 例えば、状態やアトリビュートの変化を predicting （予測）するのではなく、初期のパーティクルエフェクトのみを predicting （予測）する - 、問題の深刻さは軽減されます。

副作用を正確に相関させることができる生の prediction key （予測キー）がない場合に、（特定のアビリティ？全てのアビリティ？を）フォールバックする、弱い prediction （予測）モードを思い描くことができます。
弱い prediction （予測）モードの時は、おそらく特定のアクションのみ predicted （予測）することができます - 例えば GameplayCue の execute events はできますが、 OnAdded/OnRemove events はできません。


-----
おしまい。
