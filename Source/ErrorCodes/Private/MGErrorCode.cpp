// Copyright 2022 Miraganic Studios. All rights reserved.


#include "MGErrorCode.h"
#include "MGErrorCategory.h"

FMGErrorCode::FMGErrorCode()
	: Category(nullptr)
	, Code(0)
{}

FMGErrorCode::FMGErrorCode(const UMGErrorCategory& InCategory, int64 InCode)
	: Category(&InCategory)
	, Code(InCode)
{}

bool FMGErrorCode::IsSuccess() const
{
	return !IsError();
}

bool FMGErrorCode::IsError() const
{
	return IsValid(Category) && Code != 0;
}

FText FMGErrorCode::GetErrorMessage() const
{
	if (IsSuccess())
	{
		return FText();
	}

	return Category->GetErrorMessage(Code);
}

FText FMGErrorCode::GetErrorTitle() const
{
	if (IsSuccess())
	{
		return FText();
	}
	
	return Category->GetErrorTitle(Code);
}

FString FMGErrorCode::ToString() const
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

FMGErrorCode FMGErrorCode::Success()
{
	return FMGErrorCode();
}
