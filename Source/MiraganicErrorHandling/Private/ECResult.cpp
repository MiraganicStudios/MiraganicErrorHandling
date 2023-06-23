// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.


#include "ECResult.h"

#include "ECLogging.h"

#define LOCTEXT_NAMESPACE "ErrorHandling"

FECResult::FECResult()
	: FECResult(Success())
{}

FECResult::FECResult(const UEnum* InCategory, int64 InValue)
	: Category(InCategory)
	, Value(InValue)
{
}

FECResult::FECResult(const UEnum& InCategory, int64 InValue)
	: Category(&InCategory)
	, Value(InValue)
{
	ensure(InValue != GetSuccessValue());
}

FECResult& FECResult::Convert(FECResult From, FECResult To)
{
	if (*this == From)
	{
		*this = To;
	}

	return *this;
}

FECResult& FECResult::Invert(FECResult NewError)
{
	if (IsSuccess())
	{
		*this = NewError;
	}
	else
	{
		*this = Success();
	}
	return *this;
}

FECResult& FECResult::Ignore(FECResult Error)
{
	if (*this == Error)
	{
		*this = Success();
	}
	return *this;
}

bool FECResult::IsSuccess() const
{
	return Value == GetSuccessValue() && !::IsValid(Category);
}

bool FECResult::IsFailure() const
{
	return !IsSuccess();
}

bool FECResult::HasValidError() const
{
	return GetErrorIndex().IsSet();
}

bool FECResult::IsValid() const
{
	// Other states (E.g., {Category:Null, Code > 0}, {Category:Valid, Code == 0} are invalid)
	return IsSuccess() || HasValidError();
}

TOptional<int32> FECResult::GetErrorIndex() const
{
	if (Value == GetSuccessValue() || !::IsValid(Category))
	{
		return {};
	}

	const int32 ErrorIdx = Category->GetIndexByValue(Value);
	const int32 MaxIdx = Category->NumEnums() - 1;
	if (ErrorIdx == -1 || ErrorIdx == MaxIdx)
	{
		return {};
	}

	return ErrorIdx;
}

TOptional<int32> FECResult::GetMaxErrorIndex() const
{
	if (Value == GetSuccessValue() || !::IsValid(Category))
	{
		return {};
	}

	return Category->NumEnums() - 1;
}

FText FECResult::GetCategoryName() const
{
	if (!IsFailure())
	{
		return FText();
	}

	return GetEnumDisplayName(*Category);
}

FText FECResult::GetFormattedMessage() const
{
	if (IsSuccess())
	{
		return LOCTEXT("ErrorCode_Msg_Success", "Success");
	}
	const TOptional<int32> OptErrorIdx = GetErrorIndex();
	if (!OptErrorIdx)
	{
		return FText::Format(LOCTEXT("ErrorCode_Msg_InvalidFmt", "Invalid result: [{0}, {1}]"),
		{FText::FromString(GetNameSafe(Category)), Value});
	}
	
	return FText::Format(LOCTEXT("ErrorCode_Msg_ErrorFmt", "{0}:{1}: {2}"),
		{GetEnumDisplayName(*Category), Category->GetDisplayNameTextByIndex(*OptErrorIdx), GetEnumTooltip(*Category, *OptErrorIdx)});
}

FText FECResult::GetMessage() const
{
	if (IsSuccess())
	{
		return LOCTEXT("ErrorCode_Msg_Success", "Success");
	}
	const TOptional<int32> OptErrorIdx = GetErrorIndex();
	if (!OptErrorIdx)
	{
		return LOCTEXT("ErrorCode_Msg_Invalid", "Invalid result");
	}
	
	return GetEnumTooltip(*Category, *OptErrorIdx);
}

FText FECResult::GetTitle() const
{
	if (IsSuccess())
	{
		return LOCTEXT("ErrorCode_Title_Success", "Success");
	}
	const TOptional<int32> OptErrorIdx = GetErrorIndex();
	if (!OptErrorIdx)
	{
		return LOCTEXT("ErrorCode_Title_Invalid", "[INVALID]");
	}
	
	return Category->GetDisplayNameTextByIndex(*OptErrorIdx);
}

FString FECResult::ToShortString() const
{
	if (IsSuccess())
	{
		return TEXT("Success");
	}
	const TOptional<int32> OptErrorIdx = GetErrorIndex();
	if (!OptErrorIdx)
	{
		return TEXT("Invalid");
	}
	
	return FString::Format(TEXT("{0}:{1}"), {*GetEnumDisplayName(*Category).ToString(),
		*Category->GetDisplayNameTextByIndex(*OptErrorIdx).ToString()});
}

FString FECResult::ToString() const
{
	if (IsSuccess())
	{
		return TEXT("Success");
	}
	const TOptional<int32> OptErrorIdx = GetErrorIndex();
	if (!OptErrorIdx)
	{
		// Invalid result
		return FString::Format(TEXT("Invalid result: [{0}, {1}]"), {*GetNameSafe(Category), Value});
	}

	// Valid error
	const FText EnumDisplayName = GetEnumDisplayName(*Category);
	const FText CodeDisplayName = Category->GetDisplayNameTextByIndex(*OptErrorIdx);
	const FText CodeTooltip = GetEnumTooltip(*Category, *OptErrorIdx);
	return FString::Format(TEXT("{0}:{1}: {2}"), {*EnumDisplayName.ToString(),
		*CodeDisplayName.ToString(), *CodeTooltip.ToString()});
}

FECResult FECResult::Success()
{
	return FECResult(nullptr, GetSuccessValue());
}

const UEnum* FECResult::GetCategory() const
{
	return Category;
}

FName FECResult::GetPropertyName_Category()
{
	return GET_MEMBER_NAME_CHECKED(FECResult, Category);
}

FName FECResult::GetPropertyName_Value()
{
	return GET_MEMBER_NAME_CHECKED(FECResult, Value);
}

FECResult FECResult::ConstructRaw(const UEnum* InCategory, int64 InValue)
{
	return FECResult(InCategory, InValue);
}

int64 FECResult::GetSuccessValue()
{
	return 0;
}

FText FECResult::GetEnumDisplayName(const UEnum& Enum)
{
#if WITH_EDITOR
	return Enum.GetDisplayNameText();
#else
	return FText::FromString(Enum.GetAuthoredName());	
#endif
}

FText FECResult::GetEnumTooltip(const UEnum& Enum, int32 Index)
{
#if WITH_EDITOR
	return Enum.GetToolTipTextByIndex(Index);
#else
	return FText();
#endif
}

#undef LOCTEXT_NAMESPACE
