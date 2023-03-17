// Copyright 2022 Miraganic Studios. All rights reserved.

#include "ECErrorFunctionLibrary.h"

#include "ECLogging.h"

FECResult UECErrorFunctionLibrary::MakeResult(FECResult Result)
{
	return Result;
}

void UECErrorFunctionLibrary::LogResultToOutputLog(EECLogVerbosity Verbosity, FECResult Result)
{
	switch (Verbosity)
	{
		default:
		case EECLogVerbosity::Normal:
			UE_LOG(LogErrorCodes, Display, TEXT("%s"), *Result.ToString());
			return;
		case EECLogVerbosity::Warning:
			UE_LOG(LogErrorCodes, Warning, TEXT("%s"), *Result.ToString());
			return;
		case EECLogVerbosity::Error:
			UE_LOG(LogErrorCodes, Error, TEXT("%s"), *Result.ToString());
			return;
	}
}

void UECErrorFunctionLibrary::LogResultToMessageLog(EECLogVerbosity Verbosity, FECResult Result)
{
	FMessageLog MessageLog("PIE");
	switch (Verbosity)
	{
		default:
		case EECLogVerbosity::Normal:
			MessageLog.Info(Result.GetFormattedMessage());
		return;
		case EECLogVerbosity::Warning:
			MessageLog.Warning(Result.GetFormattedMessage());
		return;
		case EECLogVerbosity::Error:
			MessageLog.Error(Result.GetFormattedMessage());
		return;
	}
}

void UECErrorFunctionLibrary::LogResultToOutputLogIfFailure(EECLogVerbosity Verbosity, FECResult Result)
{
	if (Result.IsSuccess())
	{
		return;
	}

	LogResultToOutputLog(Verbosity, Result);
}

void UECErrorFunctionLibrary::LogResultToMessageLogIfFailure(EECLogVerbosity Verbosity, FECResult Result)
{
	if (Result.IsSuccess())
	{
		return;
	}

	LogResultToMessageLog(Verbosity, Result);
}

bool UECErrorFunctionLibrary::Equal_ResultResult(FECResult A, FECResult B)
{
	return A == B;
}

bool UECErrorFunctionLibrary::NotEqual_ResultResult(FECResult A, FECResult B)
{
	return A != B;
}

FText UECErrorFunctionLibrary::GetMessage(FECResult Result)
{
	return Result.GetMessage();
}

FText UECErrorFunctionLibrary::GetTitle(FECResult Result)
{
	return Result.GetTitle();
}

bool UECErrorFunctionLibrary::IsSuccess(FECResult Result)
{
	return Result.IsSuccess();
}

bool UECErrorFunctionLibrary::IsFailure(FECResult Result)
{
	return Result.IsFailure();
}

bool UECErrorFunctionLibrary::HasValidError(FECResult Result)
{
	return Result.HasValidError();
}

bool UECErrorFunctionLibrary::IsValid(FECResult Result)
{
	return Result.IsValid();
}

FString UECErrorFunctionLibrary::ToShortString(FECResult Result)
{
	return Result.ToShortString();
}

FString UECErrorFunctionLibrary::Conv_ErrorCodeToString(FECResult Result)
{
	return Result.ToString();
}
