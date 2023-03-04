// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ECErrorCode.generated.h"

class UECErrorCategory;

/**
 * Generic error code.
 */
USTRUCT(BlueprintType)
struct ERRORCODES_API FECErrorCode
{
	GENERATED_BODY()
public:
	// Default construct to a 'No Error' code.
	FECErrorCode();
	// Construct an error code using a category and a code.
	FECErrorCode(const UECErrorCategory& InCategory, int64 InCode);
	template<typename T, typename = typename TEnableIf<TIsEnumClass<T>::Value>::Type>
	FECErrorCode(T InCode)
		: FECErrorCode(MakeErrorCode(InCode))
	{}

	bool IsSuccess() const;
	bool IsError() const;
	FString GetTrimmedCategoryName() const;
	FText GetErrorMessage() const;
	FText GetErrorTitle() const;
	// Get this error code string without the message.
	FString ToShortString() const;
	FString ToString() const;
	static FECErrorCode Success();

	FORCEINLINE_DEBUGGABLE bool operator==(const FECErrorCode& Other) const
	{
		return (IsSuccess() && Other.IsSuccess()) || (Category == Other.Category && Code == Other.Code);
	}
	FORCEINLINE_DEBUGGABLE bool operator!=(const FECErrorCode& Other) const
	{
		return !(*this == Other);
	}
	FORCEINLINE friend uint32 GetTypeHash(const FECErrorCode& Elem)
	{
		return HashCombineFast(GetTypeHash(Elem.Category), GetTypeHash(Elem.Code));
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Error")
	TObjectPtr<const UECErrorCategory> Category;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Error")
	int64 Code;
};
