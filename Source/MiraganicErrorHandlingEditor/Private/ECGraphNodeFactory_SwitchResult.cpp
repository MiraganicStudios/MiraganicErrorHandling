// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECGraphNodeFactory_SwitchResult.h"

#include "ECK2Node_SwitchResult.h"
#include "SECGraphNode_SwitchResult.h"

TSharedPtr<SGraphNode> FECGraphNodeFactory_SwitchResult::CreateNode(UEdGraphNode* Node) const
{
	if (UECK2Node_SwitchResult* SwitchNode = Cast<UECK2Node_SwitchResult>(Node))
	{
		return SNew(SECGraphNode_SwitchResult, SwitchNode);
	}

	return nullptr;
}
