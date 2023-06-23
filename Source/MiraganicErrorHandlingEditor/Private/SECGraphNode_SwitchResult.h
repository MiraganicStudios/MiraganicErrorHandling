// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "SGraphNode.h"

class UECK2Node_SwitchResult;

/**
 * Widget for the 'Switch Error Code' K2Node.
 */
class SECGraphNode_SwitchResult : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SECGraphNode_SwitchResult) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UECK2Node_SwitchResult* InNode);

	virtual void CreatePinWidgets() override;

protected:
	virtual void CreateOutputSideAddButton(TSharedPtr<SVerticalBox> OutputBox) override;
	virtual EVisibility IsAddPinButtonVisible() const override;
	virtual FReply OnAddPin() override;

	TSharedPtr<SComboButton> AddCategoryComboButton;
};
