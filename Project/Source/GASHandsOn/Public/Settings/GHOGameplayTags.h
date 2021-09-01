// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
//#include "GHOGameplayTags.generated.h"


#define TAG_Ability										"Ability"
#define TAG_Ability_AimDownSights						"Ability.AimDownSights"
#define TAG_Ability_AimDownSights_Cancel				"Ability.AimDownSights.Cancel"
#define TAG_Ability_BlocksInteraction					"Ability.BlocksInteraction"
#define TAG_Ability_Interaction							"Ability.Interaction"
#define TAG_Ability_InteractionEnd						"Ability.InteractionEnd"
#define TAG_Ability_Jump								"Ability.Jump"
#define TAG_Ability_NotCanceledByStun					"Ability.NotCanceledByStun"
#define TAG_Ability_Revive								"Ability.Revive"
#define TAG_Ability_Skill								"Ability.Skill"
#define TAG_Ability_Skill_Ability1						"Ability.Skill.Ability1"
#define TAG_Ability_Skill_Ability2						"Ability.Skill.Ability2"
#define TAG_Ability_Skill_Ability3						"Ability.Skill.Ability3"
#define TAG_Ability_Skill_Ability5						"Ability.Skill.Ability5"
#define TAG_Ability_Sprint								"Ability.Sprint"
#define TAG_Ability_Sprint_Cancel						"Ability.Sprint.Cancel"
#define TAG_Ability_Weapon								"Ability.Weapon"
#define TAG_Ability_Weapon_Alternate					"Ability.Weapon.Alternate"
#define TAG_Ability_Weapon_Alternate_Instant			"Ability.Weapon.Alternate.Instant"
#define TAG_Ability_Weapon_IsChanging					"Ability.Weapon.IsChanging"
#define TAG_Ability_Weapon_IsChangingDelayReplication	"Ability.Weapon.IsChangingDelayReplication"
#define TAG_Ability_Weapon_Primary						"Ability.Weapon.Primary"
#define TAG_Ability_Weapon_Primary_Instant				"Ability.Weapon.Primary.Instant"
#define TAG_Ability_Weapon_Reload						"Ability.Weapon.Reload"
#define TAG_Ability_Weapon_Secondary					"Ability.Weapon.Secondary"
#define TAG_Ability_Weapon_Secondary_Instant			"Ability.Weapon.Secondary.Instant"
#define TAG_Cooldown									"Cooldown"
#define TAG_Cooldown_Skill								"Cooldown.Skill"
#define TAG_Cooldown_Skill_Ability5						"Cooldown.Skill.Ability5"
#define TAG_Data										"Data"
#define TAG_Data_Damage									"Data.Damage"
#define TAG_Data_ReloadAmount_Reserve					"Data.ReloadAmount.Reserve"
#define TAG_Effect										"Effect"
#define TAG_Effect_Damage_CanHeadShot					"Effect.Damage.CanHeadShot"
#define TAG_Effect_Damage_HeadShot						"Effect.Damage.HeadShot"
#define TAG_Effect_RemoveOnDeath						"Effect.RemoveOnDeath"
#define TAG_Effect_Shared								"Effect.Shared"
#define TAG_Effect_Shared_PassiveArmor					"Effect.Shared.PassiveArmor"
#define TAG_Event										"Event"
#define TAG_Event_Montage								"Event.Montage"
#define TAG_Event_Montage_EndAbility					"Event.Montage.EndAbility"
#define TAG_Event_Montage_SpawnProjectile				"Event.Montage.SpawnProjectile"
#define TAG_GameplayCue									"GameplayCue"
#define TAG_GameplayCue_Hero_KnockedDown				"GameplayCue.Hero.KnockedDown"
#define TAG_GameplayCue_Hero_Revived					"GameplayCue.Hero.Revived"
#define TAG_GameplayCue_Shared							"GameplayCue.Shared"
#define TAG_GameplayCue_Shared_FireGun					"GameplayCue.Shared.FireGun"
#define TAG_GameplayCue_Shared_FireGun_Impact			"GameplayCue.Shared.FireGun.Impact"
#define TAG_GameplayCue_Shared_Sprint					"GameplayCue.Shared.Sprint"
#define TAG_GameplayCue_Shared_Sprint_LocalVoice		"GameplayCue.Shared.Sprint.LocalVoice"
#define TAG_GameplayCue_Shared_Sprint_Particle			"GameplayCue.Shared.Sprint.Particle"
#define TAG_GameplayCue_Shared_Stun						"GameplayCue.Shared.Stun"
#define TAG_GameplayCue_Weapon_Rifle_Fire				"GameplayCue.Weapon.Rifle.Fire"
#define TAG_GameplayCue_Weapon_RocketLauncher_Fire		"GameplayCue.Weapon.RocketLauncher.Fire"
#define TAG_GameplayCue_Weapon_RocketLauncher_Impact	"GameplayCue.Weapon.RocketLauncher.Impact"
#define TAG_GameplayCue_Weapon_Shotgun_Fire				"GameplayCue.Weapon.Shotgun.Fire"
#define TAG_State										"State"
#define TAG_State_AimDownSights							"State.AimDownSights"
#define TAG_State_AimDownSights_Removal					"State.AimDownSights.Removal"
#define TAG_State_BleedingOut							"State.BleedingOut"
#define TAG_State_CrowdControl							"State.CrowdControl"
#define TAG_State_CrowdControl_Stun						"State.CrowdControl.Stun"
#define TAG_State_Dead									"State.Dead"
#define TAG_State_Sprinting								"State.Sprinting"
#define TAG_State_Sprinting_Removal						"State.Sprinting.Removal"
#define TAG_State_Interacting							"State.Interacting"
#define TAG_State_Interacting_Removal					"State.Interacting.Removal"
#define TAG_State_KnockedDown							"State.KnockedDown"
#define TAG_Weapon_Ammo_None							"Weapon.Ammo.None"
#define TAG_Weapon_Ammo_Rifle							"Weapon.Ammo.Rifle"
#define TAG_Weapon_Ammo_Rocket							"Weapon.Ammo.Rocket"
#define TAG_Weapon_Ammo_Shotgun							"Weapon.Ammo.Shotgun"
#define TAG_Weapon_Equipped_None						"Weapon.Equipped.None"
#define TAG_Weapon_Ammo_None							"Weapon.Ammo.None"
#define TAG_Weapon_Ammo_Rifle							"Weapon.Ammo.Rifle"
#define TAG_Weapon_Ammo_Rocket							"Weapon.Ammo.Rocket"
#define TAG_Weapon_Ammo_Shotgun							"Weapon.Ammo.Shotgun"
#define TAG_Weapon_Equipped_None						"Weapon.Equipped.None"
#define TAG_Weapon_Equipped_Rifle						"Weapon.Equipped.Rifle"
#define TAG_Weapon_Equipped_RocketLauncher				"Weapon.Equipped.RocketLauncher"
#define TAG_Weapon_Equipped_Shotgun						"Weapon.Equipped.Shotgun"
#define TAG_Weapon_Fail_Activation_OnCooldown			"Weapon.Fail.Activation.OnCooldown"
#define TAG_Weapon_FireMode_None						"Weapon.FireMode.None"
#define TAG_Weapon_IsFiring								"Weapon.IsFiring"
#define TAG_Weapon_Rifle_Aiming							"Weapon.Rifle.Aiming"
#define TAG_Weapon_Rifle_Aiming_Removal					"Weapon.Rifle.Aiming.Removal"
#define TAG_Weapon_Rifle_FireMode_Burst					"Weapon.Rifle.FireMode.Burst"
#define TAG_Weapon_Rifle_FireMode_FullAuto				"Weapon.Rifle.FireMode.FullAuto"
#define TAG_Weapon_Rifle_FireMode_SemiAuto				"Weapon.Rifle.FireMode.SemiAuto"
#define TAG_Weapon_RocketLauncher_Aiming				"Weapon.RocketLauncher.Aiming"
#define TAG_Weapon_RocketLauncher_Aiming_Removal		"Weapon.RocketLauncher.Aiming.Removal"
#define TAG_Weapon_Shotgun_Aiming						"Weapon.Shotgun.Aiming"
#define TAG_Weapon_Shotgun_Aiming_Removal				"Weapon.Shotgun.Aiming.Removal"
#define TAG_Weapon_Shotgun_FireMode_FullAuto			"Weapon.Shotgun.FireMode.FullAuto"
#define TAG_Weapon_Shotgun_FireMode_SemiAuto			"Weapon.Shotgun.FireMode.SemiAuto"
