// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECResultCode.h"

#define LOCTEXT_NAMESPACE "ErrorCodes"

FECResultCode::FECResultCode()
	: Category(nullptr)
	, Code(-1)
{}

FECResultCode::FECResultCode(const UEnum* InCategory, int64 InCode)
	: Category(InCategory)
	, Code(InCode)
{
}

FECResultCode::FECResultCode(const UEnum& InCategory, int64 InCode)
	: Category(&InCategory)
	, Code(InCode)
{
	ensure(InCode != -1);
}

bool FECResultCode::IsSuccess() const
{
	return Code == -1 && !::IsValid(Category);
}

bool FECResultCode::IsFailure() const
{
	return !IsSuccess();
	// if (Code == -1 || !::IsValid(Category))
	// {
	// 	return false;
	// }
	//
	// const int32 Idx = Category->GetIndexByValue(Code);
	// // Subtract 1 to ignore the 'MAX' enum entry.
	// const int32 MaxIdx = Category->NumEnums() - 1;
	// return Idx >= 0 && Idx < MaxIdx;
}

bool FECResultCode::HasValidError() const
{
	return GetErrorIndex().IsSet();
}

bool FECResultCode::IsValid() const
{
	// Other states (E.g., {Category:Null, Code > 0}, {Category:Valid, Code == 0} are invalid)
	return IsSuccess() || HasValidError();
}

TOptional<int32> FECResultCode::GetErrorIndex() const
{
	if (Code == -1 || !::IsValid(Category))
	{
		return {};
	}

	const int32 ErrorIdx = Category->GetIndexByValue(Code);
	const int32 MaxIdx = Category->NumEnums() - 1;
	if (ErrorIdx == -1 || ErrorIdx == MaxIdx)
	{
		return {};
	}

	return ErrorIdx;
}

TOptional<int32> FECResultCode::GetMaxErrorIndex() const
{
	if (Code == -1 || !::IsValid(Category))
	{
		return {};
	}

	return Category->NumEnums() - 1;
}

FText FECResultCode::GetCategoryName() const
{
	if (!IsFailure())
	{
		return FText();
	}

	return GetEnumDisplayName(*Category);
}

FText FECResultCode::GetFormattedMessage() const
{
	if (IsSuccess())
	{
		return LOCTEXT("ErrorCode_Msg_Success", "Success");
	}
	else
	{
		const TOptional<int32> OptErrorIdx = GetErrorIndex();
		if (!OptErrorIdx)
		{
			return FText::Format(LOCTEXT("ErrorCode_Msg_InvalidFmt", "Invalid error code: [{0}, {1}]"),
			{FText::FromString(GetNameSafe(Category)), Code});
		}
		else
		{
			return FText::Format(LOCTEXT("ErrorCode_Msg_ErrorFmt", "{0}:{1}: {2}"),
				{GetEnumDisplayName(*Category), Category->GetDisplayNameTextByIndex(*OptErrorIdx), GetEnumTooltip(*Category, *OptErrorIdx)});
		}
	}
}

FText FECResultCode::GetMessage() const
{
	if (IsSuccess())
	{
		return LOCTEXT("ErrorCode_Msg_Success", "Success");
	}
	else
	{
		const TOptional<int32> OptErrorIdx = GetErrorIndex();
		if (!OptErrorIdx)
		{
			return LOCTEXT("ErrorCode_Msg_Invalid", "Invalid error code");
		}
		else
		{
			return GetEnumTooltip(*Category, *OptErrorIdx);
		}
	}
}

FText FECResultCode::GetTitle() const
{
	if (IsSuccess())
	{
		return LOCTEXT("ErrorCode_Title_Success", "Success");
	}
	else
	{
		const TOptional<int32> OptErrorIdx = GetErrorIndex();
		if (!OptErrorIdx)
		{
			return LOCTEXT("ErrorCode_Title_Invalid", "[INVALID]");
		}
		else
		{
			return Category->GetDisplayNameTextByIndex(*OptErrorIdx);
		}
	}
}

FString FECResultCode::ToShortString() const
{
	if (IsSuccess())
	{
		return TEXT("Success");
	}
	else
	{
		const TOptional<int32> OptErrorIdx = GetErrorIndex();
		if (!OptErrorIdx)
		{
			return TEXT("Invalid");
		}
		else
		{
			return FString::Format(TEXT("{0}:{1}"), {*GetEnumDisplayName(*Category).ToString(),
				*Category->GetDisplayNameTextByIndex(*OptErrorIdx).ToString()});
		}
	}
}

FString FECResultCode::ToString() const
{
	if (IsSuccess())
	{
		return TEXT("Success");
	}
	else
	{
		const TOptional<int32> OptErrorIdx = GetErrorIndex();
		if (!OptErrorIdx)
		{
			// Invalid error code
			return FString::Format(TEXT("Invalid error code: [{0}, {1}]"), {*GetNameSafe(Category), Code});
		}
		else
		{
			// Valid error
			const FText EnumDisplayName = GetEnumDisplayName(*Category);
			const FText CodeDisplayName = Category->GetDisplayNameTextByIndex(*OptErrorIdx);
			const FText CodeTooltip = GetEnumTooltip(*Category, *OptErrorIdx);
			return FString::Format(TEXT("{0}:{1}: {2}"), {*EnumDisplayName.ToString(),
				*CodeDisplayName.ToString(), *CodeTooltip.ToString()});
		}
	}
}

FECResultCode FECResultCode::Success()
{
	return FECResultCode();
}

const UEnum* FECResultCode::GetCategory() const
{
	return Category;
}

FName FECResultCode::GetPropertyName_Category()
{
	return GET_MEMBER_NAME_CHECKED(FECResultCode, Category);
}

FName FECResultCode::GetPropertyName_Code()
{
	return GET_MEMBER_NAME_CHECKED(FECResultCode, Code);
}

FECResultCode FECResultCode::ConstructRaw(const UEnum* InCategory, int64 InCode)
{
	return FECResultCode(InCategory, InCode);
}

FText FECResultCode::GetEnumDisplayName(const UEnum& Enum)
{
#if WITH_EDITOR
	return Enum.GetDisplayNameText();
#else
	return FText::FromString(Enum.GetAuthoredName());	
#endif
}

FText FECResultCode::GetEnumTooltip(const UEnum& Enum, int32 Index)
{
#if WITH_EDITOR
	return Enum.GetToolTipTextByIndex(Index);
#else
	return FText();
#endif
}

#undef LOCTEXT_NAMESPACE
