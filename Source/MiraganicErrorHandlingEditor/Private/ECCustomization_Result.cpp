// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.


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
	CodeProperty = PropertyHandle->GetChildHandle(FECResult::GetPropertyName_Value());

	// E.g., if the property receives a paste, we must sync the dropdown widget.
	FSimpleDelegate OnResultChanged = FSimpleDelegate::CreateSP(this,
		&FECCustomization_Result::SyncResultWidgetToProperty);
	PropertyHandle->SetOnPropertyValueChanged(OnResultChanged);

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
				SAssignNew(ResultWidget, SECWidget_Result)
				.PostResultChanged(this, &FECCustomization_Result::UpdateResultProperties)
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

void FECCustomization_Result::UpdateResultProperties(FECResult NewResult)
{
	if (!StructProperty.IsValid() || !StructProperty->GetProperty())
	{
		UE_LOG(LogErrorHandlingEditor, Error, TEXT("Invalid category property or code property."));
		return;
	}

	TArray<void*> RawData;
	StructProperty->AccessRawData(RawData);
	FECResult* PreviousResult = static_cast<FECResult*>(RawData[0]);

	FString TextValue;
	FECResult::StaticStruct()->ExportText(TextValue, &NewResult, PreviousResult, nullptr, PPF_None, nullptr);
	ensure(StructProperty->SetValueFromFormattedString(TextValue) == FPropertyAccess::Success);
}

void FECCustomization_Result::SyncResultWidgetToProperty()
{
	if (StructProperty.IsValid() && StructProperty->GetProperty())
	{
		TArray<void*> RawStructData;
		StructProperty->AccessRawData(RawStructData);
		if (RawStructData.Num() > 0)
		{
			FECResult* ResultPtr = static_cast<FECResult*>(RawStructData[0]);
			if (ResultPtr && ResultWidget)
			{
				ResultWidget->SetSelectedResult(*ResultPtr);
			}
		}
	}
}
