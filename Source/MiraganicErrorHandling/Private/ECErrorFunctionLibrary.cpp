// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ECErrorFunctionLibrary.h"

#include "ECErrorMacros.h"
#include "ECLogging.h"
#include "Logging/MessageLog.h"
#include "Misc/RuntimeErrors.h"

FECResult UECErrorFunctionLibrary::MakeResult(FECResult Result)
{
	return Result;
}

void UECErrorFunctionLibrary::LogResultToOutputLog(EECLogVerbosity Verbosity, FECResult Result)
{
	Mgnc::Log::LogWithVerbosity(EC_FUNCNAME, Result.ToString(), Mgnc::Log::ToLogVerbosity(Verbosity));
}

void UECErrorFunctionLibrary::LogResultToMessageLog(EECLogVerbosity Verbosity, FECResult Result)
{
	Mgnc::Log::LogToMessageLog(Result.GetFormattedMessage(), Mgnc::Log::ToMessageVerbosity(Verbosity));
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

FECResult UECErrorFunctionLibrary::EnumToResult(const UEnum* Enum, uint8 EnumValue)
{
	if (!::IsValid(Enum))
	{
		UE_LOG(LogErrorHandling, Warning, TEXT("%s: Invalid enum ('EnumToResult' node was invalidated, usually due to error category being removed)."), EC_FUNCNAME);
	}
	if (EnumValue == FECResult::GetSuccessValue())
	{
		return FECResult::Success();
	}
	
	return FECResult::ConstructRaw(Enum, static_cast<int64>(EnumValue));
}
