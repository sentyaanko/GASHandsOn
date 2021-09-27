// (C) Sentya Anko 2021


#include "Player/GHOPlayerController.h"
#include "Player/GHOPlayerState.h"
#include "AbilitySystemComponent.h"
#include "UI/GHOHUDWidget.h"
#include "UI/GHOHUDReticle.h"
#include "Characters/AttributeSets/GHOAttributeSetBase.h"
#include "Characters/GHOCharacterBase.h"
#include "Settings/GHODefaultClasses.h"


AGHOPlayerController::AGHOPlayerController()
{
	UIHUDWidgetClass = FGHODefaultClasses::GetHUDWidgetClass();
	UIFloatingDamageTextComponentClass = FGHODefaultClasses::GetFloatingDamageTextComponentClass();
}

void AGHOPlayerController::CreateHUD()
{
	if (UIHUDWidget)
	{
		return;
	}
	if (!UIHUDWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing UIHUDWidgetClass. Please fill in on the Blueprint of the PlayerController."), *FString(__FUNCTION__));
		return;
	}
	/*
	by GASDocumentation
		Only create a HUD for local player
	和訳
		local player のみ HUD を作成する
	*/
	if (!IsLocalPlayerController())
	{
		return;
	}

	/*
	by GASDocumentation
		Need a valid PlayerState to get attributes from
	和訳
		アトリビュートを取得するために有効な PlayerState が必要
	*/
	if (AGHOPlayerState* playerState = GetPlayerState<AGHOPlayerState>())
	{
		if (UGHOAttributeSetBase* attr = Cast<UGHOAttributeSetBase>(playerState->GetAttributeSetBase()))
		{
			UIHUDWidget = CreateWidget<UGHOHUDWidget>(this, UIHUDWidgetClass);
			UIHUDWidget->AddToViewport();

			/*
			by GASDocumentation
				Set attributes
			和訳
				アトリビュートの設定
			*/
			UIHUDWidget->SetParamter(attr->GetHUDParameter());
		}
	}
}

void AGHOPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	/*
	by GASDocumentation
		For edge cases where the PlayerState is repped before the Hero is possessed.
	和訳
		Hero の possessed が呼ばれる前に PlayerState が Rep される エッジケース対策
	*/
	CreateHUD();

}

void AGHOPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (AGHOPlayerState* playerState = GetPlayerState<AGHOPlayerState>())
	{
		playerState->GetAbilitySystemComponent()->InitAbilityActorInfo(playerState, aPawn);
	}
}

void AGHOPlayerController::SetEquippedWeaponPrimaryIconFromSprite(UPaperSprite* InSprite)
{
	if (UIHUDWidget)
	{
		UIHUDWidget->SetEquippedWeaponSprite(InSprite);
	}
}

void AGHOPlayerController::SetEquippedWeaponName(const FText& WeaponName)
{
	if (UIHUDWidget)
	{
		UIHUDWidget->SetEquippedWeaponName(WeaponName);
	}
}

void AGHOPlayerController::SetEquippedWeaponStatusText(const FText& StatusText)
{
	if (UIHUDWidget)
	{
		UIHUDWidget->SetEquippedWeaponStatusText(StatusText);
	}
}

void AGHOPlayerController::SetPrimaryAmmoType(const FGameplayTag& AmmoType)
{
	if (UIHUDWidget)
	{
		UIHUDWidget->SetPrimaryAmmoType(AmmoType);
	}
}

void AGHOPlayerController::SetPrimaryClipAmmo(int32 ClipAmmo)
{
	if (UIHUDWidget)
	{
		UIHUDWidget->SetPrimaryClipAmmo(ClipAmmo);
	}
}

void AGHOPlayerController::SetPrimaryReserveAmmo(int32 ReserveAmmo)
{
	if (UIHUDWidget)
	{
		UIHUDWidget->SetPrimaryReserveAmmo(ReserveAmmo);
	}
}

void AGHOPlayerController::SetSecondaryAmmoType(const FGameplayTag& AmmoType)
{
	if (UIHUDWidget)
	{
		UIHUDWidget->SetSecondaryAmmoType(AmmoType);
	}
}

void AGHOPlayerController::SetSecondaryClipAmmo(int32 SecondaryClipAmmo)
{
	if (UIHUDWidget)
	{
		UIHUDWidget->SetSecondaryClipAmmo(SecondaryClipAmmo);
	}
}

void AGHOPlayerController::SetSecondaryReserveAmmo(int32 SecondaryReserveAmmo)
{
	if (UIHUDWidget)
	{
		UIHUDWidget->SetSecondaryReserveAmmo(SecondaryReserveAmmo);
	}
}

void AGHOPlayerController::SetHUDReticle(TSubclassOf<UGHOHUDReticle> ReticleClass)
{
	//by GASShooter
	//	!GetWorld()->bIsTearingDown Stops an error when quitting PIE while targeting when the EndAbility resets the HUD reticle
	//和訳
	//	"!GetWorld()->bIsTearingDown" は EndAbility で HUD レティクルをリセットする時、ターゲティング中に PIE を終了する時にエラーが発生するのを止める。
	if (UIHUDWidget && GetWorld() && !GetWorld()->bIsTearingDown)
	{
		UIHUDWidget->SetReticle(ReticleClass);
	}
}

void AGHOPlayerController::RPCClientShowDamageText_Implementation(float DamageAmount, EGHOFloatingTextType FloatingTextType, AGHOCharacterBase* TargetCharacter)
{
	if (TargetCharacter && UIFloatingDamageTextComponentClass)
	{
		UGHOFloatingDamageTextComponent* FloatingDamageTextComponent = NewObject<UGHOFloatingDamageTextComponent>(TargetCharacter, UIFloatingDamageTextComponentClass);
		FloatingDamageTextComponent->RegisterComponent();
		FloatingDamageTextComponent->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		FloatingDamageTextComponent->SetDamage(DamageAmount, FloatingTextType);
	}
}

void AGHOPlayerController::SetRespawnCountdown_Implementation(float RespawnTimeRemaining)
{
	if (UIHUDWidget)
	{
		UIHUDWidget->SetRespawnCountdown(RespawnTimeRemaining);
	}
}

