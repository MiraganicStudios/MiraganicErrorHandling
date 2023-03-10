// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECGraphNodeFactory_SwitchErrorCode.h"

#include "ECK2Node_SwitchErrorCode.h"
#include "SECGraphNode_SwitchErrorCode.h"

TSharedPtr<SGraphNode> FECGraphNodeFactory_SwitchErrorCode::CreateNode(UEdGraphNode* Node) const
{
	if (UECK2Node_SwitchErrorCode* SwitchNode = Cast<UECK2Node_SwitchErrorCode>(Node))
	{
		return SNew(SECGraphNode_SwitchErrorCode, SwitchNode);
	}

	return nullptr;
}
