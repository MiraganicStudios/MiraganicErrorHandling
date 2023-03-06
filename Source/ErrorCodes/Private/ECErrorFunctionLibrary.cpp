// Copyright 2022 Miraganic Studios. All rights reserved.

#include "ECErrorFunctionLibrary.h"

#include "ECLogging.h"

FECErrorCode UECErrorFunctionLibrary::MakeErrorCode(FECErrorCode ErrorCode)
{
	return ErrorCode;
}

UECErrorCategory* UECErrorFunctionLibrary::BreakErrorCode(const FECErrorCode& ErrorCode, int64& Code)
{
	if (ErrorCode.IsSuccess())
	{
		// For invalid codes, we want to always return {Null, 0}, even if the value was serialized as
		// something else.
		Code = 0;
		return nullptr;
	}
	else
	{
		Code = ErrorCode.Code;
		return const_cast<UECErrorCategory*>(ErrorCode.Category.Get());
	}
}

void UECErrorFunctionLibrary::LogErrorToOutputLog(EECLogVerbosity Verbosity, FECErrorCode Error)
{
	switch (Verbosity)
	{
		default:
		case EECLogVerbosity::Normal:
			UE_LOG(LogErrorCodes, Display, TEXT("%s"), *Error.ToString());
			return;
		case EECLogVerbosity::Warning:
			UE_LOG(LogErrorCodes, Warning, TEXT("%s"), *Error.ToString());
			return;
		case EECLogVerbosity::Error:
			UE_LOG(LogErrorCodes, Error, TEXT("%s"), *Error.ToString());
			return;
	}
}

void UECErrorFunctionLibrary::LogErrorToMessageLog(EECLogVerbosity Verbosity, FECErrorCode Error)
{
	FMessageLog MessageLog("PIE");
	switch (Verbosity)
	{
		default:
		case EECLogVerbosity::Normal:
			MessageLog.Info(Error.GetErrorMessage());
		return;
		case EECLogVerbosity::Warning:
			MessageLog.Warning(Error.GetErrorMessage());
		return;
		case EECLogVerbosity::Error:
			MessageLog.Error(Error.GetErrorMessage());
		return;
	}
}

bool UECErrorFunctionLibrary::Equal_ErrorCodeErrorCode(FECErrorCode A, FECErrorCode B)
{
	return A == B;
}

bool UECErrorFunctionLibrary::NotEqual_ErrorCodeErrorCode(FECErrorCode A, FECErrorCode B)
{
	return A != B;
}

FText UECErrorFunctionLibrary::GetErrorMessage(FECErrorCode ErrorCode)
{
	return ErrorCode.GetErrorMessage();
}

FText UECErrorFunctionLibrary::GetErrorTitle(FECErrorCode ErrorCode)
{
	return ErrorCode.GetErrorTitle();
}

bool UECErrorFunctionLibrary::IsSuccess(FECErrorCode ErrorCode)
{
	return ErrorCode.IsSuccess();
}

bool UECErrorFunctionLibrary::IsError(FECErrorCode ErrorCode)
{
	return ErrorCode.IsError();
}

FString UECErrorFunctionLibrary::Conv_ErrorCodeToString(FECErrorCode ErrorCode)
{
	return ErrorCode.ToShortString();
}

// void UMGErrorFunctionLibrary::LogError(EMGLogVerbosity Verbosity, const FMGGenericGameplayError& Error)
// {
// 	// Note that we can't simply map the verbosity to ELogVerbosity because the macro takes the 'verbosity'
// 	// string instead of a literal.
// 	switch (Verbosity)
// 	{
// 		case EMGLogVerbosity::Normal: UE_LOG(LogErrorCodes, Display, TEXT("%s"), *Error.ToString()); break;
// 		case EMGLogVerbosity::Warning: UE_LOG(LogErrorCodes, Warning, TEXT("%s"), *Error.ToString()); break;
// 		case EMGLogVerbosity::Error: UE_LOG(LogErrorCodes, Error, TEXT("%s"), *Error.ToString()); break;
// 		default: UE_LOG(LogErrorCodes, Display, TEXT("%s"), *Error.ToString()); break;
// 	}
// }
//
// void UMGErrorFunctionLibrary::LogErrorMessage(EMGLogVerbosity Verbosity, const FMGGenericGameplayError& Error)
// {
// 	FMessageLog MessageLog("PIE");
// 	switch (Verbosity)
// 	{
// 		case EMGLogVerbosity::Normal: MessageLog.Info(Error.ToText()); break;
// 		case EMGLogVerbosity::Warning: MessageLog.Warning(Error.ToText()); break;
// 		case EMGLogVerbosity::Error: MessageLog.Error(Error.ToText()); break;
// 		default: MessageLog.Info(Error.ToText()); break;
// 	}
// }
//
// bool UMGErrorFunctionLibrary::TryGetError(FMGGenericGameplayError& OutError, const FMGResult& OptionalError)
// {
// 	if (!OptionalError.bHasError)
// 	{
// 		return false;
// 	}
//
// 	OutError = OptionalError.Error;
// 	return true;
// }
//
// FString UMGErrorFunctionLibrary::GetErrorMessage_String(const FMGGameplayError& Error) { return Error.ToString(); }
//
// FText UMGErrorFunctionLibrary::GetErrorMessage_Text(const FMGGameplayError& Error) { return Error.ToText(); }
//
// FMGResult UMGErrorFunctionLibrary::MakeSuccess() { return FMGResult(); }
//
// FMGResult UMGErrorFunctionLibrary::MakeError(const FMGGenericGameplayError& Error) { return FMGResult(Error); }
//
// bool UMGErrorFunctionLibrary::HasError(const FMGResult& Result) { return Result.bHasError; }
//
// FGameplayTag UMGErrorFunctionLibrary::GetErrorTag(const FMGGenericGameplayError& Error) { return Error.IdTag; }
