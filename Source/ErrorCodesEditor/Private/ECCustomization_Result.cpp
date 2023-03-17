// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECCustomization_Result.h"

#include "DetailWidgetRow.h"
#include "ECEditorLogging.h"
#include "ECResult.h"
#include "SECWidget_Result.h"

TSharedRef<IPropertyTypeCustomization> FECCustomization_Result::MakeInstance()
{
	return MakeShareable(new FECCustomization_Result());
}

void FECCustomization_Result::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
	FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& CustomizationUtils
	)
{
	StructProperty = PropertyHandle;
	CategoryProperty = PropertyHandle->GetChildHandle(FECResult::GetPropertyName_Category());
	CodeProperty = PropertyHandle->GetChildHandle(FECResult::GetPropertyName_Code());

	// E.g., if the property receives a paste, we must sync the dropdown widget.
	FSimpleDelegate OnErrorCodeChanged = FSimpleDelegate::CreateSP(this,
		&FECCustomization_Result::SyncErrorCodeWidgetToProperty);
	PropertyHandle->SetOnPropertyValueChanged(OnErrorCodeChanged);

	FECResult DefaultValue;
	UObject* CategoryObj = nullptr;
	int64 Code = -1;
	CategoryProperty->GetValue(CategoryObj);
	CodeProperty->GetValue(Code);
	UEnum* CategoryEnum = Cast<UEnum>(CategoryObj);
	// Leave values as invalid if they were defined that way; this is clearer than converting to 'Success'
	DefaultValue = FECResult::ConstructRaw(CategoryEnum, Code);

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
				SAssignNew(ErrorCodeWidget, SECWidget_Result)
				.PostResultCodeChanged(this, &FECCustomization_Result::UpdateErrorCodeProperties)
				.DefaultValue(DefaultValue)
			]
		];
}

void FECCustomization_Result::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder,
	IPropertyTypeCustomizationUtils& CustomizationUtils
	)
{
	
}

void FECCustomization_Result::UpdateErrorCodeProperties(FECResult NewErrorCode)
{
	if (!StructProperty.IsValid() || !StructProperty->GetProperty())
	{
		UE_LOG(LogErrorCodesEditor, Error, TEXT("Invalid category property or code property."));
		return;
	}

	TArray<void*> RawData;
	StructProperty->AccessRawData(RawData);
	FECResult* PreviousErrorCode = static_cast<FECResult*>(RawData[0]);

	FString TextValue;
	FECResult::StaticStruct()->ExportText(TextValue, &NewErrorCode, PreviousErrorCode, nullptr, PPF_None, nullptr);
	ensure(StructProperty->SetValueFromFormattedString(TextValue) == FPropertyAccess::Success);
}

void FECCustomization_Result::SyncErrorCodeWidgetToProperty()
{
	if (StructProperty.IsValid() && StructProperty->GetProperty())
	{
		TArray<void*> RawStructData;
		StructProperty->AccessRawData(RawStructData);
		if (RawStructData.Num() > 0)
		{
			FECResult* ErrorCodePtr = static_cast<FECResult*>(RawStructData[0]);
			if (ErrorCodePtr && ErrorCodeWidget)
			{
				ErrorCodeWidget->SetSelectedResultCode(*ErrorCodePtr);
			}
		}
	}
}
