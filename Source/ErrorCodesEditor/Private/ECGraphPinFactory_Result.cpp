// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECGraphPinFactory_Result.h"

#include "ECResult.h"
#include "SECGraphPin_Result.h"

TSharedPtr<SGraphPin> FECGraphPinFactory_Result::CreatePin(UEdGraphPin* Pin) const
{
	if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct && Pin->PinType.PinSubCategoryObject == FECResult::StaticStruct())
	{
		return SNew(SECGraphPin_Result, Pin);
	}
	
	return nullptr;
}
