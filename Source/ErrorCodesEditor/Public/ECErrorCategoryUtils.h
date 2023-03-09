// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"

class UECErrorCategoryEnum;

namespace ErrorCodes
{
ERRORCODESEDITOR_API void BroadcastChanges(const UECErrorCategoryEnum& ErrorCategory,
	const TArray<TPair<FName, int64>>& OldNames,
	bool bResolveData = true
);
}
