// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECErrorCode.h"
#include "ECErrorCategory.h"

#define LOCTEXT_NAMESPACE "ErrorCodes"

FECErrorCode::FECErrorCode()
	: Enum(nullptr)
	, Code(0)
{}

FECErrorCode::FECErrorCode(const UEnum* InEnum, int64 InCode)
	: Enum(InEnum)
	, Code(InCode)
{
}

// FECErrorCode::FECErrorCode(const TSubclassOf<UECErrorCategory>& InCategory, int64 InCode)
// 	: CategoryClass(InCategory)
// 	, Enum(nullptr)
// 	, Code(InCode)
// {}

FECErrorCode::FECErrorCode(const UEnum& InEnum, int64 InCode)
	: Enum(&InEnum)
	, Code(InCode)
{
	ensure(InCode != 0);
}

bool FECErrorCode::IsSuccess() const
{
	return Code == 0 && !::IsValid(Enum);
}

bool FECErrorCode::IsError() const
{
	return Code > 0 && ::IsValid(Enum) && !Enum->GetNameByValue(Code).IsNone();
}

bool FECErrorCode::IsValid() const
{
	// Other states (E.g., {Category:Null, Code > 0}, {Category:Valid, Code == 0} are invalid)
	return IsSuccess() || IsError();
}

TOptional<int32> FECErrorCode::GetErrorIndex() const
{
	if (Code == 0 || !::IsValid(Enum))
	{
		return {};
	}

	const int32 ErrorIdx = Enum->GetIndexByValue(Code);
	if (ErrorIdx == -1)
	{
		return {};
	}

	return ErrorIdx;
}

TOptional<int32> FECErrorCode::GetMaxErrorIndex() const
{
	if (Code == 0 || !::IsValid(Enum))
	{
		return {};
	}

	return Enum->NumEnums();
}

FText FECErrorCode::GetCategoryName() const
{
	if (!IsError())
	{
		return FText();
	}

	return GetEnumDisplayName(*Enum);
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
			{FText::FromString(GetNameSafe(Enum)), Code});
		}
		else
		{
			return FText::Format(LOCTEXT("ErrorCode_Msg_ErrorFmt", "{0}:{1}: {2}"),
				{GetEnumDisplayName(*Enum), Enum->GetDisplayNameTextByIndex(*OptErrorIdx), GetEnumTooltip(*Enum, *OptErrorIdx)});
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
			return GetEnumTooltip(*Enum, *OptErrorIdx);
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
			return Enum->GetDisplayNameTextByIndex(*OptErrorIdx);
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
			return FString::Format(TEXT("{0}:{1}"), {*GetEnumDisplayName(*Enum).ToString(),
				*Enum->GetDisplayNameTextByIndex(*OptErrorIdx).ToString()});
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
			return FString::Format(TEXT("Invalid error code: [{0}, {1}]"), {*GetNameSafe(Enum), Code});
		}
		else
		{
			// Valid error
			const FText EnumDisplayName = GetEnumDisplayName(*Enum);
			const FText CodeDisplayName = Enum->GetDisplayNameTextByIndex(*OptErrorIdx);
			const FText CodeTooltip = GetEnumTooltip(*Enum, *OptErrorIdx);
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
	return Enum;
}

bool FECErrorCode::operator==(const FECErrorCode& Other) const
{
	return Enum == Other.Enum && Code == Other.Code;
}

FName FECErrorCode::GetPropertyName_Category()
{
	return GET_MEMBER_NAME_CHECKED(FECErrorCode, Enum);
}

FName FECErrorCode::GetPropertyName_Code()
{
	return GET_MEMBER_NAME_CHECKED(FECErrorCode, Code);
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
