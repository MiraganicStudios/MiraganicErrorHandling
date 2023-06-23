// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.


#include "SECWidget_Result.h"

#include "DetailLayoutBuilder.h"
#include "ECErrorCategory.h"
#include "ECErrorCategoryUtils.h"
#include "SlateOptMacros.h"
#include "Misc/TextFilter.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SSeparator.h"

#define LOCTEXT_NAMESPACE "ErrorHandlingEditor_ResultWidget"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

FText GetResultCategoryAndTitle(const FECResult& Result)
{
	if (Result.IsSuccess())
	{
		return Result.GetTitle();
	}
	else if (!Result.IsValid())
	{
		return LOCTEXT("DisplayName_InvalidResult", "[INVALID]");
	}
	else
	{
		return FText::Format(LOCTEXT("DisplayNameFmt", "{0}: {1}"),
		{Result.GetCategoryName(), Result.GetTitle()});
	}
}

FText FormatResultTooltip(const FECResult& Result)
{
	if (Result.HasValidError())
	{
		return FText::Format(LOCTEXT("TooltipFmt", "{0}: {1}{2}{3}"),
			{Result.GetCategoryName(), Result.GetTitle(), FText::FromString(LINE_TERMINATOR),
			Result.GetMessage()});
	}
	else
	{
		return Result.GetFormattedMessage();
	}
}

enum class EECResultTreeNodeType
{
	Category,
	Result
};

struct FECResultTreeNode
{
	FECResultTreeNode(const FECResult& InCode)
		: Result(InCode)
		, Category(nullptr)
		, Mode(EECResultTreeNodeType::Result)
	{}

	FECResultTreeNode(const UEnum& InCategory)
		: Category(&InCategory)
		, Mode(EECResultTreeNodeType::Category)
	{}

	void AddChild(const TSharedPtr<FECResultTreeNode>& InChild);
	
	void SortChildren();

	void SetFilteredChildren(TArrayView<const TSharedPtr<FECResultTreeNode>> InFilteredChildren);
	
	// Get this node's result, if mode == Result
	TOptional<FECResult> GetResult() const;

	// Get this node's category, if mode == Category
	const UEnum* GetCategory() const;

	EECResultTreeNodeType GetMode() const { return Mode; }

	FString GetSearchTerms() const;
	FString GetSortString() const;

	FText GetDisplayName() const;
	FText GetToolTip() const;

	const TArray<TSharedPtr<FECResultTreeNode>>& GetChildren() const { return AllChildren; }
	const TArray<TSharedPtr<FECResultTreeNode>>& GetFilteredChildren() const { return FilteredChildren; }

private:
	
	// Code itself, if Mode == Result
	FECResult Result;
	
	// Category, if Mode == Category
	const UEnum* Category;

	// Children, if Mode == Category
	TArray<TSharedPtr<FECResultTreeNode>> AllChildren;

	// Filtered Children, if Mode == Category
	TArray<TSharedPtr<FECResultTreeNode>> FilteredChildren;

	EECResultTreeNodeType Mode;
};

void FECResultTreeNode::AddChild(const TSharedPtr<FECResultTreeNode>& InChild)
{
	check(Mode == EECResultTreeNodeType::Category);
	AllChildren.Emplace(InChild);
}

void FECResultTreeNode::SortChildren()
{
	AllChildren.Sort([this](const TSharedPtr<FECResultTreeNode>& A, const TSharedPtr<FECResultTreeNode>& B)
	{
		return A->GetSortString().Compare(B->GetSortString()) < 0;
	});
}

TOptional<FECResult> FECResultTreeNode::GetResult() const
{
	if (Mode != EECResultTreeNodeType::Result)
	{
		return {};
	}

	return Result;
}

const UEnum* FECResultTreeNode::GetCategory() const
{
	if (Mode != EECResultTreeNodeType::Category)
	{
		return {};
	}

	return Category;
}

FString FECResultTreeNode::GetSearchTerms() const
{
	if (Mode == EECResultTreeNodeType::Result)
	{
		return Result.ToString();
	}
	else
	{
		return Category->GetName();
	}
}

FString FECResultTreeNode::GetSortString() const
{
	if (Mode == EECResultTreeNodeType::Result)
	{
		return Result.ToShortString();
	}
	else
	{
		return Category->GetName();
	}
}

FText FECResultTreeNode::GetDisplayName() const
{
	if (Mode == EECResultTreeNodeType::Result)
	{
		return Result.GetTitle();
	}
	else
	{
		return Category->GetDisplayNameText();
	}
}

FText FECResultTreeNode::GetToolTip() const
{
	if (Mode == EECResultTreeNodeType::Result)
	{
		return FormatResultTooltip(Result);
	}
	else
	{
		return Category->GetToolTipText();
	}
}

void FECResultTreeNode::SetFilteredChildren(TArrayView<const TSharedPtr<FECResultTreeNode>> InFilteredChildren)
{
	FilteredChildren = InFilteredChildren;
}


//------------------------------------------------------------------------------------------------------------
// TREE ENTRY

class SECResultTreeEntry : public SComboRow<TSharedPtr<FECResultTreeNode>>
{
public:
	SLATE_BEGIN_ARGS(SECResultTreeEntry)
		: _HighlightText()
		, _SelectedColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
	{}
	/** The text this item should highlight, if any. */
	SLATE_ATTRIBUTE(FText, HighlightText)
	/** The color to use when this item is selected. */
	SLATE_ARGUMENT(FSlateColor, SelectedColor)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, TSharedRef<FECResultTreeNode> InNode)
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
					.ColorAndOpacity(this, &SECResultTreeEntry::GetTextColor)
					.IsEnabled(true)
				]
			];

		STableRow<TSharedPtr<FECResultTreeNode>>::ConstructInternal(
			STableRow::FArguments().ShowSelection(true),
			InOwnerTableView);
	}

	FSlateColor GetTextColor() const
	{
		const TSharedPtr< ITypedTableView< TSharedPtr<FECResultTreeNode> > > OwnerWidget = OwnerTablePtr.Pin();
		const bool bIsSelected = OwnerWidget->Private_IsItemSelected(Node);

		if (bIsSelected)
		{
			return SelectedColor;
		}
		return FSlateColor::UseForeground();
	}

private:
	FSlateColor SelectedColor;
	TSharedPtr<FECResultTreeNode> Node;
};


//------------------------------------------------------------------------------------------------------------
// TREE WIDGET

class SECResultTreeWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SECResultTreeWidget)
		: _bAutoFocus(true)
	{
	}

	SLATE_ARGUMENT(FString, FilterString)
	SLATE_ARGUMENT(FECResult, DefaultSelection)
	SLATE_EVENT(FECResultChangedDelegate, PostResultSelected)
	SLATE_ARGUMENT(bool, bAutoFocus)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	static void GetResultSearchTerms(const FECResultTreeNode& Node, TArray<FString>& OutSearchTerms)
	{
		OutSearchTerms.Emplace(Node.GetSearchTerms());
	}

	TSharedPtr<SSearchBox> GetSearchBox() const { return SearchBox; }

private:
	void UpdateResultOptions();
	void FilterRootNodes();

	void UpdateFilterText(const FText& InFilterText);

	TSharedRef<ITableRow> GenerateTreeRowWidget(TSharedPtr<FECResultTreeNode> Data,
		const TSharedRef<STableViewBase>& OwnerTable
		);

	void GetChildrenForTreeRow(TSharedPtr<FECResultTreeNode> Node, TArray<TSharedPtr<FECResultTreeNode>>& OutChildren);

	void BroadcastResultSelected(TSharedPtr<FECResultTreeNode> Item, ESelectInfo::Type SelectInfo);

	void UpdateExpansionForBranch(TSharedPtr<FECResultTreeNode> Root, bool bExpanded);

	void ExpandFilteredRootNodes();

	bool IsResultNode(TSharedPtr<FECResultTreeNode> Node) const;
	
	using FECResultTextFilter = TTextFilter<const FECResultTreeNode&>;
	
	TSharedPtr<SSearchBox> SearchBox;

	TSharedPtr<STreeView<TSharedPtr<FECResultTreeNode>>> ResultTreeView;

	// All possible result nodes
	TArray<TSharedPtr<FECResultTreeNode>> ResultNodes;
	// All possible root nodes ('Success' and Categories)
	TArray<TSharedPtr<FECResultTreeNode>> RootNodes;
	// All root nodes that are currently visible (some are hidden due to search filter)
	TArray<TSharedPtr<FECResultTreeNode>> FilteredRootNodes;

	TSharedPtr<FECResultTextFilter> SearchFilter;
	
	FECResult SelectedResult;
	
	FString FilterString;
	
	FECResultChangedDelegate PostResultSelected;

	bool bAwaitingFocus = false;
};

void SECResultTreeWidget::Construct(const FArguments& InArgs)
{
	FilterString = InArgs._FilterString;
	SelectedResult = InArgs._DefaultSelection;
	PostResultSelected = InArgs._PostResultSelected;
	bAwaitingFocus = InArgs._bAutoFocus;
	SearchFilter = MakeShareable(new FECResultTextFilter(
		FECResultTextFilter::FItemToStringArray::CreateStatic(&SECResultTreeWidget::GetResultSearchTerms)));

	UpdateResultOptions();
	FilterRootNodes();

	ChildSlot
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(SearchBox, SSearchBox)
				.HintText(LOCTEXT("ResultList_SearchBoxHint", "Search Errors"))
				.OnTextChanged(this, &SECResultTreeWidget::UpdateFilterText)
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
				SAssignNew(ResultTreeView, STreeView<TSharedPtr<FECResultTreeNode>>)
				.Visibility(EVisibility::Visible)
				.SelectionMode(ESelectionMode::Single)
				.TreeItemsSource(&FilteredRootNodes)
				.OnIsSelectableOrNavigable(this, &SECResultTreeWidget::IsResultNode)
				.OnGenerateRow(this, &SECResultTreeWidget::GenerateTreeRowWidget)
				.OnGetChildren(this, &SECResultTreeWidget::GetChildrenForTreeRow)
				.OnSelectionChanged(this, &SECResultTreeWidget::BroadcastResultSelected)
				.OnSetExpansionRecursive(this, &SECResultTreeWidget::UpdateExpansionForBranch)
			]
		];

	TSharedPtr<FECResultTreeNode>* SelectedEntryPtr = ResultNodes.FindByPredicate([this](const TSharedPtr<FECResultTreeNode>& Entry)
	{
		return Entry->GetResult() == SelectedResult;
	});
	if (SelectedEntryPtr)
	{
		ResultTreeView->SetSelection(*SelectedEntryPtr, ESelectInfo::Direct);
	}
	
	ExpandFilteredRootNodes();
}

void SECResultTreeWidget::Tick(const FGeometry& AllottedGeometry,
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

void SECResultTreeWidget::UpdateResultOptions()
{
	ResultNodes.Reset();
	TSharedPtr<FECResultTreeNode> SuccessCode = MakeShareable(new FECResultTreeNode(FECResult::Success()));
	RootNodes.Emplace(SuccessCode);

	TArray<const UEnum*> ErrorCategories;
	ErrorHandling::FindAllErrorCategories(ErrorCategories);

	for (const UEnum* Category : ErrorCategories)
	{
		check(IsValid(Category));

		TSharedPtr<FECResultTreeNode> CategoryNode = MakeShareable(new FECResultTreeNode(*Category));
		RootNodes.Emplace(CategoryNode);

		// Skip the last entry, as it's always reserved for 'MAX'
		const int32 NumValues = Category->NumEnums() - 1;
		for (int32 Idx = 0; Idx < NumValues; ++Idx)
		{
			int64 Value = Category->GetValueByIndex(Idx);
			// Skip the value which is reserved for 'Success'.
			if (Value == FECResult::GetSuccessValue())
			{
				continue;
			}

			if (Category->HasMetaData(TEXT("Hidden"), Idx))
			{
				continue;
			}

			TSharedPtr<FECResultTreeNode> Entry = MakeShareable(new FECResultTreeNode(FECResult(*Category, Value)));
			CategoryNode->AddChild(Entry);
			ResultNodes.Emplace(Entry);
		}

		CategoryNode->SortChildren();
	}

	// Sort all category nodes; always leave 'Success' at the top
	if (RootNodes.Num() > 1)
	{
		TArrayView<TSharedPtr<FECResultTreeNode>> SortableRootNodes(RootNodes.GetData() + 1, RootNodes.Num() - 1);
		SortableRootNodes.Sort([](const TSharedPtr<FECResultTreeNode>& A, const TSharedPtr<FECResultTreeNode>& B)
		{
			return A->GetSortString().Compare(B->GetSortString()) < 0;
		});
	}
}

void SECResultTreeWidget::FilterRootNodes()
{
	FilteredRootNodes.Reset();
	check(RootNodes.Num() >= 1);
	//First node is always the 'Success' node
	const TSharedPtr<FECResultTreeNode>& SuccessNode = RootNodes[0];
	if (!SearchFilter.IsValid() || SearchFilter->PassesFilter(*SuccessNode))
	{
		FilteredRootNodes.Emplace(SuccessNode);
	}

	// The rest of the nodes are categories
	for (int32 Idx = 1; Idx < RootNodes.Num(); ++Idx)
	{
		const TSharedPtr<FECResultTreeNode>& CategoryRootNode = RootNodes[Idx];
		CategoryRootNode->SetFilteredChildren(CategoryRootNode->GetChildren().FilterByPredicate(
			[this](const TSharedPtr<FECResultTreeNode>& Node)
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

void SECResultTreeWidget::UpdateFilterText(const FText& InFilterText)
{
	SearchFilter->SetRawFilterText(InFilterText);
	SearchBox->SetError(SearchFilter->GetFilterErrorText());

	FilterRootNodes();
	ResultTreeView->RequestTreeRefresh();
	ExpandFilteredRootNodes();
}

TSharedRef<ITableRow> SECResultTreeWidget::GenerateTreeRowWidget(TSharedPtr<FECResultTreeNode> Data,
	const TSharedRef<STableViewBase>& OwnerTable
	)
{
	return SNew(SECResultTreeEntry, OwnerTable, Data.ToSharedRef())
		.HighlightText(SearchBox.ToSharedRef(), &SSearchBox::GetSearchText);
}

void SECResultTreeWidget::GetChildrenForTreeRow(TSharedPtr<FECResultTreeNode> Node,
	TArray<TSharedPtr<FECResultTreeNode>>& OutChildren
	)
{
	OutChildren = Node->GetFilteredChildren();
}

void SECResultTreeWidget::BroadcastResultSelected(TSharedPtr<FECResultTreeNode> Item,
	ESelectInfo::Type SelectInfo
	)
{
	if (SelectInfo == ESelectInfo::Direct)
	{
		return;
	}

	// If it was a category, we ignore the selection (categories are only for organization)
	if (Item->GetMode() == EECResultTreeNodeType::Category)
	{
		return;
	}

	SelectedResult = *Item->GetResult();
	PostResultSelected.ExecuteIfBound(*Item->GetResult());
}

void SECResultTreeWidget::UpdateExpansionForBranch(TSharedPtr<FECResultTreeNode> Root, bool bExpanded)
{
	ResultTreeView->SetItemExpansion(Root, bExpanded);

	for (const TSharedPtr<FECResultTreeNode>& Child : Root->GetChildren())
	{
		UpdateExpansionForBranch(Child, bExpanded);
	}
}

void SECResultTreeWidget::ExpandFilteredRootNodes()
{
	for (const TSharedPtr<FECResultTreeNode>& CategoryNode : FilteredRootNodes)
	{
		ResultTreeView->SetItemExpansion(CategoryNode, true);
	}
}

bool SECResultTreeWidget::IsResultNode(TSharedPtr<FECResultTreeNode> Node) const
{
	return Node->GetMode() == EECResultTreeNodeType::Result;
}

void SECWidget_Result::Construct(const FArguments& InArgs)
{
	FilterString = InArgs._FilterString;
	SelectedResult = InArgs._DefaultValue;
	PostResultChanged = InArgs._PostResultChanged;

	ChildSlot
		[
			SAssignNew(ComboButton, SComboButton)
			.OnGetMenuContent(this, &SECWidget_Result::GenerateDropdownWidget)
			.ContentPadding(0.f)
			.ToolTipText(this, &SECWidget_Result::FormatToolTipText)
			.ButtonContent()
			[
				SNew(STextBlock)
				.Text(this, &SECWidget_Result::GetSelectedValueTitle)
				.Font(IDetailLayoutBuilder::GetDetailFont())
			]
		];
}

void SECWidget_Result::SetSelectedResult(const FECResult& Result)
{
	SelectedResult = Result;
}

TSharedRef<SWidget> SECWidget_Result::GenerateDropdownWidget()
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
				SNew(SECResultTreeWidget)
				.PostResultSelected(this, &SECWidget_Result::BroadcastResultChanged)
				.FilterString(FilterString)
				.DefaultSelection(SelectedResult)
				.bAutoFocus(true)
			]
		];
}

FText SECWidget_Result::FormatToolTipText() const
{
	return FormatResultTooltip(SelectedResult);
}

FText SECWidget_Result::GetSelectedValueCategoryAndTitle() const
{
	return GetResultCategoryAndTitle(SelectedResult);
}

FText SECWidget_Result::GetSelectedValueTitle() const
{
	return SelectedResult.GetTitle();
}

void SECWidget_Result::BroadcastResultChanged(FECResult NewResult)
{
	PostResultChanged.ExecuteIfBound(NewResult);
	SelectedResult = NewResult;
	ComboButton->SetIsOpen(false);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
