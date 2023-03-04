// Copyright 2022 Miraganic Studios. All rights reserved.

#include "MGErrorFunctionLibrary.h"

#include "ECLogging.h"

void UMGErrorFunctionLibrary::LogErrorToOutputLog(EMGLogVerbosity Verbosity, FMGErrorCode Error)
{
	switch (Verbosity)
	{
		default:
		case EMGLogVerbosity::Normal:
			UE_LOG(LogErrorCodes, Display, TEXT("%s"), *Error.ToString());
			return;
		case EMGLogVerbosity::Warning:
			UE_LOG(LogErrorCodes, Warning, TEXT("%s"), *Error.ToString());
			return;
		case EMGLogVerbosity::Error:
			UE_LOG(LogErrorCodes, Error, TEXT("%s"), *Error.ToString());
			return;
	}
}

void UMGErrorFunctionLibrary::LogErrorToMessageLog(EMGLogVerbosity Verbosity, FMGErrorCode Error)
{
	FMessageLog MessageLog("PIE");
	switch (Verbosity)
	{
		default:
		case EMGLogVerbosity::Normal:
			MessageLog.Info(Error.GetErrorMessage());
		return;
		case EMGLogVerbosity::Warning:
			MessageLog.Warning(Error.GetErrorMessage());
		return;
		case EMGLogVerbosity::Error:
			MessageLog.Error(Error.GetErrorMessage());
		return;
	}
}

bool UMGErrorFunctionLibrary::Equal_ErrorCodeErrorCode(FMGErrorCode A, FMGErrorCode B)
{
	return A == B;
}

bool UMGErrorFunctionLibrary::NotEqual_ErrorCodeErrorCode(FMGErrorCode A, FMGErrorCode B)
{
	bool bResult = A != B;
	const FString ResultStr = bResult ? TEXT("true") : TEXT("false");
	UE_LOG(LogErrorCodes, Warning, TEXT("%s: %s != %s -> %s"), *FString(__FUNCTION__), *A.ToShortString(),
		*B.ToShortString(), *ResultStr);
	return A != B;
}

bool UMGErrorFunctionLibrary::NotEqual_ErrorCodeInner(FMGErrorCode A, FMGErrorCode B)
{
	return A != B;
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
