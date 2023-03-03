// Copyright 2022 Miraganic Studios. All rights reserved.


#include "MGErrorCategory.h"

UMGErrorCategory::UMGErrorCategory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

FString UMGErrorCategory::GetTrimmedName() const
{
	FString Name = GetName();
	Name.RemoveFromStart(TEXT("Default__"));
	Name.RemoveFromEnd(TEXT("_C"));
	return MoveTemp(Name);
}

FText UMGErrorCategory::GetErrorMessage(int64 ErrorCode) const
{
	const FMGErrorCodeData* ErrorDataPtr = Errors.Find(ErrorCode);
	if (!ErrorDataPtr)
	{
		return FText();
	}

	return ErrorDataPtr->Message;
}

FText UMGErrorCategory::GetErrorTitle(int64 ErrorCode) const
{
	const FMGErrorCodeData* ErrorDataPtr = Errors.Find(ErrorCode);
	if (!ErrorDataPtr)
	{
		return FText();
	}

	return ErrorDataPtr->Title;
}
