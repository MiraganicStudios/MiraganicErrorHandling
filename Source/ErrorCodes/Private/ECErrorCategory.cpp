// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECErrorCategory.h"

UECErrorCategory::UECErrorCategory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

FString UECErrorCategory::GetTrimmedName() const
{
	FString Name = GetName();
	Name.RemoveFromStart(TEXT("Default__"));
	Name.RemoveFromEnd(TEXT("_C"));
	return MoveTemp(Name);
}

FText UECErrorCategory::GetErrorMessage(int64 ErrorCode) const
{
	const FECErrorCodeData* ErrorDataPtr = Errors.Find(ErrorCode);
	if (!ErrorDataPtr)
	{
		return FText();
	}

	return ErrorDataPtr->Message;
}

FText UECErrorCategory::GetErrorTitle(int64 ErrorCode) const
{
	const FECErrorCodeData* ErrorDataPtr = Errors.Find(ErrorCode);
	if (!ErrorDataPtr)
	{
		return FText();
	}

	return ErrorDataPtr->Title;
}
