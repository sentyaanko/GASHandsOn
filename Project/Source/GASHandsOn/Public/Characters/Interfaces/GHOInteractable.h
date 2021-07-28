// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "GHOInteractable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGHOInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
by GASShooter
	Interface for Actors that can be interacted with through the GameplayAbilitySystem.
�a��
	GameplayAbilitySystem ��ʂ��ăC���^���N�g���邱�Ƃ��ł���A�N�^�[�̂��߂̃C���^�[�t�F�C�X�B
*/
class GASHANDSON_API IGHOInteractable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/**
	by GASShooter
		Is this object available for player interaction at right now?
	
		@param InteractionComponent UPrimitiveComponent in case an Actor has many separate interactable areas.
	�a��
		���̃I�u�W�F�N�g�͌��݃v���C���[������ł����Ԃɂ��邩�H

		@param InteractionComponent UPrimitiveComponent �̓A�N�^�[�������̓Ɨ������C���^���N�g�\�ȃG���A�������Ă���ꍇ�Ɏg�p���܂��B
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	bool IsAvailableForInteraction(UPrimitiveComponent* InteractionComponent) const;
	virtual bool IsAvailableForInteraction_Implementation(UPrimitiveComponent* InteractionComponent) const;

	/**
	by GASShooter
		How long does the player need to hold down the interact button to interact with this?
		
		@param InteractionComponent UPrimitiveComponent in case an Actor has many separate interactable areas.
	�a��
		�v���C���[�͂ǂ̂��炢�̎��ԁA�C���^���N�g�{�^���������Ă���΁A����ƃC���^���N�g���ł��邩�H

		@param InteractionComponent UPrimitiveComponent �̓A�N�^�[�������̓Ɨ������C���^���N�g�\�ȃG���A�������Ă���ꍇ�Ɏg�p���܂��B
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	float GetInteractionDuration(UPrimitiveComponent* InteractionComponent) const;
	virtual float GetInteractionDuration_Implementation(UPrimitiveComponent* InteractionComponent) const;

	/**
	by GASShooter
		Should we sync and who should sync before calling PreInteract()? 
		Defaults to false and OnlyServerWait.
		OnlyServerWait - client predictively calls PreInteract().
		OnlyClientWait - client waits for server to call PreInteract(). 
		                 This is useful if we are activating an ability on another ASC (player) and want to sync actions or animations with our Interact Duration timer.
		BothWait - client and server wait for each other before calling PreInteract().
		
		Player revive uses OnlyClientWait so that the player reviving is in sync with the server since we can't locally predict an ability run on another player. 
		The downed player's reviving animation will be in sync with the local player's Interact Duration Timer.
		
		@param InteractionComponent UPrimitiveComponent in case an Actor has many separate interactable areas.
	�a��
		PreInteract() ���Ăяo���O�ɁA�������ׂ����A�ƁA�N���������ׂ����B
		�f�t�H���g�ł́A false �� OnlyServerWait �ł��B
		OnlyServerWait - �N���C�A���g�� predictively �i�\���G�j�� PreInteract() ���Ăяo���܂��B
		OnlyClientWait - �N���C�A���g�́A�T�[�o�[�� PreInteract() ���Ăяo���̂�҂��܂��B
		                 ����́A���� ASC(player) �̃A�r���e�B���N�����ɁA�A�N�V������A�j���[�V������ Interact Duration �^�C�}�[�Ɠ������������ꍇ�ɕ֗��ł��B
		BothWait - �N���C�A���g�ƃT�[�o�[�́APreInteract() ���Ăяo���O�ɁA���݂��ɑҋ@���܂��B

		�v���C���[�̑h���ɂ� OnlyClientWait ���g�p���A�h������v���C���[���T�[�o�[�Ɠ�������悤�ɂ��Ă��܂��B���̃v���C���[�����s�����A�r���e�B�� locally predict �i���[�J���\���j�ł��Ȃ����߂ł��B
		�_�E�������v���C���[�̑h���A�j���[�V�����́A���[�J���v���C���[�� Interact Duration �^�C�}�[�ɓ������܂��B

		@param InteractionComponent UPrimitiveComponent �̓A�N�^�[�������̓Ɨ������C���^���N�g�\�ȃG���A�������Ă���ꍇ�Ɏg�p���܂��B
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void GetPreInteractSyncType(bool& bShouldSync, EAbilityTaskNetSyncType& Type, UPrimitiveComponent* InteractionComponent) const;
	virtual void GetPreInteractSyncType_Implementation(bool& bShouldSync, EAbilityTaskNetSyncType& Type, UPrimitiveComponent* InteractionComponent) const;

	/**
	by GASShooter
		Should we sync and who should sync before calling PostInteract()? 
		Defaults to false and OnlyServerWait.
		OnlyServerWait - client predictively calls PostInteract().
		OnlyClientWait - client waits for server to call PostInteract().
		BothWait - client and server wait for each other before calling PostInteract().
		
		Player revive uses OnlyServerWait so that the client isn't stuck waiting for the server after the Interaction Duration ends.
		Revive's PostInteract() will only run code on the server so it's fine for the client to be "finished" ahead of the server.
		
		@param InteractionComponent UPrimitiveComponent in case an Actor has many separate interactable areas.
	�a��
		PostInteract() ���Ăяo���O�ɁA�������ׂ����A�ƁA�N���������ׂ����B
		�f�t�H���g�ł́A false �� OnlyServerWait �ł��B
		OnlyServerWait - �N���C�A���g�� predictively �i�\���G�j�� PostInteract() ���Ăяo���܂��B
		OnlyClientWait - �N���C�A���g�́A�T�[�o�[�� PostInteract() ���Ăяo���̂�҂��܂��B
		BothWait - �N���C�A���g�ƃT�[�o�[�́APostInteract() ���Ăяo���O�ɁA���݂��ɑҋ@���܂��B

		�v���C���[�̑h���ɂ� OnlyServerWait ���g�p���A Interaction Duration ���I��������ŃN���C�A���g���T�[�o�[��҂��Ȃ��悤�ɂ��Ă��܂��B
		�h���� PostInteract() �́A�T�[�o�[��ł̂݃R�[�h�����s����i�悤�Ɏ������Ă���j�̂ŁA�N���C�A���g���T�[�o�[����Ɂu�I���v���Ă���肠��܂���B

		@param InteractionComponent UPrimitiveComponent �̓A�N�^�[�������̓Ɨ������C���^���N�g�\�ȃG���A�������Ă���ꍇ�Ɏg�p���܂��B
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void GetPostInteractSyncType(bool& bShouldSync, EAbilityTaskNetSyncType& Type, UPrimitiveComponent* InteractionComponent) const;
	void GetPostInteractSyncType_Implementation(bool& bShouldSync, EAbilityTaskNetSyncType& Type, UPrimitiveComponent* InteractionComponent) const;

	/**
	by GASShooter
		Interact with this Actor. 
		This will call before starting the Interact Duration timer. 
		This might do things, apply (predictively or not) GameplayEffects, trigger (predictively or not) GameplayAbilities, etc.
		
		You can use this function to grant abilities that will be predictively activated on PostInteract() to hide the AbilitySpec replication time.
		
		If you want to do something predictively, you can get the ASC from the InteractingActor and use its ScopedPredictionKey.
		
		Player revives use PreInteract() to trigger a ability that plays an animation that lasts the same duration as the Interact Duration. 
		If this ability finishes, it will revive the player in PostInteract().
		
		@param InteractingActor The Actor interacting with this Actor. It will be the AvatarActor from a GameplayAbility.
		@param InteractionComponent UPrimitiveComponent in case an Actor has many separate interactable areas.
	�a��
		���̃A�N�^�[�ƃC���^���N�g���܂��B
		���̊֐��� Interact Duration �^�C�}�[���J�n����O�ɌĂяo����܂��B
		���̊֐��ł́A (predictively �i�\���\�j�܂��͂����ł͂Ȃ�) GameplayEffects �̓K�p�A (predictively �i�\���\�j�܂��͂����ł͂Ȃ�) GameplayAbilities �̋N���Ȃǂ̏������s���܂��B
		
		���̊֐����g�p���āA PostInteract() �� predictively �i�\���I�j�ɃA�N�e�B�u�������A�r���e�B��t�^���āA AbilitySpec �̃��v���P�[�V�������Ԃ��B�����Ƃ��ł��܂��B
		
		predictively �i�\���I�j�ɉ������s�������ꍇ�́A InteractingActor ���� ASC ���擾���A �X�R�[�v�t�� Prediction �i�\���j�L�[ ���g�p���܂��B 
		
		�v���C���[�̑h���ł́A PreInteract() ���A Interact Duration �Ɠ������ԑ����A�j���[�V�������Đ�����A�r���e�B�̋N���Ɏg�p���Ă��܂��B
		���̃A�r���e�B���I������ƁA PostInteract() �Ńv���C���[��h�������܂��B
		Player revives use PreInteract() to trigger a ability that plays an animation that lasts the same duration as the Interact Duration.
		If this ability finishes, it will revive the player in PostInteract().
		
		@param InteractingActor ���̃A�N�^�[�ƃC���^���N�g����A�N�^�[�B����� GameplayAbility �� AvatarActor �ɂȂ�܂��B
		@param InteractionComponent UPrimitiveComponent �̓A�N�^�[�������̓Ɨ������C���^���N�g�\�ȃG���A�������Ă���ꍇ�Ɏg�p���܂��B
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void PreInteract(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent);
	virtual void PreInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) {};

	/**
	by GASShooter
		Interact with this Actor. 
		This will call after the Interact Duration timer completes. 
		This might do things, apply (predictively or not) GameplayEffects, trigger (predictively or not) GameplayAbilities, etc.
		
		If you want to do something predictively, you can get the ASC from the InteractingActor and use its ScopedPredictionKey.
		
		If you need to trigger a GameplayAbility predictively, the player's ASC needs to have been granted the ability ahead of time. 
		If you don't want to grant every possible predictive ability at game start, you can hide the time needed to replicate the AbilitySpec inside the time needed to interact by granted it in PreInteract().
		
		@param InteractingActor The Actor interacting with this Actor. It will be the AvatarActor from a GameplayAbility.
		@param InteractionComponent UPrimitiveComponent in case an Actor has many separate interactable areas.
	�a��
		���̃A�N�^�[�ƃC���^���N�g���܂��B
		���̊֐��� Interact Duration �^�C�}�[������������ɌĂяo����܂��B
		���̊֐��ł́A (predictively �i�\���\�j�܂��͂����ł͂Ȃ�) GameplayEffects �̓K�p�A (predictively �i�\���\�j�܂��͂����ł͂Ȃ�) GameplayAbilities �̋N���Ȃǂ̏������s���܂��B

		predictively �i�\���I�j�ɉ������s�������ꍇ�́A InteractingActor ���� ASC ���擾���A �X�R�[�v�t�� Prediction �i�\���j�L�[ ���g�p���܂��B

		GameplayAbility �� predictively �i�\���I�j�ɋN������K�v������ꍇ�A�v���C���[�� ASC �Ɏ��O�ɃA�r���e�B���t�^����Ă���K�v������܂��B
		�Q�[���J�n���ɑS�Ă� predictive �i�\���j�\�ȃA�r���e�B��t�^�������Ȃ��ꍇ�́A PreInteract() �ŕt�^���邱�ƂŁA�C���^���N�g�ɕK�v�Ȏ��Ԃ̒��� AbilitySpec �̃��v���P�[�V�������Ԃ��B�����Ƃ��ł��܂��B

		@param InteractingActor ���̃A�N�^�[�ƃC���^���N�g����A�N�^�[�B����� GameplayAbility �� AvatarActor �ɂȂ�܂��B
		@param InteractionComponent UPrimitiveComponent �̓A�N�^�[�������̓Ɨ������C���^���N�g�\�ȃG���A�������Ă���ꍇ�Ɏg�p���܂��B
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void PostInteract(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent);
	virtual void PostInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) {};

	/**
	by GASShooter
		Cancel an ongoing interaction, typically anything happening in PreInteract() while waiting on the Interact Duration Timer. 
		This will be called if the player releases input early.
		
		@param InteractionComponent UPrimitiveComponent in case an Actor has many separate interactable areas.
	�a��
		�i�s���̃C���^���N�g���L�����Z�����܂��B�ʏ�́A Interact Duration �^�C�}�[��҂��Ă���Ԃ� PreInteract() �ŋN�����Ă��鎖���L�����Z�����܂��B
		����́A�v���C���[�̓��͂̊J�������������ꍇ�ɌĂяo����܂��B

		@param InteractionComponent UPrimitiveComponent �̓A�N�^�[�������̓Ɨ������C���^���N�g�\�ȃG���A�������Ă���ꍇ�Ɏg�p���܂��B
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void CancelInteraction(UPrimitiveComponent* InteractionComponent);
	virtual void CancelInteraction_Implementation(UPrimitiveComponent* InteractionComponent) {};

	/**
	by GASShooter
		Returns a delegate for GA_Interact to bind to that fires when this Actor is canceling the interactiong (e.g. died).
		
		@param InteractionComponent UPrimitiveComponent in case an Actor has many separate interactable areas.
	�a��
		GA_Interact ���o�C���h�ł���悤�ɁA�A�N�^�[���C���^���N�g���L�����Z���������i�Ⴆ�Ύ��S�������j�ɋN������f���Q�[�g��Ԃ��܂��B
		
		@param InteractionComponent UPrimitiveComponent �̓A�N�^�[�������̓Ɨ������C���^���N�g�\�ȃG���A�������Ă���ꍇ�Ɏg�p���܂��B
	*/
	virtual FSimpleMulticastDelegate* GetTargetCancelInteractionDelegate(UPrimitiveComponent* InteractionComponent);

	/**
	by GASShooter
		Registers an Actor interacting with this Interactable. 
		Used to send a GameplayEvent to them when this Interactable wishes to cancel interaction prematurely (e.g. a reviving player dies mid-revival). 
		Not meant to be overriden.
	�a��
		���� Interactable �ƃC���^���N�g����A�N�^�[��o�^����B
		���� Interactable ���C���^���N�g�𑁊��ɃL�����Z���������ꍇ�i�Ⴆ�Αh�����̃v���C���[���h�����Ɏ��S�����ꍇ�j�� GameplayEvent �𑗂邽�߂Ɏg�p���܂��B
		overriden ���邱�Ƃ͂ł��܂���B
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable|Do Not Override")
	void RegisterInteracter(UPrimitiveComponent* InteractionComponent, AActor* InteractingActor);
	void RegisterInteracter_Implementation(UPrimitiveComponent* InteractionComponent, AActor* InteractingActor);

	/**
	by GASShooter
		Unregisters an interacting Actor from this Interactable. 
		Not meant to be overriden.
	�a��
		���� Interactable �ƃC���^���N�g����A�N�^�[�̓o�^����������B
		overriden ���邱�Ƃ͂ł��܂���B
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable|Do Not Override")
	void UnregisterInteracter(UPrimitiveComponent* InteractionComponent, AActor* InteractingActor);
	void UnregisterInteracter_Implementation(UPrimitiveComponent* InteractionComponent, AActor* InteractingActor);

	/**
	by GASShooter
		Interactable (or an external Actor, not the Interacter) wants to cancel the interaction (e.g. the reviving player dies mid-revival). 
		Not meant to be overriden.
	�a��
		Interactable �i�܂��� Interacter �ł͂Ȃ��O���̃A�N�^�[�j���C���^���N�g���L�����Z���������ꍇ�i�Ⴆ�Αh�����̃v���C���[���h�����Ɏ��S�����ꍇ�j�Ɏg�p���܂��B
		overriden ���邱�Ƃ͂ł��܂���B
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable|Do Not Override")
	void InteractableCancelInteraction(UPrimitiveComponent* InteractionComponent);
	void InteractableCancelInteraction_Implementation(UPrimitiveComponent* InteractionComponent);

protected:
	TMap<UPrimitiveComponent*, TArray<AActor*>> Interacters;


};
