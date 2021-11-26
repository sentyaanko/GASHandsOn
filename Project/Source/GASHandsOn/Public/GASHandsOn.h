// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define ACTOR_ROLE_FSTRING *(FindObject<UEnum>(ANY_PACKAGE, TEXT("ENetRole"), true)->GetNameStringByValue(GetLocalRole()))
#define GET_ACTOR_ROLE_FSTRING(Actor) *(FindObject<UEnum>(ANY_PACKAGE, TEXT("ENetRole"), true)->GetNameStringByValue(Actor->GetLocalRole()))

UENUM(BlueprintType)
enum class EGHOAbilityInputID : uint8
{
	None			UMETA(DisplayName = "None"),			// 0  None
	Jump			UMETA(DisplayName = "Jump"),			// 1  Space
	Confirm			UMETA(DisplayName = "Confirm"),			// 2  LMB
	Cancel			UMETA(DisplayName = "Cancel"),			// 3  RMB
	PrimaryFire		UMETA(DisplayName = "Primary Fire"),	// 4  LMB
	SecondaryFire	UMETA(DisplayName = "Secondary Fire"),	// 5  RMB
	AlternateFire	UMETA(DisplayName = "Alternate Fire"),	// 6  CMB
	NextWeapon		UMETA(DisplayName = "Next Weapon"),		// 7  MouseScrollUp
	PrevWeapon		UMETA(DisplayName = "Previous Weapon"),	// 8  MouseScrollDown
	Sprint			UMETA(DisplayName = "Sprint"),			// 9  LeftShift
	Dash			UMETA(DisplayName = "Dash"),			// 10 Q
	Interact		UMETA(DisplayName = "Interact"),		// 11 E
	Reload			UMETA(DisplayName = "Reload"),			// 12 R
	Skill1			UMETA(DisplayName = "Skill1"),			// 13 One
	Skill2			UMETA(DisplayName = "Skill2"),			// 14 Two
	Skill3			UMETA(DisplayName = "Skill3"),			// 15 Three
	Skill4			UMETA(DisplayName = "Skill4"),			// 16 Four
	Passive1		UMETA(DisplayName = "Passive1"),		// 17 (not key assigned)
	Passive2		UMETA(DisplayName = "Passive2"),		// 18 (not key assigned)
	Passive3		UMETA(DisplayName = "Passive3"),		// 19 (not key assigned)
	Passive4		UMETA(DisplayName = "Passive4"),		// 20 (not key assigned)
};

