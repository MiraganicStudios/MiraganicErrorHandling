// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include "CoreMinimal.h"
#include "ECResult.generated.h"

/**
 * Generic result which can be a 'Success' or a specific type of error.
 */
USTRUCT(BlueprintType, meta = (DisableSplitPin, 
	HasNativeMake = "MiraganicErrorHandling.ECErrorFunctionLibrary.MakeResult"))
struct MIRAGANICERRORHANDLING_API FECResult
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
		// Set to 'Success' if the code is 0. This allows easy implicit conversion from functions returning
		// enums as errors to FECResult.
		// E.g.,:
		// 
		// enum class EMyResult : uint8
		// {
		//     Success = 0,
		//     ErrorA
		// }
		// FECResult MyWrapperFunction() { return EMyResult::Success; } // Can implicitly convert;
		// EMyResult::Success converts to FECResult::Success
		if (Value == GetSuccessValue())
		{
			Category = nullptr;
		}
	}

	/**
	 * Convert results to other results. E.g., if you're calling a generic function, you might want to convert
	 * its generic errors to more specific errors for additional context.
	 */
	FECResult& Convert(FECResult From, FECResult To);
	
	/**
	 * Map results to other results. E.g., if you're calling a generic function, you might want to map its
	 * generic errors to more specific errors for additional context.
	 * MapFunctor Signature: (FECResult) -> FECResult
	 */
	template<typename MapFuncT, typename = typename TEnableIf<TIsInvocable<MapFuncT, FECResult>::Value>::Type>
	FECResult& Map(MapFuncT&& MapFunctor);

	/**
	 * Invert this result, mapping:
	 * - Any errors -> 'Success'
	 * - 'Success'  -> 'NewError'
	 */
	FECResult& Invert(FECResult NewError);

	/**
	 * Ignore a specific error, converting it into success.
	 */
	FECResult& Ignore(FECResult Error);

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

	const UEnum* GetCategory() const;
	int64 GetValue() const { return Value; }

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
	
	// Construct a 'Success' result code.
	static FECResult Success();

	// Construct a result code using a category and value. This can return an invalid result.
	static FECResult ConstructRaw(const UEnum* InCategory, int64 InValue);

	// Get the value that's reserved for 'Success'
	static int64 GetSuccessValue();

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

template <typename MapFuncT, typename>
FECResult& FECResult::Map(MapFuncT&& MapFunctor)
{
	*this = Invoke(MapFunctor, *this);
	return *this;
}
