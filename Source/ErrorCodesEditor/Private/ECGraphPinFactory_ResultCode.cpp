// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECGraphPinFactory_ResultCode.h"

#include "ECResultCode.h"
#include "SECGraphPin_ResultCode.h"

TSharedPtr<SGraphPin> FECGraphPinFactory_ResultCode::CreatePin(UEdGraphPin* Pin) const
{
	if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct && Pin->PinType.PinSubCategoryObject == FECResultCode::StaticStruct())
	{
		return SNew(SECGraphPin_ResultCode, Pin);
	}
	
	return nullptr;
}
