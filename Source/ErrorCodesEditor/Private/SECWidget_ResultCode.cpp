// Copyright 2022 Miraganic Studios. All rights reserved.


#include "SECWidget_ResultCode.h"

#include "DetailLayoutBuilder.h"
#include "ECErrorCategory.h"
#include "ECErrorCategoryUtils.h"
#include "SlateOptMacros.h"
#include "Misc/TextFilter.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SSeparator.h"

#define LOCTEXT_NAMESPACE "ErrorCodesEditor_ResultCodeWidget"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

FText GetResultCodeCategoryAndTitle(const FECResultCode& ResultCode)
{
	if (ResultCode.IsSuccess())
	{
		return ResultCode.GetTitle();
	}
	else if (!ResultCode.IsValid())
	{
		return LOCTEXT("DisplayName_InvalidResultCode", "[INVALID]");
	}
	else
	{
		return FText::Format(LOCTEXT("DisplayNameFmt", "{0}: {1}"),
		{ResultCode.GetCategoryName(), ResultCode.GetTitle()});
	}
}

FText FormatResultCodeTooltip(const FECResultCode& ResultCode)
{
	if (ResultCode.HasValidError())
	{
		return FText::Format(LOCTEXT("TooltipFmt", "{0}: {1}{2}{3}"),
			{ResultCode.GetCategoryName(), ResultCode.GetTitle(), FText::FromString(LINE_TERMINATOR),
			ResultCode.GetMessage()});
	}
	else
	{
		return ResultCode.GetFormattedMessage();
	}
}

enum class EECResultCodeTreeNodeType
{
	Category,
	ResultCode
};

struct FECResultCodeTreeNode
{
	FECResultCodeTreeNode(const FECResultCode& InCode)
		: ResultCode(InCode)
		, Category(nullptr)
		, Mode(EECResultCodeTreeNodeType::ResultCode)
	{}

	FECResultCodeTreeNode(const UEnum& InCategory)
		: Category(&InCategory)
		, Mode(EECResultCodeTreeNodeType::Category)
	{}

	void AddChild(const TSharedPtr<FECResultCodeTreeNode>& InChild);
	
	void SortChildren();

	void SetFilteredChildren(TArrayView<const TSharedPtr<FECResultCodeTreeNode>> InFilteredChildren);
	
	// Get this node's error code, if mode == ResultCode
	TOptional<FECResultCode> GetResultCode() const;

	// Get this node's category, if mode == Category
	const UEnum* GetCategory() const;

	EECResultCodeTreeNodeType GetMode() const { return Mode; }

	FString GetSearchTerms() const;
	FString GetSortString() const;

	FText GetDisplayName() const;
	FText GetToolTip() const;

	const TArray<TSharedPtr<FECResultCodeTreeNode>>& GetChildren() const { return AllChildren; }
	const TArray<TSharedPtr<FECResultCodeTreeNode>>& GetFilteredChildren() const { return FilteredChildren; }

private:
	
	// Code itself, if Mode == ResultCode
	FECResultCode ResultCode;
	
	// Category, if Mode == Category
	const UEnum* Category;

	// Children, if Mode == Category
	TArray<TSharedPtr<FECResultCodeTreeNode>> AllChildren;

	// Filtered Children, if Mode == Category
	TArray<TSharedPtr<FECResultCodeTreeNode>> FilteredChildren;

	EECResultCodeTreeNodeType Mode;
};

void FECResultCodeTreeNode::AddChild(const TSharedPtr<FECResultCodeTreeNode>& InChild)
{
	check(Mode == EECResultCodeTreeNodeType::Category);
	AllChildren.Emplace(InChild);
}

void FECResultCodeTreeNode::SortChildren()
{
	AllChildren.Sort([this](const TSharedPtr<FECResultCodeTreeNode>& A, const TSharedPtr<FECResultCodeTreeNode>& B)
	{
		return A->GetSortString().Compare(B->GetSortString()) < 0;
	});
}

TOptional<FECResultCode> FECResultCodeTreeNode::GetResultCode() const
{
	if (Mode != EECResultCodeTreeNodeType::ResultCode)
	{
		return {};
	}

	return ResultCode;
}

const UEnum* FECResultCodeTreeNode::GetCategory() const
{
	if (Mode != EECResultCodeTreeNodeType::Category)
	{
		return {};
	}

	return Category;
}

FString FECResultCodeTreeNode::GetSearchTerms() const
{
	if (Mode == EECResultCodeTreeNodeType::ResultCode)
	{
		return ResultCode.ToString();
	}
	else
	{
		return Category->GetName();
	}
}

FString FECResultCodeTreeNode::GetSortString() const
{
	if (Mode == EECResultCodeTreeNodeType::ResultCode)
	{
		return ResultCode.ToShortString();
	}
	else
	{
		return Category->GetName();
	}
}

FText FECResultCodeTreeNode::GetDisplayName() const
{
	if (Mode == EECResultCodeTreeNodeType::ResultCode)
	{
		return ResultCode.GetTitle();
	}
	else
	{
		return Category->GetDisplayNameText();
	}
}

FText FECResultCodeTreeNode::GetToolTip() const
{
	if (Mode == EECResultCodeTreeNodeType::ResultCode)
	{
		return FormatResultCodeTooltip(ResultCode);
	}
	else
	{
		return Category->GetToolTipText();
	}
}

void FECResultCodeTreeNode::SetFilteredChildren(TArrayView<const TSharedPtr<FECResultCodeTreeNode>> InFilteredChildren)
{
	FilteredChildren = InFilteredChildren;
}


//------------------------------------------------------------------------------------------------------------
// TREE ENTRY

class SECResultCodeTreeEntry : public SComboRow<TSharedPtr<FECResultCodeTreeNode>>
{
public:
	SLATE_BEGIN_ARGS(SECResultCodeTreeEntry)
		: _HighlightText()
		, _SelectedColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
	{}
	/** The text this item should highlight, if any. */
	SLATE_ATTRIBUTE(FText, HighlightText)
	/** The color to use when this item is selected. */
	SLATE_ARGUMENT(FSlateColor, SelectedColor)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, TSharedRef<FECResultCodeTreeNode> InNode)
	{
		Node = InNode;
		SelectedColor = InArgs._SelectedColor;

		ChildSlot
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SExpanderArrow, SharedThis(this))
				]
				+SHorizontalBox::Slot()
				.FillWidth(1.f)
				.Padding(0.f, 2.f, 6.f, 2.f)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(Node->GetDisplayName())
					.ToolTipText(Node->GetToolTip())
					.HighlightText(InArgs._HighlightText)
					.ColorAndOpacity(this, &SECResultCodeTreeEntry::GetTextColor)
					.IsEnabled(true)
				]
			];

		STableRow<TSharedPtr<FECResultCodeTreeNode>>::ConstructInternal(
			STableRow::FArguments().ShowSelection(true),
			InOwnerTableView);
	}

	FSlateColor GetTextColor() const
	{
		const TSharedPtr< ITypedTableView< TSharedPtr<FECResultCodeTreeNode> > > OwnerWidget = OwnerTablePtr.Pin();
		const bool bIsSelected = OwnerWidget->Private_IsItemSelected(Node);

		if (bIsSelected)
		{
			return SelectedColor;
		}
		return FSlateColor::UseForeground();
	}

private:
	FSlateColor SelectedColor;
	TSharedPtr<FECResultCodeTreeNode> Node;
};


//------------------------------------------------------------------------------------------------------------
// TREE WIDGET

class SECResultCodeTreeWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SECResultCodeTreeWidget)
		: _bAutoFocus(true)
	{
	}

	SLATE_ARGUMENT(FString, FilterString)
	SLATE_ARGUMENT(FECResultCode, DefaultSelection)
	SLATE_EVENT(FECResultCodeChangedDelegate, PostResultCodeSelected)
	SLATE_ARGUMENT(bool, bAutoFocus)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	static void GetResultCodeSearchTerms(const FECResultCodeTreeNode& Node, TArray<FString>& OutSearchTerms)
	{
		OutSearchTerms.Emplace(Node.GetSearchTerms());
	}

	TSharedPtr<SSearchBox> GetSearchBox() const { return SearchBox; }

private:
	void UpdateResultCodeOptions();
	void FilterRootNodes();

	void UpdateFilterText(const FText& InFilterText);

	TSharedRef<ITableRow> GenerateTreeRowWidget(TSharedPtr<FECResultCodeTreeNode> Data,
		const TSharedRef<STableViewBase>& OwnerTable
		);

	void GetChildrenForTreeRow(TSharedPtr<FECResultCodeTreeNode> Node, TArray<TSharedPtr<FECResultCodeTreeNode>>& OutChildren);

	void BroadcastResultCodeSelected(TSharedPtr<FECResultCodeTreeNode> Item, ESelectInfo::Type SelectInfo);

	void UpdateExpansionForBranch(TSharedPtr<FECResultCodeTreeNode> Root, bool bExpanded);

	void ExpandFilteredRootNodes();

	bool IsResultCodeNode(TSharedPtr<FECResultCodeTreeNode> Node) const;
	
	using FECResultCodeTextFilter = TTextFilter<const FECResultCodeTreeNode&>;
	
	TSharedPtr<SSearchBox> SearchBox;

	TSharedPtr<STreeView<TSharedPtr<FECResultCodeTreeNode>>> ResultCodeTreeView;

	// All possible error code nodes
	TArray<TSharedPtr<FECResultCodeTreeNode>> ResultCodeNodes;
	// All possible root nodes ('Success' and Categories)
	TArray<TSharedPtr<FECResultCodeTreeNode>> RootNodes;
	// All root nodes that are currently visible (some are hidden due to search filter)
	TArray<TSharedPtr<FECResultCodeTreeNode>> FilteredRootNodes;

	TSharedPtr<FECResultCodeTextFilter> SearchFilter;
	
	FECResultCode SelectedResultCode;
	
	FString FilterString;
	
	FECResultCodeChangedDelegate PostResultCodeSelected;

	bool bAwaitingFocus = false;
};

void SECResultCodeTreeWidget::Construct(const FArguments& InArgs)
{
	FilterString = InArgs._FilterString;
	SelectedResultCode = InArgs._DefaultSelection;
	PostResultCodeSelected = InArgs._PostResultCodeSelected;
	bAwaitingFocus = InArgs._bAutoFocus;
	SearchFilter = MakeShareable(new FECResultCodeTextFilter(
		FECResultCodeTextFilter::FItemToStringArray::CreateStatic(&SECResultCodeTreeWidget::GetResultCodeSearchTerms)));

	UpdateResultCodeOptions();
	FilterRootNodes();

	ChildSlot
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(SearchBox, SSearchBox)
				.HintText(LOCTEXT("ResultCodeList_SearchBoxHint", "Search Error Codes"))
				.OnTextChanged(this, &SECResultCodeTreeWidget::UpdateFilterText)
				.DelayChangeNotificationsWhileTyping(true)
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SSeparator)
				.Visibility(EVisibility::Collapsed)
			]
			+SVerticalBox::Slot()
			.FillHeight(1.f)
			[
				SAssignNew(ResultCodeTreeView, STreeView<TSharedPtr<FECResultCodeTreeNode>>)
				.Visibility(EVisibility::Visible)
				.SelectionMode(ESelectionMode::Single)
				.TreeItemsSource(&FilteredRootNodes)
				.OnIsSelectableOrNavigable(this, &SECResultCodeTreeWidget::IsResultCodeNode)
				.OnGenerateRow(this, &SECResultCodeTreeWidget::GenerateTreeRowWidget)
				.OnGetChildren(this, &SECResultCodeTreeWidget::GetChildrenForTreeRow)
				.OnSelectionChanged(this, &SECResultCodeTreeWidget::BroadcastResultCodeSelected)
				.OnSetExpansionRecursive(this, &SECResultCodeTreeWidget::UpdateExpansionForBranch)
			]
		];

	TSharedPtr<FECResultCodeTreeNode>* SelectedEntryPtr = ResultCodeNodes.FindByPredicate([this](const TSharedPtr<FECResultCodeTreeNode>& Entry)
	{
		return Entry->GetResultCode() == SelectedResultCode;
	});
	if (SelectedEntryPtr)
	{
		ResultCodeTreeView->SetSelection(*SelectedEntryPtr, ESelectInfo::Direct);
	}
	
	ExpandFilteredRootNodes();
}

void SECResultCodeTreeWidget::Tick(const FGeometry& AllottedGeometry,
	const double InCurrentTime,
	const float InDeltaTime
	)
{
	if (bAwaitingFocus)
	{
		bAwaitingFocus = false;
		FSlateApplication::Get().SetKeyboardFocus(SearchBox);
	}
}

void SECResultCodeTreeWidget::UpdateResultCodeOptions()
{
	ResultCodeNodes.Reset();
	TSharedPtr<FECResultCodeTreeNode> SuccessCode = MakeShareable(new FECResultCodeTreeNode(FECResultCode::Success()));
	RootNodes.Emplace(SuccessCode);

	TArray<const UEnum*> ErrorCategories;
	ErrorCodes::FindAllErrorCategories(ErrorCategories);

	for (const UEnum* Category : ErrorCategories)
	{
		check(IsValid(Category));

		TSharedPtr<FECResultCodeTreeNode> CategoryNode = MakeShareable(new FECResultCodeTreeNode(*Category));
		RootNodes.Emplace(CategoryNode);

		// Skip the last entry, as it's always reserved for 'MAX'
		const int32 NumValues = Category->NumEnums() - 1;
		for (int32 Idx = 0; Idx < NumValues; ++Idx)
		{
			int64 Value = Category->GetValueByIndex(Idx);
			if (Value == FECResultCode::Success().GetCode())
			{
				continue;
			}

			if (Category->HasMetaData(TEXT("Hidden"), Idx))
			{
				continue;
			}

			TSharedPtr<FECResultCodeTreeNode> Entry = MakeShareable(new FECResultCodeTreeNode(FECResultCode(*Category, Value)));
			CategoryNode->AddChild(Entry);
			ResultCodeNodes.Emplace(Entry);
		}

		CategoryNode->SortChildren();
	}

	// Sort all category nodes; always leave 'Success' at the top
	if (RootNodes.Num() > 1)
	{
		TArrayView<TSharedPtr<FECResultCodeTreeNode>> SortableRootNodes(RootNodes.GetData() + 1, RootNodes.Num() - 1);
		SortableRootNodes.Sort([](const TSharedPtr<FECResultCodeTreeNode>& A, const TSharedPtr<FECResultCodeTreeNode>& B)
		{
			return A->GetSortString().Compare(B->GetSortString()) < 0;
		});
	}
}

void SECResultCodeTreeWidget::FilterRootNodes()
{
	FilteredRootNodes.Reset();
	check(RootNodes.Num() >= 1);
	//First node is always the 'Success' node
	const TSharedPtr<FECResultCodeTreeNode>& SuccessNode = RootNodes[0];
	if (!SearchFilter.IsValid() || SearchFilter->PassesFilter(*SuccessNode))
	{
		FilteredRootNodes.Emplace(SuccessNode);
	}

	// The rest of the nodes are categories
	for (int32 Idx = 1; Idx < RootNodes.Num(); ++Idx)
	{
		const TSharedPtr<FECResultCodeTreeNode>& CategoryRootNode = RootNodes[Idx];
		CategoryRootNode->SetFilteredChildren(CategoryRootNode->GetChildren().FilterByPredicate(
			[this](const TSharedPtr<FECResultCodeTreeNode>& Node)
			{
				if (SearchFilter.IsValid() && !SearchFilter->PassesFilter(*Node))
				{
					return false;
				}

				return true;
			}));

		if (CategoryRootNode->GetFilteredChildren().Num() > 0)
		{
			FilteredRootNodes.Emplace(CategoryRootNode);
		}
	}
}

void SECResultCodeTreeWidget::UpdateFilterText(const FText& InFilterText)
{
	SearchFilter->SetRawFilterText(InFilterText);
	SearchBox->SetError(SearchFilter->GetFilterErrorText());

	FilterRootNodes();
	ResultCodeTreeView->RequestTreeRefresh();
	ExpandFilteredRootNodes();
}

TSharedRef<ITableRow> SECResultCodeTreeWidget::GenerateTreeRowWidget(TSharedPtr<FECResultCodeTreeNode> Data,
	const TSharedRef<STableViewBase>& OwnerTable
	)
{
	return SNew(SECResultCodeTreeEntry, OwnerTable, Data.ToSharedRef())
		.HighlightText(SearchBox.ToSharedRef(), &SSearchBox::GetSearchText);
}

void SECResultCodeTreeWidget::GetChildrenForTreeRow(TSharedPtr<FECResultCodeTreeNode> Node,
	TArray<TSharedPtr<FECResultCodeTreeNode>>& OutChildren
	)
{
	OutChildren = Node->GetFilteredChildren();
}

void SECResultCodeTreeWidget::BroadcastResultCodeSelected(TSharedPtr<FECResultCodeTreeNode> Item,
	ESelectInfo::Type SelectInfo
	)
{
	if (SelectInfo == ESelectInfo::Direct)
	{
		return;
	}

	// If it was a category, we ignore the selection (categories are only for organization)
	if (Item->GetMode() == EECResultCodeTreeNodeType::Category)
	{
		return;
	}

	SelectedResultCode = *Item->GetResultCode();
	PostResultCodeSelected.ExecuteIfBound(*Item->GetResultCode());
}

void SECResultCodeTreeWidget::UpdateExpansionForBranch(TSharedPtr<FECResultCodeTreeNode> Root, bool bExpanded)
{
	ResultCodeTreeView->SetItemExpansion(Root, bExpanded);

	for (const TSharedPtr<FECResultCodeTreeNode>& Child : Root->GetChildren())
	{
		UpdateExpansionForBranch(Child, bExpanded);
	}
}

void SECResultCodeTreeWidget::ExpandFilteredRootNodes()
{
	for (const TSharedPtr<FECResultCodeTreeNode>& CategoryNode : FilteredRootNodes)
	{
		ResultCodeTreeView->SetItemExpansion(CategoryNode, true);
	}
}

bool SECResultCodeTreeWidget::IsResultCodeNode(TSharedPtr<FECResultCodeTreeNode> Node) const
{
	return Node->GetMode() == EECResultCodeTreeNodeType::ResultCode;
}

void SECWidget_ResultCode::Construct(const FArguments& InArgs)
{
	FilterString = InArgs._FilterString;
	SelectedResultCode = InArgs._DefaultValue;
	PostResultCodeChanged = InArgs._PostResultCodeChanged;

	ChildSlot
		[
			SAssignNew(ComboButton, SComboButton)
			.OnGetMenuContent(this, &SECWidget_ResultCode::GenerateDropdownWidget)
			.ContentPadding(0.f)
			.ToolTipText(this, &SECWidget_ResultCode::FormatToolTipText)
			.ButtonContent()
			[
				SNew(STextBlock)
				.Text(this, &SECWidget_ResultCode::GetSelectedValueTitle)
				.Font(IDetailLayoutBuilder::GetDetailFont())
			]
		];
}

void SECWidget_ResultCode::SetSelectedResultCode(const FECResultCode& ResultCode)
{
	SelectedResultCode = ResultCode;
}

TSharedRef<SWidget> SECWidget_ResultCode::GenerateDropdownWidget()
{
	// Tree Widget
	return SNew(SBox)
		.WidthOverride(280.f)
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			.MaxHeight(500)
			[
				SNew(SECResultCodeTreeWidget)
				.PostResultCodeSelected(this, &SECWidget_ResultCode::BroadcastResultCodeChanged)
				.FilterString(FilterString)
				.DefaultSelection(SelectedResultCode)
				.bAutoFocus(true)
			]
		];
}

FText SECWidget_ResultCode::FormatToolTipText() const
{
	return FormatResultCodeTooltip(SelectedResultCode);
}

FText SECWidget_ResultCode::GetSelectedValueCategoryAndTitle() const
{
	return GetResultCodeCategoryAndTitle(SelectedResultCode);
}

FText SECWidget_ResultCode::GetSelectedValueTitle() const
{
	return SelectedResultCode.GetTitle();
}

void SECWidget_ResultCode::BroadcastResultCodeChanged(FECResultCode NewResultCode)
{
	PostResultCodeChanged.ExecuteIfBound(NewResultCode);
	SelectedResultCode = NewResultCode;
	ComboButton->SetIsOpen(false);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
