// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ECFactory_ErrorCategory.generated.h"

/**
 * Factory for creating Error Categories.
 */
UCLASS()
class ERRORCODESEDITOR_API UECFactory_ErrorCategory : public UFactory
{
	GENERATED_BODY()

public:
	UECFactory_ErrorCategory(const FObjectInitializer& ObjectInitializer);
	
	virtual UObject* FactoryCreateNew(UClass* Class,
		UObject* InParent,
		FName Name,
		EObjectFlags Flags,
		UObject* Context,
		FFeedbackContext* Warn
	) override;
};
