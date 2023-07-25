// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include "CoreMinimal.h"
#include "ECLogging.generated.h"

MIRAGANICERRORHANDLING_API DECLARE_LOG_CATEGORY_EXTERN(LogErrorHandling, Log, Log);

namespace EMessageSeverity
{
enum Type;
}

UENUM(BlueprintType)
enum class EECLogVerbosity : uint8
{
	// Prints to console and log file (Red)
	Error,
	// Prints to console and log file (Yellow)
	Warning,
	// Prints to console and log file (White)
	Display,
	// Prints to log file (White)
	Log,
	// Prints to log file (White)
	Verbose,
	// Prints to log file (White)
	VeryVerbose,
};

namespace Mgnc
{
namespace Log
{
FORCEINLINE ELogVerbosity::Type ToLogVerbosity(EECLogVerbosity Verbosity)
{
	switch (Verbosity)
	{
		case EECLogVerbosity::Error:
			return ELogVerbosity::Error;
		case EECLogVerbosity::Warning:
			return ELogVerbosity::Warning;
		case EECLogVerbosity::Display:
			return ELogVerbosity::Display;
		case EECLogVerbosity::Log:
			return ELogVerbosity::Log;
		case EECLogVerbosity::Verbose:
			return ELogVerbosity::Verbose;
		case EECLogVerbosity::VeryVerbose:
			return ELogVerbosity::VeryVerbose;
		default:
			return ELogVerbosity::Error;
	}
}

EMessageSeverity::Type ToMessageVerbosity(EECLogVerbosity Verbosity);

/**
 * Log a message with a variable verbosity.
 */
MIRAGANICERRORHANDLING_API void LogWithVerbosity(FStringView FuncName, FStringView Msg, ELogVerbosity::Type Verbosity);

/**
 * Log a formatted message with a variable verbosity.
 */
MIRAGANICERRORHANDLING_API void LogFormattedWithVerbosity(FStringView FuncName, FStringView MsgFormat, const FStringFormatOrderedArguments& Args, ELogVerbosity::Type Verbosity);

/**
 * Log a message to the message log with a variable verbosity.
 */
MIRAGANICERRORHANDLING_API void LogToMessageLog(const FText& Msg, EMessageSeverity::Type Verbosity);
} // namespace Log
} // namespace Mgnc
