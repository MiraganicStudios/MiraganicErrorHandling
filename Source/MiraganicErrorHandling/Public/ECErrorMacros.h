// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

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

/**
 * Log the current function name and use the result's message as the formatting string.
 */
#define EC_LOG_RESULT_FMT(LogCategory, Verbosity, Enum, ...) \
	UE_LOG(LogCategory, Verbosity, TEXT("%s: %s"), EC_FUNCNAME, *FString::Format(*FECResult(Enum).GetMessage().ToString(), {##__VA_ARGS__}));

/**
 * Log a result's message if a condition passes.
 */
#define EC_CLOG_RESULT(Condition, LogCategory, Verbosity, Result) \
	UE_CLOG(Condition, LogCategory, Verbosity, TEXT("%s: %s"), EC_FUNCNAME, *FECResult(Result).ToString());

#define _EC_TRY_IMPL(TempName, Expr) \
	auto TempName = Expr; \
	if (TempName.IsFailure()) \
	{ \
		return TempName; \
	}

/**
 * If 'Expr' fails, return the result to the caller. Else, continue.
 */
#define EC_TRY(Expr) \
	EC_EXPAND(_EC_TRY_IMPL(EC_UNIQUE_NAME, Expr))
