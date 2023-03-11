// Copyright 2022 Miraganic Studios. All rights reserved.

#include "ECErrorFunctionLibrary.h"

#include "ECLogging.h"

FECResultCode UECErrorFunctionLibrary::MakeResultCode(FECResultCode ResultCode)
{
	return ResultCode;
}

void UECErrorFunctionLibrary::LogResultCodeToOutputLog(EECLogVerbosity Verbosity, FECResultCode ResultCode)
{
	switch (Verbosity)
	{
		default:
		case EECLogVerbosity::Normal:
			UE_LOG(LogErrorCodes, Display, TEXT("%s"), *ResultCode.ToString());
			return;
		case EECLogVerbosity::Warning:
			UE_LOG(LogErrorCodes, Warning, TEXT("%s"), *ResultCode.ToString());
			return;
		case EECLogVerbosity::Error:
			UE_LOG(LogErrorCodes, Error, TEXT("%s"), *ResultCode.ToString());
			return;
	}
}

void UECErrorFunctionLibrary::LogResultCodeToMessageLog(EECLogVerbosity Verbosity, FECResultCode ResultCode)
{
	FMessageLog MessageLog("PIE");
	switch (Verbosity)
	{
		default:
		case EECLogVerbosity::Normal:
			MessageLog.Info(ResultCode.GetFormattedMessage());
		return;
		case EECLogVerbosity::Warning:
			MessageLog.Warning(ResultCode.GetFormattedMessage());
		return;
		case EECLogVerbosity::Error:
			MessageLog.Error(ResultCode.GetFormattedMessage());
		return;
	}
}

void UECErrorFunctionLibrary::LogResultCodeToOutputLogIfFailure(EECLogVerbosity Verbosity, FECResultCode ResultCode)
{
	if (ResultCode.IsSuccess())
	{
		return;
	}

	LogResultCodeToOutputLog(Verbosity, ResultCode);
}

void UECErrorFunctionLibrary::LogResultCodeToMessageLogIfFailure(EECLogVerbosity Verbosity, FECResultCode ResultCode)
{
	if (ResultCode.IsSuccess())
	{
		return;
	}

	LogResultCodeToMessageLog(Verbosity, ResultCode);
}

bool UECErrorFunctionLibrary::Equal_ResultCodeResultCode(FECResultCode A, FECResultCode B)
{
	return A == B;
}

bool UECErrorFunctionLibrary::NotEqual_ResultCodeResultCode(FECResultCode A, FECResultCode B)
{
	return A != B;
}

FText UECErrorFunctionLibrary::GetMessage(FECResultCode ResultCode)
{
	return ResultCode.GetMessage();
}

FText UECErrorFunctionLibrary::GetTitle(FECResultCode ResultCode)
{
	return ResultCode.GetTitle();
}

bool UECErrorFunctionLibrary::IsSuccess(FECResultCode ResultCode)
{
	return ResultCode.IsSuccess();
}

bool UECErrorFunctionLibrary::IsFailure(FECResultCode ResultCode)
{
	return ResultCode.IsFailure();
}

bool UECErrorFunctionLibrary::HasValidError(FECResultCode ResultCode)
{
	return ResultCode.HasValidError();
}

bool UECErrorFunctionLibrary::IsValid(FECResultCode ResultCode)
{
	return ResultCode.IsValid();
}

FString UECErrorFunctionLibrary::ToShortString(FECResultCode ResultCode)
{
	return ResultCode.ToShortString();
}

FString UECErrorFunctionLibrary::Conv_ErrorCodeToString(FECResultCode ResultCode)
{
	return ResultCode.ToString();
}
