// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ECErrorCategoryEnum.h"

class UECErrorCategoryEnum;

namespace ErrorCodes
{
/**
 * Broadcast that an error category is about to be changed.
 */
void BroadcastPreChange(UECErrorCategoryEnum& ErrorCategory);

/**
 * Update editor objects (blueprints, etc.) that are dependent on an error category.
 */
void BroadcastPostChange(const UECErrorCategoryEnum& ErrorCategory,
	const TArray<TPair<FName, int64>>& OldNames,
	bool bResolveData = true
);

void SetErrorCodeDisplayName(UECErrorCategoryEnum& Category, int32 Idx, const FText& NewDisplayName);

void SetErrorCodeMessage(UECErrorCategoryEnum& Category, int32 Idx, const FText& NewMessage);

void AddErrorCodeToCategory(UECErrorCategoryEnum& Category, int64 NewCode);

void RemoveErrorCodeFromCategory(UECErrorCategoryEnum& Category, int32 Idx);

/**
 * Copy all enum pairs, excluding the 'MAX' value.
 */
void CopyErrorCodesWithoutMax(TArray<TPair<FName, int64>>& OutEnumPairs, const UECErrorCategoryEnum& Category);

/**
 * Get all Error Categories. Order is non-deterministic.
 */
void FindAllErrorCategories(TArray<const UEnum*>& OutErrorCategories);
}
