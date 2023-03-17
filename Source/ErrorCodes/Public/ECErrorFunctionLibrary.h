// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ECResult.h"
#include "UObject/Object.h"

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
class ERRORCODES_API UECErrorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Create a result.
	 */
	UFUNCTION(BlueprintPure, Category = "Errors", meta = (BlueprintThreadSafe, NativeMakeFunc))
	static FECResult MakeResult(FECResult Result);
	
	/**
	 * Prints a result's message to the output log.
	 * @param Verbosity Verbosity of the message.
	 * @param Result Error to log.
	 */
	UFUNCTION(BlueprintCallable, Category = "Errors", DisplayName = "Log Result (Output Log)")
	static void LogResultToOutputLog(EECLogVerbosity Verbosity, FECResult Result);

	/**
	 * Prints a result's message to the message log.
	 * @param Verbosity Verbosity of the message.
	 * @param Result Error to log.
	 */
	UFUNCTION(BlueprintCallable, Category = "Errors", DisplayName = "Log Result (Message Log)")
	static void LogResultToMessageLog(EECLogVerbosity Verbosity, FECResult Result);

	/**
	 * Print a result's message to the output log if it's a failure.
	 * @param Verbosity Verbosity of the message.
	 * @param Result Error code to log.
	 */
	UFUNCTION(BlueprintCallable, Category = "Errors", DisplayName = "Log If Failure (Output Log)")
	static void LogResultToOutputLogIfFailure(EECLogVerbosity Verbosity, FECResult Result);

	/**
	 * Print a result's message to the message log if it's a failure.
	 * @param Verbosity Verbosity of the message.
	 * @param Result Error code to log.
	 */
	UFUNCTION(BlueprintCallable, Category = "Errors", DisplayName = "Log If Failure (Message Log)")
	static void LogResultToMessageLogIfFailure(EECLogVerbosity Verbosity, FECResult Result);

	/**
	 * Check if two results are equal.
	 */
	UFUNCTION(BlueprintPure, Category = "Errors", DisplayName = "Equal (Error Code)", meta = (CompactNodeTitle = "==", BlueprintThreadSafe))
	static bool Equal_ResultResult(FECResult A, FECResult B);

	/**
	 * Check if two results are not equal.
	 */
	UFUNCTION(BlueprintPure, Category = "Errors", DisplayName = "Not Equal (Error Code)", meta = (CompactNodeTitle = "!=", BlueprintThreadSafe))
	static bool NotEqual_ResultResult(FECResult A, FECResult B);

	/**
	 * Get the message for a result.
	 */
	UFUNCTION(BlueprintPure, Category = "Errors")
	static FText GetMessage(FECResult Result);

	/**
	 * Get the title for a result.
	 */
	UFUNCTION(BlueprintPure, Category = "Errors")
	static FText GetTitle(FECResult Result);

	/**
	 * Check if a result is a success (no error).
	 */
	UFUNCTION(BlueprintPure, Category = "Errors")
	static bool IsSuccess(FECResult Result);

	/**
	 * Check if a result is a failure (either a valid error or an invalid state).
	 */
	UFUNCTION(BlueprintPure, Category = "Errors")
	static bool IsFailure(FECResult Result);

	/**
	 * Check if a result has a valid error (its category is valid and contains the result).
	 */
	UFUNCTION(BlueprintPure, Category = "Errors")
	static bool HasValidError(FECResult Result);

	/**
	 * Check if a result is in a valid state (either IsSuccess or HasValidError).
	 */
	UFUNCTION(BlueprintPure, Category = "Errors")
	static bool IsValid(FECResult Result);

	/**
	 * Convert a result to a short string (Only Category and Title).
	 */
	UFUNCTION(BlueprintPure, Category = "Errors")
	static FString ToShortString(FECResult Result);

	/**
	 * Convert a result to a string (Category, Title, and Message).
	 */
	UFUNCTION(BlueprintPure, Category = "Errors", DisplayName = "To String (Error Code)",
		meta = (BlueprintAutocast, Keywords = "cast convert", CompactNodeTitle = "->"))
	static FString Conv_ErrorCodeToString(FECResult Result);
};
