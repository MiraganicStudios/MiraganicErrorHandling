// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.


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
