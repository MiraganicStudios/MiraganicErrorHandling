// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ECLogging.h"
#include "Logging/TokenizedMessage.h"

DEFINE_LOG_CATEGORY(LogErrorHandling);

EMessageSeverity::Type Mgnc::Log::ToMessageVerbosity(EECLogVerbosity Verbosity)
{
	switch (Verbosity)
	{
		case EECLogVerbosity::Error:
			return EMessageSeverity::Error;
		case EECLogVerbosity::Warning:
			return EMessageSeverity::Warning;
		case EECLogVerbosity::Display:
		case EECLogVerbosity::Log:
		case EECLogVerbosity::Verbose:
		case EECLogVerbosity::VeryVerbose:
		default:
			return EMessageSeverity::Info;
	}
}

void Mgnc::Log::LogWithVerbosity(
	FStringView FuncName,
	FStringView Msg,
	ELogVerbosity::Type Verbosity
)
{
	// Log macro expects compile-time constant verbosity
	switch (Verbosity)
	{
		case ELogVerbosity::Fatal:
			UE_LOG(LogErrorHandling, Fatal, TEXT("%s: %s"), FuncName.GetData(), Msg.GetData());
			break;
		case ELogVerbosity::Error:
			UE_LOG(LogErrorHandling, Error, TEXT("%s: %s"), FuncName.GetData(), Msg.GetData());
			break;
		case ELogVerbosity::Warning:
			UE_LOG(LogErrorHandling, Warning, TEXT("%s: %s"), FuncName.GetData(), Msg.GetData());
			break;
		case ELogVerbosity::Display:
			UE_LOG(LogErrorHandling, Display, TEXT("%s: %s"), FuncName.GetData(), Msg.GetData());
			break;
		case ELogVerbosity::Log:
			UE_LOG(LogErrorHandling, Log, TEXT("%s: %s"), FuncName.GetData(), Msg.GetData());
			break;
		case ELogVerbosity::Verbose:
			UE_LOG(LogErrorHandling, Verbose, TEXT("%s: %s"), FuncName.GetData(), Msg.GetData());
			break;
		case ELogVerbosity::VeryVerbose:
			UE_LOG(LogErrorHandling, VeryVerbose, TEXT("%s: %s"), FuncName.GetData(), Msg.GetData());
			break;
		default:
			ensureAlwaysMsgf(false, TEXT("Unknown log verbosity for message: %s: %s"), FuncName.GetData(), Msg.GetData());
			break;
	}
}

void Mgnc::Log::LogFormattedWithVerbosity(
	FStringView FuncName,
	FStringView MsgFormat,
	const FStringFormatOrderedArguments& Args,
	ELogVerbosity::Type Verbosity
)
{
	// Log macro expects compile-time constant verbosity
	switch (Verbosity)
	{
		case ELogVerbosity::Fatal:
			UE_LOG(LogErrorHandling, Fatal, TEXT("%s: %s"), FuncName.GetData(), *FString::Format(MsgFormat.GetData(), Args));
			break;
		case ELogVerbosity::Error:
			UE_LOG(LogErrorHandling, Error, TEXT("%s: %s"), FuncName.GetData(), *FString::Format(MsgFormat.GetData(), Args));
			break;
		case ELogVerbosity::Warning:
			UE_LOG(LogErrorHandling, Warning, TEXT("%s: %s"), FuncName.GetData(), *FString::Format(MsgFormat.GetData(), Args));
			break;
		case ELogVerbosity::Display:
			UE_LOG(LogErrorHandling, Display, TEXT("%s: %s"), FuncName.GetData(), *FString::Format(MsgFormat.GetData(), Args));
			break;
		case ELogVerbosity::Log:
			UE_LOG(LogErrorHandling, Log, TEXT("%s: %s"), FuncName.GetData(), *FString::Format(MsgFormat.GetData(), Args));
			break;
		case ELogVerbosity::Verbose:
			UE_LOG(LogErrorHandling, Verbose, TEXT("%s: %s"), FuncName.GetData(), *FString::Format(MsgFormat.GetData(), Args));
			break;
		case ELogVerbosity::VeryVerbose:
			UE_LOG(LogErrorHandling, VeryVerbose, TEXT("%s: %s"), FuncName.GetData(), *FString::Format(MsgFormat.GetData(), Args));
			break;
		default:
			ensureAlwaysMsgf(false, TEXT("Unknown log verbosity for message: %s: %s"), FuncName.GetData(), *FString::Format(MsgFormat.GetData(), Args));
			break;
	}
}

void Mgnc::Log::LogToMessageLog(
	const FText& Msg,
	EMessageSeverity::Type Verbosity
)
{
	FMessageLog MessageLog(TEXT("PIE"));
	MessageLog.AddMessage(FTokenizedMessage::Create(Verbosity, Msg));
}
