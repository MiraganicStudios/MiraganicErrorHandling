// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ECFactory_ErrorCategory.generated.h"

/**
 * 
 */
UCLASS()
class UECFactory_ErrorCategory : public UFactory
{
	GENERATED_BODY()

public:
	UECFactory_ErrorCategory(const FObjectInitializer& ObjectInitializer);

	virtual FText GetDisplayName() const override;
	virtual UObject* FactoryCreateNew(UClass* InClass,
		UObject* InParent,
		FName InName,
		EObjectFlags Flags,
		UObject* Context,
		FFeedbackContext* Warn
		) override;
};
