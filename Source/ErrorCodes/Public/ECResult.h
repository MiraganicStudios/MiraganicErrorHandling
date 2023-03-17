// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ECResult.generated.h"

/**
 * Generic result which can be a 'Success' or a specific type of error.
 */
USTRUCT(BlueprintType, meta = (DisableSplitPin, 
	HasNativeMake = "ErrorCodes.ECErrorFunctionLibrary.MakeResult"))
struct ERRORCODES_API FECResult
{
	GENERATED_BODY()
	
public:
	// Default construct to 'Success'.
	FECResult();

	// Construct an error from a category and a code. Prefer using the enum conversion constructor if possible.
	FECResult(const UEnum& InCategory, int64 InValue);

	/**
	 * Construct an result code using an error enum.
	 *
	 * Note that this will work for all UENUMs, even if they don't have the metadata 'ErrorCategory'.
	 */
	template<typename T, typename = typename TEnableIf<TIsEnumClass<T>::Value || TIsEnum<T>::Value>::Type>
	FECResult(T InEnum)
		: Category(StaticEnum<T>())
		, Value(static_cast<int64>(InEnum))
	{
		// Set to 'Success' if the code is -1. This allows easy implicit conversion from functions returning
		// enums as errors to FECResult.
		// E.g.,:
		// 
		// enum class EMyError : int64
		// {
		//     Success = -1,
		//     ErrorA
		// }
		// EMyError MyFunction();
		// FECResult MyWrapperFunction() { return MyFunction(); } // Can implicitly convert; EMyError::Success
		// converts to FECResult::Success
		if (Value == -1)
		{
			Category = nullptr;
		}
	}

	// Check if this is a success.
	bool IsSuccess() const;
	// Check if this is a failure (anything but success).
	bool IsFailure() const;
	// Check if this contains a valid error for its category.
	bool HasValidError() const;
	// Check if this result code is in a valid state (either success or a valid result code).
	bool IsValid() const;
	// Get the error index for this code, or None if this does not contain an error.
	TOptional<int32> GetErrorIndex() const;
	// Get the error index reserved for the 'MAX' value, or None if this does not contain an error.
	TOptional<int32> GetMaxErrorIndex() const;
	// Get this result code's category name with suffixes/prefixes trimmed.
	FText GetCategoryName() const;
	// Format this result code's category, title, and message as text.
	FText GetFormattedMessage() const;
	// Get this result code's message, or 'Success' or 'Invalid'.
	FText GetMessage() const;
	// Get this result code's title, or 'Success' or 'Invalid'.
	FText GetTitle() const;
	// Format this result code's category and title as a string.
	FString ToShortString() const;
	// Format this result code's category, title, and message as a string.
	FString ToString() const;
	// Construct a 'Success' result code.
	static FECResult Success();

	const UEnum* GetCategory() const;
	int64 GetCode() const { return Value; }

	FORCEINLINE bool operator==(const FECResult& Other) const
	{
		return Category == Other.Category && Value == Other.Value;
	}
	FORCEINLINE bool operator!=(const FECResult& Other) const
	{
		return !(*this == Other);
	}
	FORCEINLINE friend uint32 GetTypeHash(const FECResult& Elem)
	{
		return HashCombineFast(GetTypeHash(Elem.Category), GetTypeHash(Elem.Value));
	}

	static FName GetPropertyName_Category();
	static FName GetPropertyName_Value();

	// Construct a result code using a category and value. This can return an invalid result.
	static FECResult ConstructRaw(const UEnum* InCategory, int64 InValue);

protected:
	
	// Construct a result code from an enum
	FECResult(const UEnum* InCategory, int64 InValue);

	// Get an enum's display name if building with editor, else use its authored name.
	static FText GetEnumDisplayName(const UEnum& Enum);
	// Get an enum's tooltip, or an empty text if not building with editor.
	static FText GetEnumTooltip(const UEnum& Enum, int32 Index);

	// This result's category object.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Error")
	TObjectPtr<const UEnum> Category;

	// The value for the result.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Error")
	int64 Value;
};
