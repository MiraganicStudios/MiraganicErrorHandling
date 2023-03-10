// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECCustomization_ErrorCode.h"

#include "DetailWidgetRow.h"
#include "ECEditorLogging.h"
#include "ECErrorCode.h"
#include "SECWidget_ErrorCode.h"

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
	CategoryProperty = PropertyHandle->GetChildHandle(FECErrorCode::GetPropertyName_Category());
	CodeProperty = PropertyHandle->GetChildHandle(FECErrorCode::GetPropertyName_Code());

	// E.g., if the property receives a paste, we must sync the dropdown widget.
	FSimpleDelegate OnErrorCodeChanged = FSimpleDelegate::CreateSP(this,
		&FECCustomization_ErrorCode::SyncErrorCodeWidgetToProperty);
	PropertyHandle->SetOnPropertyValueChanged(OnErrorCodeChanged);

	FECErrorCode DefaultValue;
	UObject* CategoryObj = nullptr;
	int64 Code = -1;
	CategoryProperty->GetValue(CategoryObj);
	CodeProperty->GetValue(Code);
	UEnum* CategoryEnum = Cast<UEnum>(CategoryObj);
	// Leave values as invalid if they were defined that way; this is clearer than converting to 'Success'
	DefaultValue = FECErrorCode::ConstructRaw(CategoryEnum, Code);

	HeaderRow
		.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MinDesiredWidth(175.f)
		.MaxDesiredWidth(400.f)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.Padding(0.f, 2.f, 2.f, 2.f)
			[
				SAssignNew(ErrorCodeWidget, SECWidget_ErrorCode)
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

void FECCustomization_ErrorCode::UpdateErrorCodeProperties(FECErrorCode NewErrorCode)
{
	if (!StructProperty.IsValid() || !StructProperty->GetProperty())
	{
		UE_LOG(LogErrorCodesEditor, Error, TEXT("Invalid category property or code property."));
		return;
	}

	TArray<void*> RawData;
	StructProperty->AccessRawData(RawData);
	FECErrorCode* PreviousErrorCode = static_cast<FECErrorCode*>(RawData[0]);

	FString TextValue;
	FECErrorCode::StaticStruct()->ExportText(TextValue, &NewErrorCode, PreviousErrorCode, nullptr, PPF_None, nullptr);
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
			FECErrorCode* ErrorCodePtr = static_cast<FECErrorCode*>(RawStructData[0]);
			if (ErrorCodePtr && ErrorCodeWidget)
			{
				ErrorCodeWidget->SetSelectedErrorCode(*ErrorCodePtr);
			}
		}
	}
}
