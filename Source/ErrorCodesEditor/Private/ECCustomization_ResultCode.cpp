// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECCustomization_ResultCode.h"

#include "DetailWidgetRow.h"
#include "ECEditorLogging.h"
#include "ECResultCode.h"
#include "SECWidget_ResultCode.h"

TSharedRef<IPropertyTypeCustomization> FECCustomization_ResultCode::MakeInstance()
{
	return MakeShareable(new FECCustomization_ResultCode());
}

void FECCustomization_ResultCode::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
	FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& CustomizationUtils
	)
{
	StructProperty = PropertyHandle;
	CategoryProperty = PropertyHandle->GetChildHandle(FECResultCode::GetPropertyName_Category());
	CodeProperty = PropertyHandle->GetChildHandle(FECResultCode::GetPropertyName_Code());

	// E.g., if the property receives a paste, we must sync the dropdown widget.
	FSimpleDelegate OnErrorCodeChanged = FSimpleDelegate::CreateSP(this,
		&FECCustomization_ResultCode::SyncErrorCodeWidgetToProperty);
	PropertyHandle->SetOnPropertyValueChanged(OnErrorCodeChanged);

	FECResultCode DefaultValue;
	UObject* CategoryObj = nullptr;
	int64 Code = -1;
	CategoryProperty->GetValue(CategoryObj);
	CodeProperty->GetValue(Code);
	UEnum* CategoryEnum = Cast<UEnum>(CategoryObj);
	// Leave values as invalid if they were defined that way; this is clearer than converting to 'Success'
	DefaultValue = FECResultCode::ConstructRaw(CategoryEnum, Code);

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
				SAssignNew(ErrorCodeWidget, SECWidget_ResultCode)
				.PostResultCodeChanged(this, &FECCustomization_ResultCode::UpdateErrorCodeProperties)
				.DefaultValue(DefaultValue)
			]
		];
}

void FECCustomization_ResultCode::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder,
	IPropertyTypeCustomizationUtils& CustomizationUtils
	)
{
	
}

void FECCustomization_ResultCode::UpdateErrorCodeProperties(FECResultCode NewErrorCode)
{
	if (!StructProperty.IsValid() || !StructProperty->GetProperty())
	{
		UE_LOG(LogErrorCodesEditor, Error, TEXT("Invalid category property or code property."));
		return;
	}

	TArray<void*> RawData;
	StructProperty->AccessRawData(RawData);
	FECResultCode* PreviousErrorCode = static_cast<FECResultCode*>(RawData[0]);

	FString TextValue;
	FECResultCode::StaticStruct()->ExportText(TextValue, &NewErrorCode, PreviousErrorCode, nullptr, PPF_None, nullptr);
	ensure(StructProperty->SetValueFromFormattedString(TextValue) == FPropertyAccess::Success);
}

void FECCustomization_ResultCode::SyncErrorCodeWidgetToProperty()
{
	if (StructProperty.IsValid() && StructProperty->GetProperty())
	{
		TArray<void*> RawStructData;
		StructProperty->AccessRawData(RawStructData);
		if (RawStructData.Num() > 0)
		{
			FECResultCode* ErrorCodePtr = static_cast<FECResultCode*>(RawStructData[0]);
			if (ErrorCodePtr && ErrorCodeWidget)
			{
				ErrorCodeWidget->SetSelectedResultCode(*ErrorCodePtr);
			}
		}
	}
}
