﻿// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECCustomization_ErrorCode.h"

#include "DetailWidgetRow.h"
#include "ECEditorLogging.h"
#include "MGErrorCategory.h"
#include "MGErrorCode.h"
#include "SECErrorCodeWidget.h"

TSharedRef<IPropertyTypeCustomization> FECCustomization_ErrorCode::MakeInstance()
{
	return MakeShareable(new FECCustomization_ErrorCode());
}

void FECCustomization_ErrorCode::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
	FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& CustomizationUtils
	)
{
	StructProperty = PropertyHandle;
	CategoryProperty = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMGErrorCode, Category));
	CodeProperty = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMGErrorCode, Code));

	// E.g., if the property receives a paste, we must sync the dropdown widget.
	FSimpleDelegate OnErrorCodeChanged = FSimpleDelegate::CreateSP(this,
		&FECCustomization_ErrorCode::SyncErrorCodeWidgetToProperty);
	PropertyHandle->SetOnPropertyValueChanged(OnErrorCodeChanged);

	FMGErrorCode DefaultValue;
	UObject* CategoryObj = nullptr;
	int64 Code = 0;
	CategoryProperty->GetValue(CategoryObj);
	CodeProperty->GetValue(Code);
	const UMGErrorCategory* Category = Cast<UMGErrorCategory>(CategoryObj);
	if (Category && Code > 0)
	{
		DefaultValue = FMGErrorCode(*Category, Code);
	}

	HeaderRow
		.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MinDesiredWidth(500)
		.MaxDesiredWidth(4096)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.Padding(0.f, 0.f, 2.f, 0.f)
			[
				SAssignNew(ErrorCodeWidget, SECErrorCodeWidget)
				.PostErrorCodeChanged(this, &FECCustomization_ErrorCode::UpdateErrorCodeProperties)
				.DefaultValue(DefaultValue)
			]
		];
}

void FECCustomization_ErrorCode::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder,
	IPropertyTypeCustomizationUtils& CustomizationUtils
	)
{
	
}

void FECCustomization_ErrorCode::UpdateErrorCodeProperties(FMGErrorCode NewErrorCode)
{
	if (!StructProperty.IsValid() || !StructProperty->GetProperty())
	{
		UE_LOG(LogErrorCodesEditor, Error, TEXT("Invalid category property or code property."));
		return;
	}

	TArray<void*> RawData;
	StructProperty->AccessRawData(RawData);
	FMGErrorCode* PreviousErrorCode = static_cast<FMGErrorCode*>(RawData[0]);

	FString TextValue;
	FMGErrorCode::StaticStruct()->ExportText(TextValue, &NewErrorCode, PreviousErrorCode, nullptr, PPF_None, nullptr);
	ensure(StructProperty->SetValueFromFormattedString(TextValue) == FPropertyAccess::Success);
}

void FECCustomization_ErrorCode::SyncErrorCodeWidgetToProperty()
{
	if (StructProperty.IsValid() && StructProperty->GetProperty())
	{
		TArray<void*> RawStructData;
		StructProperty->AccessRawData(RawStructData);
		if (RawStructData.Num() > 0)
		{
			FMGErrorCode* ErrorCodePtr = static_cast<FMGErrorCode*>(RawStructData[0]);
			if (ErrorCodePtr && ErrorCodeWidget)
			{
				ErrorCodeWidget->SetCurrentValue(*ErrorCodePtr);
			}
		}
	}
}