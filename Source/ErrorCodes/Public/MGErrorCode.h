// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MGErrorCode.generated.h"

class UMGErrorCategory;

/**
 * Generic error code.
 */
USTRUCT(BlueprintType)
struct ERRORCODES_API FMGErrorCode
{
	GENERATED_BODY()
public:
	// Default construct to a 'No Error' code.
	FMGErrorCode();
	// Construct an error code using a category and a code.
	FMGErrorCode(const UMGErrorCategory& InCategory, int64 InCode);

	bool IsSuccess() const;
	bool IsError() const;
	FText GetErrorMessage() const;
	FText GetErrorTitle() const;
	FString ToString() const;
	static FMGErrorCode Success();

	FORCEINLINE bool operator==(const FMGErrorCode& Other) const
	{
		return Category == Other.Category && Code == Other.Code;
	}
	FORCEINLINE bool operator!=(const FMGErrorCode& Other) const
	{
		return Category != Other.Category || Code != Other.Code;
	}
	FORCEINLINE friend uint32 GetTypeHash(const FMGErrorCode& Elem)
	{
		return HashCombineFast(GetTypeHash(Elem.Category), GetTypeHash(Elem.Code));
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Error")
	TObjectPtr<const UMGErrorCategory> Category;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Error")
	int64 Code;
};
