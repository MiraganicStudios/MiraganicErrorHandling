// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "SGraphNode.h"

class UECK2Node_SwitchErrorCode;

/**
 * Widget for the 'Switch Error Code' K2Node.
 */
class SECGraphNode_SwitchErrorCode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SECGraphNode_SwitchErrorCode) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UECK2Node_SwitchErrorCode* InNode);

	virtual void CreatePinWidgets() override;

protected:
	virtual void CreateOutputSideAddButton(TSharedPtr<SVerticalBox> OutputBox) override;
	virtual EVisibility IsAddPinButtonVisible() const override;
	virtual FReply OnAddPin() override;

	TSharedPtr<SComboButton> AddCategoryComboButton;
};
