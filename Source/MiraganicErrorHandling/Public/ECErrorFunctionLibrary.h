// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include "CoreMinimal.h"
#include "ECResult.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "ECErrorFunctionLibrary.generated.h"

UENUM(BlueprintType)
enum class EECLogVerbosity : uint8
{
	// Failure (Printed in red)
	Error,
	// Warning (Printed in yellow)
	Warning,
	// Normal (Printed in white)
	Normal,
};

/**
 * Function library for blueprint error handling.
 */
UCLASS(meta=(ScriptName="ECErrorFunctionLibrary"))
class MIRAGANICERRORHANDLING_API UECErrorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Create a result.
	 */
	UFUNCTION(BlueprintPure, Category = "ErrorHandling", meta = (BlueprintThreadSafe, NativeMakeFunc))
	static FECResult MakeResult(FECResult Result);
	
	/**
	 * Prints a result's message to the output log.
	 * @param Verbosity Verbosity of the message.
	 * @param Result Error to log.
	 */
	UFUNCTION(BlueprintCallable, Category = "ErrorHandling", DisplayName = "Log Result (Output Log)")
	static void LogResultToOutputLog(EECLogVerbosity Verbosity, FECResult Result);

	/**
	 * Prints a result's message to the message log.
	 * @param Verbosity Verbosity of the message.
	 * @param Result Error to log.
	 */
	UFUNCTION(BlueprintCallable, Category = "ErrorHandling", DisplayName = "Log Result (Message Log)")
	static void LogResultToMessageLog(EECLogVerbosity Verbosity, FECResult Result);

	/**
	 * Print a result's message to the output log if it's a failure.
	 * @param Verbosity Verbosity of the message.
	 * @param Result Result to log.
	 */
	UFUNCTION(BlueprintCallable, Category = "ErrorHandling", DisplayName = "Log If Failure (Output Log)")
	static void LogResultToOutputLogIfFailure(EECLogVerbosity Verbosity, FECResult Result);

	/**
	 * Print a result's message to the message log if it's a failure.
	 * @param Verbosity Verbosity of the message.
	 * @param Result Result to log.
	 */
	UFUNCTION(BlueprintCallable, Category = "ErrorHandling", DisplayName = "Log If Failure (Message Log)")
	static void LogResultToMessageLogIfFailure(EECLogVerbosity Verbosity, FECResult Result);

	/**
	 * Check if two results are equal.
	 */
	UFUNCTION(BlueprintPure, Category = "ErrorHandling", DisplayName = "Equal (Result)", meta = (CompactNodeTitle = "==", BlueprintThreadSafe))
	static bool Equal_ResultResult(FECResult A, FECResult B);

	/**
	 * Check if two results are not equal.
	 */
	UFUNCTION(BlueprintPure, Category = "ErrorHandling", DisplayName = "Not Equal (Result)", meta = (CompactNodeTitle = "!=", BlueprintThreadSafe))
	static bool NotEqual_ResultResult(FECResult A, FECResult B);

	/**
	 * Get the message for a result.
	 */
	UFUNCTION(BlueprintPure, Category = "ErrorHandling")
	static FText GetMessage(FECResult Result);

	/**
	 * Get the title for a result.
	 */
	UFUNCTION(BlueprintPure, Category = "ErrorHandling")
	static FText GetTitle(FECResult Result);

	/**
	 * Check if a result is a success (no error).
	 */
	UFUNCTION(BlueprintPure, Category = "ErrorHandling")
	static bool IsSuccess(FECResult Result);

	/**
	 * Check if a result is a failure (either a valid error or an invalid state).
	 */
	UFUNCTION(BlueprintPure, Category = "ErrorHandling")
	static bool IsFailure(FECResult Result);

	/**
	 * Check if a result has a valid error (its category is valid and contains the result).
	 */
	UFUNCTION(BlueprintPure, Category = "ErrorHandling")
	static bool HasValidError(FECResult Result);

	/**
	 * Check if a result is in a valid state (either IsSuccess or HasValidError).
	 */
	UFUNCTION(BlueprintPure, Category = "ErrorHandling")
	static bool IsValid(FECResult Result);

	/**
	 * Convert a result to a short string (Only Category and Title).
	 */
	UFUNCTION(BlueprintPure, Category = "ErrorHandling")
	static FString ToShortString(FECResult Result);

	/**
	 * Convert a result to a string (Category, Title, and Message).
	 */
	UFUNCTION(BlueprintPure, Category = "ErrorHandling", DisplayName = "To String (Result)",
		meta = (BlueprintAutocast, Keywords = "cast convert", CompactNodeTitle = "->"))
	static FString Conv_ErrorCodeToString(FECResult Result);

	/**
	 * Construct a result from an enum and its value. Note that this can return invalid Results.
	 *
	 * This is not the same as 'ConstructRaw', because in blueprint we want to display an error when the enum
	 * is invalid. For each case:
	 * Valid Enum  , Value == 0: Success (Success value for user-defined enum)
	 * Valid Enum  , Value >  0: Some user-defined error
	 * Invalid Enum, Value == 0: Success, but print runtime warning (Enum input was invalid)
	 * Invalid Enum, Value >  0: Invalid, print runtime warning (Enum input was invalid)
	 */
	UFUNCTION(BlueprintPure, Category = "ErrorHandling", meta = (BlueprintInternalUseOnly = true))
	static FECResult EnumToResult(const UEnum* Enum, uint8 EnumValue);
};
