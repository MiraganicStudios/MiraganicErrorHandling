// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.


#include "SECGraphNode_SwitchResult.h"

#include "K2Node_SwitchEnum.h"
#include "ECK2Node_SwitchResult.h"
#include "GraphEditorSettings.h"
#include "NodeFactory.h"
#include "SECWidget_ErrorCategoryList.h"
#include "SlateOptMacros.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetPins/SGraphPinExec.h"

#define LOCTEXT_NAMESPACE "ErrorHandlingEditor_SwitchResultGraphNode"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

class SECGraphPin_SwitchDefaultCaseExec : public SGraphPinExec
{
public:
	SLATE_BEGIN_ARGS(SECGraphPin_SwitchDefaultCaseExec) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin)
	{
		SGraphPin::Construct(SGraphPin::FArguments().PinLabelStyle(TEXT("Graph.Node.DefaultPinName")), InPin);

		CachePinIcons();
	}
};

void SECGraphNode_SwitchResult::Construct(const FArguments& InArgs, UECK2Node_SwitchResult* InNode)
{
	GraphNode = InNode;
	SetCursor(EMouseCursor::CardinalCross);
	UpdateGraphNode();
}

void SECGraphNode_SwitchResult::CreatePinWidgets()
{
	UK2Node_Switch* SwitchNode = CastChecked<UK2Node_Switch>(GraphNode);
	UEdGraphPin* DefaultPin = SwitchNode->GetDefaultPin();

	// Create Pin widgets for each of the pins, except for the default pin
	for (auto PinIt = GraphNode->Pins.CreateConstIterator(); PinIt; ++PinIt)
	{
		UEdGraphPin* CurrentPin = *PinIt;
		if ((!CurrentPin->bHidden) && (CurrentPin != DefaultPin))
		{
			TSharedPtr<SGraphPin> NewPin = FNodeFactory::CreatePinWidget(CurrentPin);
			check(NewPin.IsValid());

			this->AddPin(NewPin.ToSharedRef());
		}
	}

	// Handle the default pin
	if (DefaultPin != NULL)
	{
		// Create some padding
		RightNodeBox->AddSlot()
			.AutoHeight()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			.Padding(1.0f)
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Graph.Pin.DefaultPinSeparator"))
			];

		// Create the pin itself
		TSharedPtr<SGraphPin> NewPin = SNew(SECGraphPin_SwitchDefaultCaseExec, DefaultPin);

		this->AddPin(NewPin.ToSharedRef());
	}
}

void SECGraphNode_SwitchResult::CreateOutputSideAddButton(TSharedPtr<SVerticalBox> OutputBox)
{
	// Don't include an add button; it clutters the node
}

EVisibility SECGraphNode_SwitchResult::IsAddPinButtonVisible() const
{
	if (!GraphNode->IsA<UK2Node_SwitchEnum>())
	{
		return SGraphNode::IsAddPinButtonVisible();
	}

	return EVisibility::Collapsed;
}

FReply SECGraphNode_SwitchResult::OnAddPin()
{
	UK2Node_Switch* SwitchNode = CastChecked<UK2Node_Switch>(GraphNode);

	const FScopedTransaction Transaction( NSLOCTEXT("Kismet", "AddExecutionPin", "Add Execution Pin") );
	SwitchNode->Modify();

	SwitchNode->AddPinToSwitchNode();
	FBlueprintEditorUtils::MarkBlueprintAsModified(SwitchNode->GetBlueprint());

	UpdateGraphNode();
	GraphNode->GetGraph()->NotifyGraphChanged();

	return FReply::Handled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
