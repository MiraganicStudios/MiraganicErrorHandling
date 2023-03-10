// Copyright 2022 Miraganic Studios. All rights reserved.


#include "SECGraphPin_ErrorCode.h"

#include "ECErrorCode.h"
#include "SECWidget_ErrorCode.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SECGraphPin_ErrorCode::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

TSharedRef<SWidget> SECGraphPin_ErrorCode::GetDefaultValueWidget()
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
			SNew(SECWidget_ErrorCode)
			.PostErrorCodeChanged(this, &SECGraphPin_ErrorCode::UpdatePinValue)
			.DefaultValue(DefaultErrorCode)
			.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
			.IsEnabled(this, &SGraphPin::GetDefaultValueIsEditable)
		];
}

void SECGraphPin_ErrorCode::UpdatePinValue(FECErrorCode NewValue)
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
