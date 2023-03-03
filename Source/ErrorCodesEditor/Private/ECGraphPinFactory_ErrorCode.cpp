// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECGraphPinFactory_ErrorCode.h"

#include "MGErrorCode.h"
#include "SECErrorCodeGraphPin.h"

TSharedPtr<SGraphPin> FECGraphPinFactory_ErrorCode::CreatePin(UEdGraphPin* Pin) const
{
	if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct && Pin->PinType.PinSubCategoryObject == FMGErrorCode::StaticStruct())
	{
		return SNew(SECErrorCodeGraphPin, Pin);
	}
	
	return nullptr;
}
