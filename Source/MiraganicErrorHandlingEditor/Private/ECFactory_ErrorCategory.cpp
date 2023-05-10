// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECFactory_ErrorCategory.h"

#include "ECErrorCategory.h"
#include "ECErrorCategoryUtils.h"

UECFactory_ErrorCategory::UECFactory_ErrorCategory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UECErrorCategory::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

FText UECFactory_ErrorCategory::GetDisplayName() const
{
	return NSLOCTEXT("ErrorCodes", "ErrorEnum_DisplayName", "ErrorCategory");
}

UObject* UECFactory_ErrorCategory::FactoryCreateNew(UClass* InClass,
	UObject* InParent,
	FName InName,
	EObjectFlags Flags,
	UObject* Context,
	FFeedbackContext* Warn
	)
{
	UECErrorCategory* ErrorCategory = NewObject<UECErrorCategory>(InParent, InName, Flags);
	if (ErrorCategory)
	{
		TArray<TPair<FName, int64>> EmptyNames;
		ErrorCategory->SetEnums(EmptyNames, UEnum::ECppForm::Namespaced);

		// Enable as an enum in BP
		ErrorCategory->SetMetaData(TEXT("BlueprintType"), TEXT("true"));
		
		// Enable the enum for Results
		ErrorCategory->SetMetaData(TEXT("ErrorCategory"), TEXT("true"));

		// Add the default 'Success' value 
		ErrorHandling::AddSuccessValueToCategory(*ErrorCategory);
	}

	return ErrorCategory;
}
