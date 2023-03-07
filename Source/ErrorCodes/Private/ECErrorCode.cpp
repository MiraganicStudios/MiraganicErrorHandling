// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECErrorCode.h"
#include "ECErrorCategory.h"

FECErrorCode::FECErrorCode()
	: CategoryClass(nullptr)
	, Code(0)
{}

// FECErrorCode::FECErrorCode(const UECErrorCategory& InCategory, int64 InCode)
// 	: Category(&InCategory)
// 	, Code(InCode)
// {}

FECErrorCode::FECErrorCode(const TSubclassOf<UECErrorCategory>& InCategory, int64 InCode)
	: CategoryClass(InCategory)
	, Code(InCode)
{}

bool FECErrorCode::IsSuccess() const
{
	return !IsError();
}

bool FECErrorCode::IsError() const
{
	return IsValid(CategoryClass) && Code != 0;
}

FString FECErrorCode::GetTrimmedCategoryName() const
{
	if (IsSuccess())
	{
		return FString();
	}

	const UECErrorCategory* Category = CategoryClass->GetDefaultObject<UECErrorCategory>();
	return Category->GetTrimmedName();
}

FText FECErrorCode::GetErrorMessage() const
{
	if (IsSuccess())
	{
		return FText();
	}

	const UECErrorCategory* Category = CategoryClass->GetDefaultObject<UECErrorCategory>();
	return Category->GetErrorMessage(Code);
}

FText FECErrorCode::GetErrorTitle() const
{
	if (IsSuccess())
	{
		return FText();
	}
	
	const UECErrorCategory* Category = CategoryClass->GetDefaultObject<UECErrorCategory>();
	return Category->GetErrorTitle(Code);
}

FString FECErrorCode::ToShortString() const
{
	if (IsError())
	{
		const UECErrorCategory* Category = CategoryClass->GetDefaultObject<UECErrorCategory>();
		return FString::Printf(TEXT("%s:%s"), *Category->GetTrimmedName(), *Category->GetErrorTitle(Code).ToString());
	}
	else
	{
		return TEXT("Success");
	}
}

FString FECErrorCode::ToString() const
{
	if (IsError())
	{
		const UECErrorCategory* Category = CategoryClass->GetDefaultObject<UECErrorCategory>();
		return FString::Printf(TEXT("%s:%s: %s"), *Category->GetTrimmedName(),
			*Category->GetErrorTitle(Code).ToString(), *Category->GetErrorMessage(Code).ToString());
	}
	else
	{
		return TEXT("Success");
	}
}

FECErrorCode FECErrorCode::Success()
{
	return FECErrorCode();
}

const UECErrorCategory* FECErrorCode::GetCategory() const
{
	if (!IsValid(CategoryClass))
	{
		return nullptr;
	}
	
	return CategoryClass->GetDefaultObject<UECErrorCategory>();
}

bool FECErrorCode::operator==(const FECErrorCode& Other) const
{
	return (IsSuccess() && Other.IsSuccess()) || (CategoryClass == Other.CategoryClass && Code == Other.Code);
}

FName FECErrorCode::GetPropertyName_Category()
{
	return GET_MEMBER_NAME_CHECKED(FECErrorCode, CategoryClass);
}

FName FECErrorCode::GetPropertyName_Code()
{
	return GET_MEMBER_NAME_CHECKED(FECErrorCode, Code);
}
