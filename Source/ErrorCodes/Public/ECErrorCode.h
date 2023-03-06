﻿// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ECErrorCode.generated.h"

class UECErrorCategory;

/**
 * Generic error code.
 */
USTRUCT(BlueprintType, meta = (DisableSplitPin, 
	HasNativeMake = "ErrorCodes.ECErrorFunctionLibrary.MakeErrorCode",
	HasNativeBreak = "ErrorCodes.ECErrorFunctionLibrary.BreakErrorCode"))
struct ERRORCODES_API FECErrorCode
{
	GENERATED_BODY()
public:
	// Default construct to a 'No Error' code (AKA 'Success').
	FECErrorCode();
	// Construct an error code using a category and a code.
	FECErrorCode(const UECErrorCategory& InCategory, int64 InCode);
	// Construct an error code using an error enum. To use this constructor, create an overload of
	// 'MakeErrorCode(MyEnumType)' using your enum type and return the correct error code(s) and categories.
	// Usually this involves static_casting your enum to uint64 for the code.
	// The constructor will automatically find your overload and call it.
	template<typename T, typename = typename TEnableIf<TIsEnumClass<T>::Value>::Type>
	FECErrorCode(T InCode)
		: FECErrorCode(MakeErrorCode(InCode))
	{}

	// Check if this is a success (no error code).
	bool IsSuccess() const;
	// Check if this is an error.
	bool IsError() const;
	// Get this error code's category name with suffixes/prefixes trimmed.
	FString GetTrimmedCategoryName() const;
	// Get this error code's message, or empty if this doesn't contain an error.
	FText GetErrorMessage() const;
	// Get this error code's title, or empty if this doesn't contain an error.
	FText GetErrorTitle() const;
	// Format this error code's category and title as a string.
	FString ToShortString() const;
	// Format this error code's category, title, and message as a string.
	FString ToString() const;
	// Construct a 'Success' error code.
	static FECErrorCode Success();

	FORCEINLINE bool operator==(const FECErrorCode& Other) const
	{
		return (IsSuccess() && Other.IsSuccess()) || (Category == Other.Category && Code == Other.Code);
	}
	FORCEINLINE bool operator!=(const FECErrorCode& Other) const
	{
		return !(*this == Other);
	}
	FORCEINLINE friend uint32 GetTypeHash(const FECErrorCode& Elem)
	{
		return HashCombineFast(GetTypeHash(Elem.Category), GetTypeHash(Elem.Code));
	}

	// This error's category object.
	UPROPERTY(EditAnywhere, Category = "Error")
	TObjectPtr<const UECErrorCategory> Category;

	// The code for this error.
	UPROPERTY(EditAnywhere, Category = "Error")
	int64 Code;
};
