// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ECErrorCategory.h"

class UECErrorCategory;

namespace ErrorCodes
{
/**
 * Broadcast that an error category is about to be changed.
 */
void BroadcastPreChange(UECErrorCategory& ErrorCategory);

/**
 * Update editor objects (blueprints, etc.) that are dependent on an error category.
 */
void BroadcastPostChange(const UECErrorCategory& ErrorCategory,
	const TArray<TPair<FName, int64>>& OldNames,
	bool bResolveData = true
);

void SetResultCodeDisplayName(UECErrorCategory& Category, int32 Idx, const FText& NewDisplayName);

void SetResultCodeMessage(UECErrorCategory& Category, int32 Idx, const FText& NewMessage);

void AddResultCodeToCategory(UECErrorCategory& Category, int64 NewCode);

void RemoveResultCodeFromCategory(UECErrorCategory& Category, int32 Idx);

/**
 * Copy all enum pairs, excluding the 'MAX' value.
 */
void CopyResultCodesWithoutMax(TArray<TPair<FName, int64>>& OutEnumPairs, const UECErrorCategory& Category);

/**
 * Get all Error Categories. Order is non-deterministic.
 */
void FindAllErrorCategories(TArray<const UEnum*>& OutErrorCategories);
}
