// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ECLogging.h"
#include "ECResult.h"

/** Needed to force macro expansion on MSVC. */
#define EC_EXPAND(X) X

#define _EC_CONCAT_IMPL(X, Y) X##Y

/** Concatenate two macro variables */
#define EC_CONCAT(X, Y) _EC_CONCAT_IMPL(X, Y)

#define _EC_FUNCNAME_2(X) L##X
#define _EC_FUNCNAME_1(X) _EC_FUNCNAME_2(X)

/** The same as __FUNCTION__, but for a TCHAR string (FString or FStringView). */
#define EC_FUNCNAME _EC_FUNCNAME_1(__FUNCTION__)

/** Generate a compile-time unique identifier for a variable. */
#define EC_UNIQUE_NAME EC_CONCAT(_Temp, __COUNTER__)

/**
 * Log the current function name and an error code enum's message.
 */
#define EC_LOG_RESULT(LogCategory, Verbosity, Enum) \
	UE_LOG(LogCategory, Verbosity, TEXT("%s: %s"), EC_FUNCNAME, *FECResult(Enum).ToString());

#define _EC_VALIDATE_IMPL(TempName, Expr) \
	auto TempName = Expr; \
	if (TempName.IsFailure()) \
	{ \
		return TempName; \
	}

/**
 * If 'Expr' succeeded, continue execution. Else, return the Result Code from 'Expr'.
 */
#define EC_VALIDATE(Expr) \
	EC_EXPAND(_EC_VALIDATE_IMPL(EC_UNIQUE_NAME, Expr))

#define _EC_VALIDATE_LOG_IMPL(TempName, Expr, Else) \
	auto TempName = Expr; \
	if (TempName.IsFailure()) \
	{ \
		EC_LOG_RESULT(LogErrorCodes, Error, TempName); \
		Else; \
	}
/**
 * If Expr succeeded, continue execution. Otherwise, log the result from 'Expr' and execute 'Else'. Usually,
 * 'Else' is either 'return', 'continue', or 'return ReturnValue'.
 */
#define EC_VALIDATE_OR_LOG(Expr, Else) \
	EC_EXPAND(_EC_VALIDATE_LOG_IMPL(EC_UNIQUE_NAME, Expr, Else))
