// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECGraphPinFactory_ErrorCode.h"

#include "ECErrorCode.h"
#include "SECErrorCodeGraphPin.h"

TSharedPtr<SGraphPin> FECGraphPinFactory_ErrorCode::CreatePin(UEdGraphPin* Pin) const
{
	if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct && Pin->PinType.PinSubCategoryObject == FECErrorCode::StaticStruct())
	{
		return SNew(SECErrorCodeGraphPin, Pin);
	}
	
	return nullptr;
}
