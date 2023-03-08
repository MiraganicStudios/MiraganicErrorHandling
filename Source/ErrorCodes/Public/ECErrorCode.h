// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
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

	// Construct an error code using a category class and a code.
	FECErrorCode(const TSubclassOf<UECErrorCategory>& InCategory, int64 InCode);

	/**
	 * Construct an error code using an error enum.
	 *
	 * To use this constructor, create an overload of 'MakeErrorCode(MyEnumType)' using your enum type and
	 * return the correct error code(s) and categories. Usually this involves static_casting your enum to
	 * uint64 for the code.
	 * The constructor will automatically find your overload and call it.
	 */
	template<typename T, typename = typename TEnableIf<TIsEnumClass<T>::Value || TIsEnum<T>::Value>::Type>
	FECErrorCode(T InCode)
		: FECErrorCode(MakeErrorCode(InCode))
	{}

	// Check if this is a success (no error code).
	bool IsSuccess() const;
	// Check if this is an error.
	bool IsError() const;
	// Check if this error code is in a valid state (either success or a valid error code).
	bool IsValid() const;
	// Get this error code's category name with suffixes/prefixes trimmed.
	FString GetTrimmedCategoryName() const;
	// Format this error code's category, title, and message as text.
	FText GetFormattedMessage() const;
	// Get this error code's message, or 'Success' or 'Invalid'.
	FText GetMessage() const;
	// Get this error code's title, or 'Success' or 'Invalid'.
	FText GetTitle() const;
	// Format this error code's category and title as a string.
	FString ToShortString() const;
	// Format this error code's category, title, and message as a string.
	FString ToString() const;
	// Construct a 'Success' error code.
	static FECErrorCode Success();

	const UECErrorCategory* GetCategory() const;
	int64 GetCode() const { return Code; }

	bool operator==(const FECErrorCode& Other) const;
	FORCEINLINE bool operator!=(const FECErrorCode& Other) const
	{
		return !(*this == Other);
	}
	FORCEINLINE friend uint32 GetTypeHash(const FECErrorCode& Elem)
	{
		return HashCombineFast(GetTypeHash(Elem.CategoryClass), GetTypeHash(Elem.Code));
	}

	static FName GetPropertyName_Category();
	static FName GetPropertyName_Code();

protected:

	// This error's category object.
	UPROPERTY(EditAnywhere, Category = "Error")
	TSubclassOf<UECErrorCategory> CategoryClass;

	// The code for this error.
	UPROPERTY(EditAnywhere, Category = "Error")
	int64 Code;
};
