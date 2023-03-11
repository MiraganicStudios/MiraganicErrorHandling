// Copyright 2022 Miraganic Studios. All rights reserved.


#include "SECGraphPin_ResultCode.h"

#include "ECResultCode.h"
#include "SECWidget_ResultCode.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SECGraphPin_ResultCode::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

TSharedRef<SWidget> SECGraphPin_ResultCode::GetDefaultValueWidget()
{
	FString DefaultString = GraphPinObj->GetDefaultAsString();
	FECResultCode DefaultErrorCode;

	UScriptStruct* PinStructType = FECResultCode::StaticStruct();
	if (!DefaultString.IsEmpty())
	{
		PinStructType->ImportText(*DefaultString, &DefaultErrorCode, nullptr,
			EPropertyPortFlags::PPF_SerializedAsImportText, GError, PinStructType->GetName(), true);
	}

	return SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SECWidget_ResultCode)
			.PostResultCodeChanged(this, &SECGraphPin_ResultCode::UpdatePinValue)
			.DefaultValue(DefaultErrorCode)
			.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
			.IsEnabled(this, &SGraphPin::GetDefaultValueIsEditable)
		];
}

void SECGraphPin_ResultCode::UpdatePinValue(FECResultCode NewValue)
{
	FString StringValue;
	FECResultCode DefaultValue;
	FECResultCode::StaticStruct()->ExportText(StringValue, &NewValue, &DefaultValue, nullptr,
		EPropertyPortFlags::PPF_SerializedAsImportText, nullptr);

	if (StringValue != GraphPinObj->GetDefaultAsString())
	{
		const FScopedTransaction Transaction(NSLOCTEXT("GraphEditor", "ChangePinValue", "Change Pin Value"));
		GraphPinObj->Modify();
		GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, StringValue);
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
