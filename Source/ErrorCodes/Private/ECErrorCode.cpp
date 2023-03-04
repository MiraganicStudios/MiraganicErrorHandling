// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECErrorCode.h"
#include "ECErrorCategory.h"

FECErrorCode::FECErrorCode()
	: Category(nullptr)
	, Code(0)
{}

FECErrorCode::FECErrorCode(const UECErrorCategory& InCategory, int64 InCode)
	: Category(&InCategory)
	, Code(InCode)
{}

bool FECErrorCode::IsSuccess() const
{
	return !IsError();
}

bool FECErrorCode::IsError() const
{
	return IsValid(Category) && Code != 0;
}

FString FECErrorCode::GetTrimmedCategoryName() const
{
	if (IsSuccess())
	{
		return FString();
	}

	return Category->GetTrimmedName();
}

FText FECErrorCode::GetErrorMessage() const
{
	if (IsSuccess())
	{
		return FText();
	}

	return Category->GetErrorMessage(Code);
}

FText FECErrorCode::GetErrorTitle() const
{
	if (IsSuccess())
	{
		return FText();
	}
	
	return Category->GetErrorTitle(Code);
}

FString FECErrorCode::ToShortString() const
{
	if (IsSuccess())
	{
		return TEXT("Success");
	}
	else
	{
		return FString::Printf(TEXT("%s:%s"), *Category->GetTrimmedName(), *Category->GetErrorTitle(Code).ToString());
	}
}

FString FECErrorCode::ToString() const
{
	if (IsError())
	{
		return FString::Printf(TEXT("%s:%s: %s"), *Category->GetTrimmedName(),
			*Category->GetErrorTitle(Code).ToString(), *Category->GetErrorMessage(Code).ToString());
	}
	else
	{
		return TEXT("ErrorCode{Success}");
	}
}

FECErrorCode FECErrorCode::Success()
{
	return FECErrorCode();
}
