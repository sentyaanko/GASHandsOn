// (C) Sentya Anko 2021


#include "Engine/GHOEngineSubsystem.h"
#include "AbilitySystemGlobals.h"

/*
解説
	詳しくは
	* [「GASDocumentation」の「3. GAS を使用したプロジェクトの設定」](https://github.com/tranek/GASDocumentation#setup) [(和訳)](https://github.com/sentyaanko/GASDocumentation/blob/lang-ja/README.jp.md#setup)
	* [「GASDocumentation」の「4.9.1 InitGlobalData()」](https://github.com/tranek/GASDocumentation#concepts-asg-initglobaldata) [(和訳)](https://github.com/sentyaanko/GASDocumentation/blob/lang-ja/README.jp.md#concepts-asg-initglobaldata)
	* [「GASDocumentation」の「4.11.1 Target Data」](https://github.com/tranek/GASDocumentation#concepts-targeting-data) [(和訳)](https://github.com/sentyaanko/GASDocumentation/blob/lang-ja/README.jp.md#concepts-targeting-data)
	を参照。
*/
void UGHOEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UAbilitySystemGlobals::Get().InitGlobalData();
}
