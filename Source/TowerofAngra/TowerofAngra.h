// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"
#include "UObject/ConstructorHelpers.h"

UENUM(BlueprintType)
enum class EPlayerType : uint8
{
	KAYA,
	AXE,
	WARRIOR,
	NONE
};

static EPlayerType PlayerType{ EPlayerType::NONE };

DECLARE_LOG_CATEGORY_EXTERN(TowerofAngra, Log, All);
#define TOALOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define TOALOG_S(Verbosity) UE_LOG(TowerofAngra, Verbosity, TEXT("%s"), *TOALOG_CALLINFO)
#define TOALOG(Verbosity, Format, ...) UE_LOG(TowerofAngra, Verbosity, TEXT("%s %s"),*TOALOG_CALLINFO, *FString::Printf(Format,##__VA_ARGS__))