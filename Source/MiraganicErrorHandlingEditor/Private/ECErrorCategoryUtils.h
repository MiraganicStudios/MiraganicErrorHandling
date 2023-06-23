// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include "CoreMinimal.h"
#include "ECErrorCategory.h"

class UECErrorCategory;

namespace ErrorHandling
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

/**
 * Set an error value's display name.
 */
void SetErrorValueDisplayName(UECErrorCategory& Category, int32 Idx, const FText& NewDisplayName);

/**
 * Set an error value's message.
 */
void SetErrorValueMessage(UECErrorCategory& Category, int32 Idx, const FText& NewMessage);

/**
 * Add a new error value to an error category.
 */
void AddErrorValueToCategory(UECErrorCategory& Category, int64 NewCode);

/**
 * Add the default 'Success' value to an error category.
 */
void AddSuccessValueToCategory(UECErrorCategory& Category);

/**
 * Remove the error value at the specified index from an error category.
 */
void RemoveErrorValueFromCategory(UECErrorCategory& Category, int32 Idx);

/**
 * Copy all enum pairs, excluding the 'MAX' value.
 */
void CopyErrorValuesWithoutMax(TArray<TPair<FName, int64>>& OutEnumPairs, const UECErrorCategory& Category);

/**
 * Get all Error Categories. Order is non-deterministic.
 */
void FindAllErrorCategories(TArray<const UEnum*>& OutErrorCategories);
}
