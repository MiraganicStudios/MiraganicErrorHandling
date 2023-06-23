// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.


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
