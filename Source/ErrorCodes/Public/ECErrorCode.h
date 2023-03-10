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
	HasNativeMake = "ErrorCodes.ECErrorFunctionLibrary.MakeErrorCode"
	//HasNativeBreak = "ErrorCodes.ECErrorFunctionLibrary.BreakErrorCode")
	))
struct ERRORCODES_API FECErrorCode
{
	GENERATED_BODY()
public:
	// Default construct to a 'No Error' code (AKA 'Success').
	FECErrorCode();

	// Construct an error code using a category class and a code.
	//FECErrorCode(const TSubclassOf<UECErrorCategory>& InCategory, int64 InCode);

	FECErrorCode(const UEnum* InEnum, int64 InCode);

	// Construct an error code from an enum
	FECErrorCode(const UEnum& InEnum, int64 InCode);

	/**
	 * Construct an error code using an error enum.
	 *
	 * Note that this will work for all UENUMs, even if they don't have the metadata 'ErrorCategory'.
	 */
	template<typename T, typename = typename TEnableIf<TIsEnumClass<T>::Value || TIsEnum<T>::Value>::Type>
	FECErrorCode(T InCode)
		: FECErrorCode(*StaticEnum<T>(), static_cast<int64>(InCode))
	{}

	// Check if this is a success (no error code).
	bool IsSuccess() const;
	// Check if this is an error.
	bool IsError() const;
	// Check if this error code is in a valid state (either success or a valid error code).
	bool IsValid() const;
	// Get the error index for this code, or None if this does not contain an error.
	TOptional<int32> GetErrorIndex() const;
	// Get the error index reserved for the 'MAX' value, or None if this does not contain an error.
	TOptional<int32> GetMaxErrorIndex() const;
	// Get this error code's category name with suffixes/prefixes trimmed.
	FText GetCategoryName() const;
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

	const UEnum* GetCategory() const;
	int64 GetCode() const { return Code; }

	bool operator==(const FECErrorCode& Other) const;
	FORCEINLINE bool operator!=(const FECErrorCode& Other) const
	{
		return !(*this == Other);
	}
	FORCEINLINE friend uint32 GetTypeHash(const FECErrorCode& Elem)
	{
		return HashCombineFast(GetTypeHash(Elem.Enum), GetTypeHash(Elem.Code));
	}

	static FName GetPropertyName_Category();
	static FName GetPropertyName_Code();

protected:

	// Get an enum's display name if building with editor, else use its authored name.
	static FText GetEnumDisplayName(const UEnum& Enum);
	// Get an enum's tooltip, or an empty text if not building with editor.
	static FText GetEnumTooltip(const UEnum& Enum, int32 Index);

	// This error's category object.
	//UPROPERTY(EditAnywhere, Category = "Error")
	//TSubclassOf<UECErrorCategory> CategoryClass;

	// This error's enum object.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Error")
	TObjectPtr<const UEnum> Enum;

	// The code for this error.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Error")
	int64 Code;
};
