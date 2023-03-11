// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ECResultCode.h"
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
	 * Create an result code.
	 */
	UFUNCTION(BlueprintPure, Category = "Errors", meta = (BlueprintThreadSafe, NativeMakeFunc))
	static FECResultCode MakeResultCode(FECResultCode ResultCode);
	
	/**
	 * Prints an result code's message to the output log.
	 * @param Verbosity Verbosity of the message.
	 * @param ResultCode Error to log.
	 */
	UFUNCTION(BlueprintCallable, Category = "Errors", DisplayName = "Log Error Code (Output Log)")
	static void LogResultCodeToOutputLog(EECLogVerbosity Verbosity, FECResultCode ResultCode);

	/**
	 * Prints an result code's message to the message log.
	 * @param Verbosity Verbosity of the message.
	 * @param ResultCode Error to log.
	 */
	UFUNCTION(BlueprintCallable, Category = "Errors", DisplayName = "Log Error Code (Message Log)")
	static void LogResultCodeToMessageLog(EECLogVerbosity Verbosity, FECResultCode ResultCode);

	/**
	 * Print an result code's message to the output log if it's a failure.
	 * @param Verbosity Verbosity of the message.
	 * @param ResultCode Error code to log.
	 */
	UFUNCTION(BlueprintCallable, Category = "Errors", DisplayName = "Log If Failure (Output Log)")
	static void LogResultCodeToOutputLogIfFailure(EECLogVerbosity Verbosity, FECResultCode ResultCode);

	/**
	 * Print an result code's message to the message log if it's a failure.
	 * @param Verbosity Verbosity of the message.
	 * @param ResultCode Error code to log.
	 */
	UFUNCTION(BlueprintCallable, Category = "Errors", DisplayName = "Log If Failure (Message Log)")
	static void LogResultCodeToMessageLogIfFailure(EECLogVerbosity Verbosity, FECResultCode ResultCode);

	/**
	 * Check if two result codes are equal.
	 */
	UFUNCTION(BlueprintPure, Category = "Errors", DisplayName = "Equal (Error Code)", meta = (CompactNodeTitle = "==", BlueprintThreadSafe))
	static bool Equal_ResultCodeResultCode(FECResultCode A, FECResultCode B);

	/**
	 * Check if two result codes are not equal.
	 */
	UFUNCTION(BlueprintPure, Category = "Errors", DisplayName = "Not Equal (Error Code)", meta = (CompactNodeTitle = "!=", BlueprintThreadSafe))
	static bool NotEqual_ResultCodeResultCode(FECResultCode A, FECResultCode B);

	/**
	 * Get the message for an result code.
	 */
	UFUNCTION(BlueprintPure, Category = "Errors")
	static FText GetMessage(FECResultCode ResultCode);

	/**
	 * Get the title for an result code.
	 */
	UFUNCTION(BlueprintPure, Category = "Errors")
	static FText GetTitle(FECResultCode ResultCode);

	/**
	 * Check if an result code is a success (no error).
	 */
	UFUNCTION(BlueprintPure, Category = "Errors")
	static bool IsSuccess(FECResultCode ResultCode);

	/**
	 * Check if an result code is a failure (either a valid error or an invalid state).
	 */
	UFUNCTION(BlueprintPure, Category = "Errors")
	static bool IsFailure(FECResultCode ResultCode);

	/**
	 * Check if an result code has a valid error (its category is valid and contains the result code).
	 */
	UFUNCTION(BlueprintPure, Category = "Errors")
	static bool HasValidError(FECResultCode ResultCode);

	/**
	 * Check if an result code is in a valid state (either IsSuccess or HasValidError).
	 */
	UFUNCTION(BlueprintPure, Category = "Errors")
	static bool IsValid(FECResultCode ResultCode);

	/**
	 * Convert an result code to a short string (Only category and title).
	 */
	UFUNCTION(BlueprintPure, Category = "Errors")
	static FString ToShortString(FECResultCode ResultCode);

	/**
	 * Convert an result code to a short string (category and title only).
	 */
	UFUNCTION(BlueprintPure, Category = "Errors", DisplayName = "To String (Error Code)",
		meta = (BlueprintAutocast, Keywords = "cast convert", CompactNodeTitle = "->"))
	static FString Conv_ErrorCodeToString(FECResultCode ResultCode);
};
