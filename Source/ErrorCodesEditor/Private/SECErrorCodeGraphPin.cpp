// Copyright 2022 Miraganic Studios. All rights reserved.


#include "SECErrorCodeGraphPin.h"

#include "ECErrorCode.h"
#include "SECErrorCodeWidget.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SECErrorCodeGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

TSharedRef<SWidget> SECErrorCodeGraphPin::GetDefaultValueWidget()
{
	FString DefaultString = GraphPinObj->GetDefaultAsString();
	FECErrorCode DefaultErrorCode;

	UScriptStruct* PinStructType = FECErrorCode::StaticStruct();
	if (!DefaultString.IsEmpty())
	{
		PinStructType->ImportText(*DefaultString, &DefaultErrorCode, nullptr,
			EPropertyPortFlags::PPF_SerializedAsImportText, GError, PinStructType->GetName(), true);
	}

	return SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SECErrorCodeWidget)
			.PostErrorCodeChanged(this, &SECErrorCodeGraphPin::UpdatePinValue)
			.DefaultValue(DefaultErrorCode)
			.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
			.IsEnabled(this, &SGraphPin::GetDefaultValueIsEditable)
		];
}

void SECErrorCodeGraphPin::UpdatePinValue(FECErrorCode NewValue)
{
	FString StringValue;
	FECErrorCode DefaultValue;
	FECErrorCode::StaticStruct()->ExportText(StringValue, &NewValue, &DefaultValue, nullptr,
		EPropertyPortFlags::PPF_SerializedAsImportText, nullptr);

	if (StringValue != GraphPinObj->GetDefaultAsString())
	{
		const FScopedTransaction Transaction(NSLOCTEXT("GraphEditor", "ChangePinValue", "Change Pin Value"));
		GraphPinObj->Modify();
		GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, StringValue);
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
