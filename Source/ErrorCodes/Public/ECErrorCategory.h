// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ECErrorCategory.generated.h"

USTRUCT(BlueprintType)
struct ERRORCODES_API FECErrorCodeData
{
	GENERATED_BODY()

public:
	FECErrorCodeData() {}

	FECErrorCodeData(const FText& InTitle, const FText& InMessage)
		: Title(InTitle)
		, Message(InMessage)
	{}

	/**
	 * Short title of the error.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Errors")
	FText Title;

	/**
	 * Error message displayed in logs.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Errors")
	FText Message;
};

/**
 * Represents a category of errors. Stores error messages for each error code.
 */
UCLASS(Const, Blueprintable, BlueprintType)
class ERRORCODES_API UECErrorCategory : public UObject
{
	GENERATED_BODY()

public:
	UECErrorCategory(const FObjectInitializer& ObjectInitializer);

	FString GetTrimmedName() const;
	
	FText GetErrorMessage(int64 ErrorCode) const;
	FText GetErrorTitle(int64 ErrorCode) const;

	/** All errors in this category. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Errors")
	TMap<int64, FECErrorCodeData> Errors;
};
