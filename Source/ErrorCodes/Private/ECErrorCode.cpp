// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECErrorCode.h"

#define LOCTEXT_NAMESPACE "ErrorCodes"

FECErrorCode::FECErrorCode()
	: Category(nullptr)
	, Code(-1)
{}

FECErrorCode::FECErrorCode(const UEnum* InCategory, int64 InCode)
	: Category(InCategory)
	, Code(InCode)
{
}

FECErrorCode::FECErrorCode(const UEnum& InCategory, int64 InCode)
	: Category(&InCategory)
	, Code(InCode)
{
	ensure(InCode != -1);
}

bool FECErrorCode::IsSuccess() const
{
	return Code == -1 && !::IsValid(Category);
}

bool FECErrorCode::IsError() const
{
	return Code >= 0 && ::IsValid(Category) && !Category->GetNameByValue(Code).IsNone();
}

bool FECErrorCode::IsValid() const
{
	// Other states (E.g., {Category:Null, Code > 0}, {Category:Valid, Code == 0} are invalid)
	return IsSuccess() || IsError();
}

TOptional<int32> FECErrorCode::GetErrorIndex() const
{
	if (Code == -1 || !::IsValid(Category))
	{
		return {};
	}

	const int32 ErrorIdx = Category->GetIndexByValue(Code);
	if (ErrorIdx == -1)
	{
		return {};
	}

	return ErrorIdx;
}

TOptional<int32> FECErrorCode::GetMaxErrorIndex() const
{
	if (Code == -1 || !::IsValid(Category))
	{
		return {};
	}

	return Category->NumEnums();
}

FText FECErrorCode::GetCategoryName() const
{
	if (!IsError())
	{
		return FText();
	}

	return GetEnumDisplayName(*Category);
}

FText FECErrorCode::GetFormattedMessage() const
{
	if (IsSuccess())
	{
		return LOCTEXT("ErrorCode_Msg_Success", "Success");
	}
	else
	{
		const TOptional<int32> OptErrorIdx = GetErrorIndex();
		if (!OptErrorIdx || OptErrorIdx == GetMaxErrorIndex())
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

FText FECErrorCode::GetMessage() const
{
	if (IsSuccess())
	{
		return LOCTEXT("ErrorCode_Msg_Success", "Success");
	}
	else
	{
		const TOptional<int32> OptErrorIdx = GetErrorIndex();
		if (!OptErrorIdx || OptErrorIdx == GetMaxErrorIndex())
		{
			return LOCTEXT("ErrorCode_Msg_Invalid", "Invalid error code");
		}
		else
		{
			return GetEnumTooltip(*Category, *OptErrorIdx);
		}
	}
}

FText FECErrorCode::GetTitle() const
{
	if (IsSuccess())
	{
		return LOCTEXT("ErrorCode_Title_Success", "Success");
	}
	else
	{
		const TOptional<int32> OptErrorIdx = GetErrorIndex();
		if (!OptErrorIdx || OptErrorIdx == GetMaxErrorIndex())
		{
			return LOCTEXT("ErrorCode_Title_Invalid", "[INVALID]");
		}
		else
		{
			return Category->GetDisplayNameTextByIndex(*OptErrorIdx);
		}
	}
}

FString FECErrorCode::ToShortString() const
{
	if (IsSuccess())
	{
		return TEXT("Success");
	}
	else
	{
		const TOptional<int32> OptErrorIdx = GetErrorIndex();
		if (!OptErrorIdx || OptErrorIdx == GetMaxErrorIndex())
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

FString FECErrorCode::ToString() const
{
	if (IsSuccess())
	{
		return TEXT("Success");
	}
	else
	{
		const TOptional<int32> OptErrorIdx = GetErrorIndex();
		if (!OptErrorIdx || OptErrorIdx == GetMaxErrorIndex())
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

FECErrorCode FECErrorCode::Success()
{
	return FECErrorCode();
}

const UEnum* FECErrorCode::GetCategory() const
{
	return Category;
}

FName FECErrorCode::GetPropertyName_Category()
{
	return GET_MEMBER_NAME_CHECKED(FECErrorCode, Category);
}

FName FECErrorCode::GetPropertyName_Code()
{
	return GET_MEMBER_NAME_CHECKED(FECErrorCode, Code);
}

FECErrorCode FECErrorCode::ConstructRaw(const UEnum* InCategory, int64 InCode)
{
	return FECErrorCode(InCategory, InCode);
}

FText FECErrorCode::GetEnumDisplayName(const UEnum& Enum)
{
#if WITH_EDITOR
	return Enum.GetDisplayNameText();
#else
	return FText::FromString(Enum.GetAuthoredName());	
#endif
}

FText FECErrorCode::GetEnumTooltip(const UEnum& Enum, int32 Index)
{
#if WITH_EDITOR
	return Enum.GetToolTipTextByIndex(Index);
#else
	return FText();
#endif
}

#undef LOCTEXT_NAMESPACE
