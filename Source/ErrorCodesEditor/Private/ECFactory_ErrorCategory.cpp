// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECFactory_ErrorCategory.h"

#include "ECErrorCategory.h"

UECFactory_ErrorCategory::UECFactory_ErrorCategory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UECErrorCategory::StaticClass();
}

UObject* UECFactory_ErrorCategory::FactoryCreateNew(UClass* Class,
	UObject* InParent,
	FName Name,
	EObjectFlags Flags,
	UObject* Context,
	FFeedbackContext* Warn
	)
{
	return NewObject<UECErrorCategory>(InParent, UECErrorCategory::StaticClass(), Name, Flags);
}
