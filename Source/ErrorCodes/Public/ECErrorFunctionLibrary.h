﻿// Copyright 2022 Miraganic Studios. All rights reserved.

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
UCLASS()
class ERRORCODES_API UECErrorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
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

	UFUNCTION(BlueprintPure, Category = "Errors", DisplayName = "Equal (Error Code)", meta = (CompactNodeTitle = "==", BlueprintThreadSafe))
	static bool Equal_ErrorCodeErrorCode(FECErrorCode A, FECErrorCode B);

	UFUNCTION(BlueprintPure, Category = "Errors", DisplayName = "Not Equal (Error Code)", meta = (CompactNodeTitle = "!=", BlueprintThreadSafe))
	static bool NotEqual_ErrorCodeErrorCode(FECErrorCode A, FECErrorCode B);

	UFUNCTION(BlueprintPure, Category = "PinOptions", meta = (BlueprintInternalUseOnly = "true"))
	static bool NotEqual_ErrorCodeInner(FECErrorCode A, FECErrorCode B);
	
	// /**
	//  *  Prints a generic gameplay error to the output log.
	//  * @param Error Error to log.
	//  */
	// UFUNCTION(BlueprintCallable, Category = "Miraganic|Errors")
	// static void LogError(EMGLogVerbosity Verbosity, const FMGGenericGameplayError& Error);
	//
	// /**
	//  *  Prints a generic gameplay error to the message log.
	//  * @param Error Error to log.
	//  */
	// UFUNCTION(BlueprintCallable, Category = "Miraganic|Errors")
	// static void LogErrorMessage(EMGLogVerbosity Verbosity, const FMGGenericGameplayError& Error);
	//
	// /**
	//  *  Try to get an error from an optional error.
	//  * @param OutError Unwrapped error.
	//  * @param OptionalError Optional error to unwrap.
	//  * @return True if the option contained an error.
	//  */
	// UFUNCTION(BlueprintCallable, Category = "Miraganic|Errors")
	// static bool TryGetError(FMGGenericGameplayError& OutError, const FMGResult& OptionalError);
	//
	// /**
	//  *  Get an error's message.
	//  * @param Error Error to get a message from.
	//  * @return The error message as a string.
	//  */
	// UFUNCTION(BlueprintPure, Category = "Miraganic|Errors")
	// static FString GetErrorMessage_String(const FMGGameplayError& Error);
	//
	// /**
	//  *  Get an error's message.
	//  * @param Error Error to get a message from.
	//  * @return The error message as text.
	//  */
	// UFUNCTION(BlueprintPure, Category = "Miraganic|Errors")
	// static FText GetErrorMessage_Text(const FMGGameplayError& Error);
	//
	// //--------------------------------------------------------------------------------------------------------
	// // Using Results in BP
	// //   BP doesn't support variants, so instead of Result<T, E>, we use a struct similar to Optional<E>.
	// //   BP supports returning multiple values, so users may pass their success values separately from their
	// //   optional error.
	//
	// /**
	//  * Get a return value for success.
	//  * @return A result indicating success.
	//  */
	// UFUNCTION(BlueprintPure, Category = "Miraganic|Errors", DisplayName = "Succeed")
	// static FMGResult MakeSuccess();
	//
	// /**
	//  * Get a return value for failure.
	//  * @return A result containing the error.
	//  */
	// UFUNCTION(BlueprintPure, Category = "Miraganic|Errors", DisplayName = "Fail")
	// static FMGResult MakeError(const FMGGenericGameplayError& Error);
	//
	// /**
	//  * Check whether a result has an error.
	//  */
	// UFUNCTION(BlueprintPure, Category = "Miraganic|Errors")
	// static bool HasError(const FMGResult& Result);
	//
	// /**
	//  * Get the gameplay tag for a generic gameplay error.
	//  */
	// UFUNCTION(BlueprintPure, Category = "Miraganic|Errors")
	// static FGameplayTag GetErrorTag(const FMGGenericGameplayError& Error);
};