// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#define EC_DEFINE_ERROR_CONVERSION(Category, Enum) \
	FORCEINLINE FECErrorCode MakeErrorCode(Category::Enum ErrorEnum) \
	{ \
		return FECErrorCode(*GetDefault<Category>(), static_cast<int64>(ErrorEnum)); \
	}

#define EC_DEFINE_ERROR(Title, Message) \
	Errors.Emplace(ErrorCodes::Title, FECErrorCodeData(LOCTEXT("ErrorCodeTitle_" #Title, #Title), LOCTEXT("ErrorCodeMessage_" #Title, ##Message)));
