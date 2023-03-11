// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"

/** Needed to force macro expansion on MSVC. */
#define EC_EXPAND(X) X

#define _EC_CONCAT_IMPL(X, Y) X##Y

/** Concatenate two macro variables */
#define EC_CONCAT(X, Y) _EC_CONCAT_IMPL(X, Y)

/** Generate a compile-time unique identifier for a variable. */
#define EC_UNIQUE_NAME EC_CONCAT(_Temp, __COUNTER__)

#define _EC_RETURN_IF_FAIL_IMPL(TempName, Expr) \
	auto TempName = Expr; \
	if (!TempName.IsSuccess()) \
	{ \
		return TempName; \
	}

/**
 * If Expr did not succeed, return the error code. Else, continue.
 */
#define EC_VALIDATE(Expr) \
	EC_EXPAND(_EC_RETURN_IF_FAIL_IMPL(EC_UNIQUE_NAME, Expr))
