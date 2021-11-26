// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
//#include "GHOGameplayTags.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_AimDownSights)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_AimDownSights_Cancel)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_BlocksInteraction)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Interaction)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_InteractionEnd)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Jump)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_NotCanceledByStun)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Revive)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Skill)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Skill_Ability1)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Skill_Ability2)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Skill_Ability3)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Skill_Ability5)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Sprint)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Sprint_Cancel)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Weapon)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Weapon_Alternate)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Weapon_Alternate_Instant)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Weapon_IsChanging)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Weapon_IsChangingDelayReplication)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Weapon_Primary)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Weapon_Primary_Instant)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Weapon_Reload)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Weapon_Secondary)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Weapon_Secondary_Instant)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Cooldown)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Cooldown_Skill)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Cooldown_Skill_Ability1)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Cooldown_Skill_Ability2)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Data)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Data_Damage)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Data_ReloadAmount_Reserve)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Effect)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Effect_Damage_CanHeadShot)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Effect_Damage_HeadShot)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Effect_RemoveOnDeath)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Effect_Shared)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Effect_Shared_PassiveArmor)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Event)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Event_Montage)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Event_Montage_EndAbility)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Event_Montage_SpawnProjectile)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Hero_KnockedDown)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Hero_Revived)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Shared)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Shared_FireGun)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Shared_FireGun_Impact)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Shared_Sprint)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Shared_Sprint_LocalVoice)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Shared_Sprint_Particle)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Shared_Stun)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Weapon_Rifle_Fire)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Weapon_RocketLauncher_Fire)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Weapon_RocketLauncher_Impact)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayCue_Weapon_Shotgun_Fire)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_AimDownSights)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_AimDownSights_Removal)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_BleedingOut)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_CrowdControl)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_CrowdControl_Stun)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Dead)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Sprinting)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Sprinting_Removal)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Interacting)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Interacting_Removal)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_KnockedDown)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_Ammo_None)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_Ammo_Rifle)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_Ammo_Rocket)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_Ammo_Shotgun)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_Equipped_None)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_Equipped_Rifle)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_Equipped_RocketLauncher)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_Equipped_Shotgun)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_Fail_Activation_OnCooldown)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_FireMode_None)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_IsFiring)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_Rifle_Aiming)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_Rifle_Aiming_Removal)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_Rifle_FireMode_Burst)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_Rifle_FireMode_FullAuto)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_Rifle_FireMode_SemiAuto)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_RocketLauncher_Aiming)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_RocketLauncher_Aiming_Removal)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_Shotgun_Aiming)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_Shotgun_Aiming_Removal)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_Shotgun_FireMode_FullAuto)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_Shotgun_FireMode_SemiAuto)
