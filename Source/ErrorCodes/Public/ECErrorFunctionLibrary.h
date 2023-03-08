// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ECErrorCode.h"
#include "UObject/Object.h"

#include "ECErrorFunctionLibrary.generated.h"

UENUM(BlueprintType)
enum class EECLogVerbosity : uint8
{
	// Normal (Printed in white)
	Normal,
	// Warning (Printed in yellow)
	Warning,
	// Failure (Printed in red)
	Error
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
	 * Create an error code.
	 */
	UFUNCTION(BlueprintPure, Category = "Errors", meta = (BlueprintThreadSafe, NativeMakeFunc))
	static FECErrorCode MakeErrorCode(FECErrorCode ErrorCode);

	/**
	 * Break an error code, returning its Category and Code.
	 */
	UFUNCTION(BlueprintPure, Category = "Errors", meta = (BlueprintThreadSafe, NativeBreakFunc))
	static UECErrorCategory* BreakErrorCode(const FECErrorCode& ErrorCode, int64& Code);
	
	/**
	 * Prints an error code's message to the output log.
	 * @param Verbosity Verbosity of the message.
	 * @param Error Error to log.
	 */
	UFUNCTION(BlueprintCallable, Category = "Errors", DisplayName = "Log Error (Output Log)")
	static void LogErrorToOutputLog(EECLogVerbosity Verbosity, FECErrorCode Error);

	/**
	 * Prints an error code's message to the message log.
	 * @param Verbosity Verbosity of the message.
	 * @param Error Error to log.
	 */
	UFUNCTION(BlueprintCallable, Category = "Errors", DisplayName = "Log Error (Message Log)")
	static void LogErrorToMessageLog(EECLogVerbosity Verbosity, FECErrorCode Error);

	/**
	 * Check if two error codes are equal.
	 */
	UFUNCTION(BlueprintPure, Category = "Errors", DisplayName = "Equal (Error Code)", meta = (CompactNodeTitle = "==", BlueprintThreadSafe))
	static bool Equal_ErrorCodeErrorCode(FECErrorCode A, FECErrorCode B);

	/**
	 * Check if two error codes are not equal.
	 */
	UFUNCTION(BlueprintPure, Category = "Errors", DisplayName = "Not Equal (Error Code)", meta = (CompactNodeTitle = "!=", BlueprintThreadSafe))
	static bool NotEqual_ErrorCodeErrorCode(FECErrorCode A, FECErrorCode B);

	/**
	 * Get the message for an error code.
	 */
	UFUNCTION(BlueprintPure, Category = "Errors")
	static FText GetErrorMessage(FECErrorCode ErrorCode);

	/**
	 * Get the title for an error code.
	 */
	UFUNCTION(BlueprintPure, Category = "Errors")
	static FText GetErrorTitle(FECErrorCode ErrorCode);

	/**
	 * Check if an error code is a success (no error).
	 */
	UFUNCTION(BlueprintPure, Category = "Errors")
	static bool IsSuccess(FECErrorCode ErrorCode);

	/**
	 * Check if an error code is an error (it has a valid category and code).
	 */
	UFUNCTION(BlueprintPure, Category = "Errors")
	static bool IsError(FECErrorCode ErrorCode);

	/**
	 * Check if an error code is valid (either IsSuccess or IsError).
	 */
	UFUNCTION(BlueprintPure, Category = "Errors")
	static bool IsValid(FECErrorCode ErrorCode);

	/**
	 * Convert an error code to a short string (Only category and title).
	 */
	UFUNCTION(BlueprintPure, Category = "Errors")
	static FString ToShortString(FECErrorCode ErrorCode);

	/**
	 * Convert an error code to a short string (category and title only).
	 */
	UFUNCTION(BlueprintPure, Category = "Errors", meta = (BlueprintAutocast, Keywords = "cast convert",
		CompactNodeTitle = "->"))
	static FString Conv_ErrorCodeToString(FECErrorCode ErrorCode);
};
