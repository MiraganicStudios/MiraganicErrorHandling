// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECErrorCode.h"
#include "ECErrorCategory.h"

#define LOCTEXT_NAMESPACE "ErrorCodes"

FECErrorCode::FECErrorCode()
	: CategoryClass(nullptr)
	, Code(0)
{}

FECErrorCode::FECErrorCode(const TSubclassOf<UECErrorCategory>& InCategory, int64 InCode)
	: CategoryClass(InCategory)
	, Code(InCode)
{}

bool FECErrorCode::IsSuccess() const
{
	return Code == 0 && !::IsValid(CategoryClass);
}

bool FECErrorCode::IsError() const
{
	return Code > 0 && ::IsValid(CategoryClass) && CategoryClass.GetDefaultObject()->HasError(Code);
}

bool FECErrorCode::IsValid() const
{
	// Other states (E.g., {Category:Null, Code > 0}, {Category:Valid, Code == 0} are invalid)
	return IsSuccess() || IsError();
}

FString FECErrorCode::GetTrimmedCategoryName() const
{
	if (!IsError())
	{
		return FString();
	}

	const UECErrorCategory* Category = GetCategory();
	return Category->GetTrimmedName(); 
}

FText FECErrorCode::GetFormattedMessage() const
{
	if (IsSuccess())
	{
		return LOCTEXT("ErrorCode_Msg_Success", "Success");
	}
	else if (IsError())
	{
		const UECErrorCategory* Category = GetCategory();
		const FECErrorCodeData* ErrorCodeData = Category->GetErrorData(Code);
		return FText::Format(LOCTEXT("ErrorCode_Msg_ErrorFmt", "{0}:{1}: {2}"),
			{FText::FromString(Category->GetTrimmedName()), ErrorCodeData->Title, ErrorCodeData->Message});
	}
	else
	{
		return FText::Format(LOCTEXT("ErrorCode_Msg_InvalidFmt", "Invalid error code: [{0}, {1}]"),
			{FText::FromString(GetNameSafe(CategoryClass)), Code});
	}
}

FText FECErrorCode::GetMessage() const
{
	if (IsSuccess())
	{
		return LOCTEXT("ErrorCode_Msg_Success", "Success");
	}
	else if (IsError())
	{
		const UECErrorCategory* Category = GetCategory();
		return Category->GetErrorMessage(Code);
	}
	else
	{
		return LOCTEXT("ErrorCode_Msg_Invalid", "Invalid error code");
	}
}

FText FECErrorCode::GetTitle() const
{
	if (IsSuccess())
	{
		return LOCTEXT("ErrorCode_Title_Success", "Success");
	}
	else if (IsError())
	{
		const UECErrorCategory* Category = GetCategory();
		return Category->GetErrorTitle(Code);
	}
	else
	{
		return LOCTEXT("ErrorCode_Title_Invalid", "Invalid");
	}
}

FString FECErrorCode::ToShortString() const
{
	if (IsSuccess())
	{
		return TEXT("Success");
	}
	else if (IsError())
	{
		const UECErrorCategory* Category = GetCategory();
		return FString::Format(TEXT("{0}:{1}"), {*Category->GetTrimmedName(), *Category->GetErrorTitle(Code).ToString()});
	}
	else
	{
		return TEXT("Invalid");
	}
}

FString FECErrorCode::ToString() const
{
	if (IsSuccess())
	{
		return TEXT("Success");
	}
	else if (IsError())
	{
		const UECErrorCategory* Category = GetCategory();
		const FECErrorCodeData* ErrorData = Category->GetErrorData(Code);
		return FString::Format(TEXT("{0}:{1}: {2}"), {*Category->GetTrimmedName(),
			*ErrorData->Title.ToString(), *ErrorData->Message.ToString()});
	}
	else
	{
		return FString::Format(TEXT("Invalid error code: [{0}, {1}]"), {*GetNameSafe(CategoryClass), Code});
	}
}

FECErrorCode FECErrorCode::Success()
{
	return FECErrorCode(nullptr, 0);
}

const UECErrorCategory* FECErrorCode::GetCategory() const
{
	if (!::IsValid(CategoryClass))
	{
		return nullptr;
	}
	
	return CategoryClass->GetDefaultObject<UECErrorCategory>();
}

bool FECErrorCode::operator==(const FECErrorCode& Other) const
{
	return CategoryClass == Other.CategoryClass && Code == Other.Code;
}

FName FECErrorCode::GetPropertyName_Category()
{
	return GET_MEMBER_NAME_CHECKED(FECErrorCode, CategoryClass);
}

FName FECErrorCode::GetPropertyName_Code()
{
	return GET_MEMBER_NAME_CHECKED(FECErrorCode, Code);
}

#undef LOCTEXT_NAMESPACE
