// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECGraphNodeFactory_SwitchResultCode.h"

#include "ECK2Node_SwitchResultCode.h"
#include "SECGraphNode_SwitchResultCode.h"

TSharedPtr<SGraphNode> FECGraphNodeFactory_SwitchResultCode::CreateNode(UEdGraphNode* Node) const
{
	if (UECK2Node_SwitchResultCode* SwitchNode = Cast<UECK2Node_SwitchResultCode>(Node))
	{
		return SNew(SECGraphNode_SwitchResultCode, SwitchNode);
	}

	return nullptr;
}
