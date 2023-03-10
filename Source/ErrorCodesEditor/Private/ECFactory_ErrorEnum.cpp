// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECFactory_ErrorEnum.h"

#include "ECErrorCategoryEnum.h"
#include "Engine/UserDefinedEnum.h"
#include "Kismet2/EnumEditorUtils.h"

UECFactory_ErrorEnum::UECFactory_ErrorEnum(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UECErrorCategoryEnum::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

FText UECFactory_ErrorEnum::GetDisplayName() const
{
	return NSLOCTEXT("ErrorCodes", "ErrorEnum_DisplayName", "ErrorCategory");
}

UObject* UECFactory_ErrorEnum::FactoryCreateNew(UClass* InClass,
	UObject* InParent,
	FName InName,
	EObjectFlags Flags,
	UObject* Context,
	FFeedbackContext* Warn
	)
{
	UEnum* Enum = NewObject<UECErrorCategoryEnum>(InParent, InName, Flags);
	if (Enum)
	{
		TArray<TPair<FName, int64>> EmptyNames;
		Enum->SetEnums(EmptyNames, UEnum::ECppForm::Namespaced);
		
		// Error categories are not exposed as enums at the moment
		// This makes it clearer that they're only intended to be used as error codes
		// We don't have a conversion node for Enum -> ErrorCode for BP yet
		//Enum->SetMetaData(TEXT("BlueprintType"), TEXT("true"));
		
		// Enable the enum for error codes
		Enum->SetMetaData(TEXT("ErrorCategory"), TEXT("true"));
	}

	return Enum;
}
