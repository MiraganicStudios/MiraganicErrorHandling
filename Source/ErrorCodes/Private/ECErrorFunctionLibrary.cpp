// Copyright 2022 Miraganic Studios. All rights reserved.

#include "ECErrorFunctionLibrary.h"

#include "ECLogging.h"

FECErrorCode UECErrorFunctionLibrary::MakeErrorCode(FECErrorCode ErrorCode)
{
	return ErrorCode;
}

// UEnum* UECErrorFunctionLibrary::BreakErrorCode(const FECErrorCode& ErrorCode, int64& Code)
// {
// 	Code = ErrorCode.GetCode();
// 	return const_cast<UEnum*>(ErrorCode.GetCategory());
// }

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
			MessageLog.Info(Error.GetFormattedMessage());
		return;
		case EECLogVerbosity::Warning:
			MessageLog.Warning(Error.GetFormattedMessage());
		return;
		case EECLogVerbosity::Error:
			MessageLog.Error(Error.GetFormattedMessage());
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
	return ErrorCode.GetMessage();
}

FText UECErrorFunctionLibrary::GetErrorTitle(FECErrorCode ErrorCode)
{
	return ErrorCode.GetTitle();
}

bool UECErrorFunctionLibrary::IsSuccess(FECErrorCode ErrorCode)
{
	return ErrorCode.IsSuccess();
}

bool UECErrorFunctionLibrary::IsError(FECErrorCode ErrorCode)
{
	return ErrorCode.IsError();
}

bool UECErrorFunctionLibrary::IsValid(FECErrorCode ErrorCode)
{
	return ErrorCode.IsValid();
}

FString UECErrorFunctionLibrary::ToShortString(FECErrorCode ErrorCode)
{
	return ErrorCode.ToShortString();
}

FString UECErrorFunctionLibrary::Conv_ErrorCodeToString(FECErrorCode ErrorCode)
{
	return ErrorCode.ToString();
}
