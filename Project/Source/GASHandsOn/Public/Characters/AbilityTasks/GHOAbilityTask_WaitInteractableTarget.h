// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Engine/CollisionProfile.h"
#include "GHOAbilityTask_WaitInteractableTarget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitInteractableTargetDelegate, const FGameplayAbilityTargetDataHandle&, Data);

/**
by GASShooter
	Performs a line trace on a timer, looking for an Actor that implements IGSInteractable that is available for interaction.
	The StartLocations are hardcoded for GASShooter since we can be in first and third person so we have to check every time we trace. 
	If you only have one start location, you should make it more generic with a parameter on your AbilityTask node.
�a��
	�^�C�}�[�Ń��C���g���[�X�����s���A�C���^���N�V�����\�� IGHOInteractable �����������A�N�^�[���������܂��B
	StartLocations �� GASShooter �ł̓n�[�h�R�[�h����Ă��āA��l�̂ƎO�l�̂�����̂ŁA�g���[�X���邽�тɃ`�F�b�N���Ȃ���΂Ȃ�܂���B
	StartLocation ��1�����Ȃ��ꍇ�́A AbilityTask �m�[�h�Ƀp�����[�^���w�肵�āA����ʓI�Ȃ��̂ɂ���K�v������܂��B
*/
UCLASS()
class GASHANDSON_API UGHOAbilityTask_WaitInteractableTarget : public UAbilityTask
{
	GENERATED_BODY()

public:
	UGHOAbilityTask_WaitInteractableTarget();
	/**
	by GASShooter
		Traces a line on a timer looking for InteractableTargets.
		@param MaxRange How far to trace.
		@param TimerPeriod Period of trace timer.
		@param bShowDebug Draws debug lines for traces.
	�a��
		InteractableTargets ��T�����߂Ƀ^�C�}�[�Ń��C���g���[�X�����s���܂��B
		@param MaxRange �ǂ��܂Ńg���[�X���邩
		@param TimerPeriod �g���[�X�^�C�}�[�̎���
		@param bShowDebug �g���[�X�̃f�o�b�O���C����`�悷�邩
	*/
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true", HideSpawnParms = "Instigator"), Category = "Ability|Tasks")
	static UGHOAbilityTask_WaitInteractableTarget* WaitForInteractableTarget(UGameplayAbility* OwningAbility, FName TaskInstanceName, FCollisionProfileName TraceProfile, float MaxRange = 200.0f, float TimerPeriod = 0.1f, bool bShowDebug = true);

	// UGameplayTask interface
public:
	/*
	by Epic
		Called to trigger the actual task once the delegates have been set up
		Note that the default implementation does nothing and you don't have to call it
	�a��
		�f���Q�[�g���ݒ肳�ꂽ��A���ۂ̃^�X�N�̋N�������邽�߂ɌĂяo�����B
		�Ȃ��A�f�t�H���g�̎����ł͉������Ȃ��̂ŁA������Ă�d�X�K�v�͂���܂���B
	*/
	virtual void Activate() override;

	/*
	by Epic
		End and CleanUp the task - may be called by the task itself or by the task owner if the owner is ending.
		IMPORTANT! Do NOT call directly! Call EndTask() or TaskOwnerEnded()
		IMPORTANT! When overriding this function make sure to call Super::OnDestroy(bOwnerFinished) as the last thing,
			since the function internally marks the task as "Pending Kill", and this may interfere with internal BP mechanics
	�a��
		�^�X�N�̏I���ƃN���[���A�b�v - �^�X�N���g���Ăяo�����Ƃ��R�[�i�[���I������ꍇ�̓^�X�N�I�[�i�[���Ăяo�����Ƃ��o���܂��B
		�d�v�I���ڌĂяo���Ȃ��ł��������I EndTask() ���� TaskOwnerEnded() ���Ăяo���Ă��������B
		�d�v�I���̊֐����I�[�o�[���C�h����ۂɂ́A�Ō�� Super::OnDestroy(bInOwnerFinished) ���Ăяo���悤�ɂ��Ă��������B
			�Ȃ��Ȃ�A���̊֐��ɂ͓����I�Ƀ^�X�N���u Pending Kill �v�ƃ}�[�N���Ă���A BP �̓������J�j�N�X�Ɏx����������\�������邩��ł��B
	���
		���ڂ̊��̃A�N�Z�X�w��q�����������B���̃N���X�ł͍������ɍ��킹�܂��B
		UGameplayTask(protected) > UAbilityTask(public) > UGHOAbilityTask_WaitReceiveDamage(protected)
	*/
	virtual void OnDestroy(bool AbilityEnded) override;

	// End of UObject interface

protected:
	/** Traces as normal, but will manually filter all hit actors */
	void LineTrace(FHitResult& OutHitResult, const UWorld* World, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params, bool bLookForInteractableActor) const;

	void AimWithPlayerController(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector& TraceStart, FVector& OutTraceEnd, bool bIgnorePitch = false) const;

	bool ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& ClippedPosition) const;

	UFUNCTION()
	void PerformTrace();

	FGameplayAbilityTargetDataHandle MakeTargetData(const FHitResult& HitResult) const;

private:
	void Initialize(UGameplayAbility* InOwningAbility, FName InTaskInstanceName, FCollisionProfileName InTraceProfile, float InMaxRange, float InTimerPeriod, bool bInShowDebug);

public:
	UPROPERTY(BlueprintAssignable)
	FWaitInteractableTargetDelegate FoundNewInteractableTarget;

	UPROPERTY(BlueprintAssignable)
	FWaitInteractableTargetDelegate LostInteractableTarget;

protected:
	FGameplayAbilityTargetingLocationInfo StartLocation;
	FGameplayAbilityTargetingLocationInfo StartLocation1P;
	FGameplayAbilityTargetingLocationInfo StartLocation3P;

	float MaxRange;

	float TimerPeriod;

	bool bShowDebug;

	bool bTraceAffectsAimPitch;

	FCollisionProfileName TraceProfile;

	FGameplayAbilityTargetDataHandle TargetData;

	FTimerHandle TraceTimerHandle;
};
