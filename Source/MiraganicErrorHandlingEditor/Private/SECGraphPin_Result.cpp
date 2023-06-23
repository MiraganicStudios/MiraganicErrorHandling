// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.


#include "SECGraphPin_Result.h"

#include "ECResult.h"
#include "SECWidget_Result.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SECGraphPin_Result::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

TSharedRef<SWidget> SECGraphPin_Result::GetDefaultValueWidget()
{
	FString DefaultString = GraphPinObj->GetDefaultAsString();
	FECResult DefaultResult;

	UScriptStruct* PinStructType = FECResult::StaticStruct();
	if (!DefaultString.IsEmpty())
	{
		PinStructType->ImportText(*DefaultString, &DefaultResult, nullptr,
			EPropertyPortFlags::PPF_SerializedAsImportText, GError, PinStructType->GetName(), true);
	}

	return SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SECWidget_Result)
			.PostResultChanged(this, &SECGraphPin_Result::UpdatePinValue)
			.DefaultValue(DefaultResult)
			.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
			.IsEnabled(this, &SGraphPin::GetDefaultValueIsEditable)
		];
}

void SECGraphPin_Result::UpdatePinValue(FECResult NewValue)
{
	FString StringValue;
	FECResult DefaultValue;
	FECResult::StaticStruct()->ExportText(StringValue, &NewValue, &DefaultValue, nullptr,
		EPropertyPortFlags::PPF_SerializedAsImportText, nullptr);

	if (StringValue != GraphPinObj->GetDefaultAsString())
	{
		const FScopedTransaction Transaction(NSLOCTEXT("GraphEditor", "ChangePinValue", "Change Pin Value"));
		GraphPinObj->Modify();
		GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, StringValue);
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
