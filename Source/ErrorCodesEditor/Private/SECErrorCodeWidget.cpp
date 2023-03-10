// Copyright 2022 Miraganic Studios. All rights reserved.


#include "SECErrorCodeWidget.h"

#include "ClassViewerModule.h"
#include "DetailLayoutBuilder.h"
#include "ECErrorCategory.h"
#include "ECErrorCategoryEnum.h"
#include "ECErrorCategoryUtils.h"
#include "SlateOptMacros.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/TextFilter.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SSeparator.h"

#define LOCTEXT_NAMESPACE "ErrorCodesEditor_ErrorCodeWidget"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

FString GetErrorCodeDisplayName(const FECErrorCode& ErrorCode)
{
	return ErrorCode.ToShortString();
}

FText GetErrorCodeDisplayNameText(const FECErrorCode& ErrorCode)
{
	if (ErrorCode.IsError())
	{
		return FText::Format(LOCTEXT("DisplayNameFmt", "{0}: {1}"),
		{ErrorCode.GetCategoryName(), ErrorCode.GetTitle()});
	}
	else if (ErrorCode.IsSuccess())
	{
		return ErrorCode.GetTitle();
	}
	else
	{
		return LOCTEXT("DisplayName_InvalidErrorCode", "[INVALID]");
	}
}

FText FormatErrorCodeTooltip(const FECErrorCode& ErrorCode)
{
	if (ErrorCode.IsError())
	{
		return FText::Format(LOCTEXT("TooltipFmt", "{0}: {1}{2}{3}"),
			{ErrorCode.GetCategoryName(), ErrorCode.GetTitle(), FText::FromString(LINE_TERMINATOR),
			ErrorCode.GetMessage()});
	}
	else
	{
		return ErrorCode.GetFormattedMessage();
	}
}

enum class EECErrorCodeTreeNodeType
{
	Category,
	ErrorCode
};

struct FECErrorCodeTreeNode
{
	FECErrorCodeTreeNode(const FECErrorCode& InCode)
		: ErrorCode(InCode)
		, Category(nullptr)
		, Mode(EECErrorCodeTreeNodeType::ErrorCode)
	{}

	FECErrorCodeTreeNode(const UEnum& InCategory)
		: Category(&InCategory)
		, Mode(EECErrorCodeTreeNodeType::Category)
	{}

	void AddChild(const TSharedPtr<FECErrorCodeTreeNode>& InChild);
	
	// Get this node's error code, if mode == ErrorCode
	TOptional<FECErrorCode> GetErrorCode() const;

	// Get this node's category, if mode == Category
	const UEnum* GetCategory() const;

	EECErrorCodeTreeNodeType GetMode() const { return Mode; }

	FString GetSearchTerms() const;
	FString GetSortString() const;

	FText GetDisplayName() const;
	FText GetToolTip() const;

	void SetFilteredChildren(TArrayView<const TSharedPtr<FECErrorCodeTreeNode>> InFilteredChildren);

	const TArray<TSharedPtr<FECErrorCodeTreeNode>>& GetChildren() const { return AllChildren; }
	const TArray<TSharedPtr<FECErrorCodeTreeNode>>& GetFilteredChildren() const { return FilteredChildren; }

private:
	
	// Code itself, if Mode == ErrorCode
	FECErrorCode ErrorCode;
	
	// Category, if Mode == Category
	const UEnum* Category;

	// Children, if Mode == Category
	TArray<TSharedPtr<FECErrorCodeTreeNode>> AllChildren;

	// Filtered Children, if Mode == Category
	TArray<TSharedPtr<FECErrorCodeTreeNode>> FilteredChildren;

	EECErrorCodeTreeNodeType Mode;
};

void FECErrorCodeTreeNode::AddChild(const TSharedPtr<FECErrorCodeTreeNode>& InChild)
{
	check(Mode == EECErrorCodeTreeNodeType::Category);
	AllChildren.Emplace(InChild);
}

TOptional<FECErrorCode> FECErrorCodeTreeNode::GetErrorCode() const
{
	if (Mode != EECErrorCodeTreeNodeType::ErrorCode)
	{
		return {};
	}

	return ErrorCode;
}

const UEnum* FECErrorCodeTreeNode::GetCategory() const
{
	if (Mode != EECErrorCodeTreeNodeType::Category)
	{
		return {};
	}

	return Category;
}

FString FECErrorCodeTreeNode::GetSearchTerms() const
{
	if (Mode == EECErrorCodeTreeNodeType::ErrorCode)
	{
		return ErrorCode.ToString();
	}
	else
	{
		return Category->GetName();
	}
}

FString FECErrorCodeTreeNode::GetSortString() const
{
	if (Mode == EECErrorCodeTreeNodeType::ErrorCode)
	{
		return ErrorCode.ToShortString();
	}
	else
	{
		return Category->GetName();
	}
}

FText FECErrorCodeTreeNode::GetDisplayName() const
{
	if (Mode == EECErrorCodeTreeNodeType::ErrorCode)
	{
		return ErrorCode.GetTitle();
	}
	else
	{
		return Category->GetDisplayNameText();
	}
}

FText FECErrorCodeTreeNode::GetToolTip() const
{
	if (Mode == EECErrorCodeTreeNodeType::ErrorCode)
	{
		return FormatErrorCodeTooltip(ErrorCode);
	}
	else
	{
		return Category->GetToolTipText();
	}
}

void FECErrorCodeTreeNode::SetFilteredChildren(TArrayView<const TSharedPtr<FECErrorCodeTreeNode>> InFilteredChildren)
{
	FilteredChildren = InFilteredChildren;
}


//------------------------------------------------------------------------------------------------------------
// TREE ENTRY

class SECErrorCodeTreeEntry : public SComboRow<TSharedPtr<FECErrorCodeTreeNode>>
{
public:
	SLATE_BEGIN_ARGS(SECErrorCodeTreeEntry)
		: _HighlightText()
		, _SelectedColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
	{}
	/** The text this item should highlight, if any. */
	SLATE_ATTRIBUTE(FText, HighlightText)
	/** The color to use when this item is selected. */
	SLATE_ARGUMENT(FSlateColor, SelectedColor)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, TSharedRef<FECErrorCodeTreeNode> InNode)
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
					.ColorAndOpacity(this, &SECErrorCodeTreeEntry::GetTextColor)
					.IsEnabled(true)
				]
			];

		STableRow<TSharedPtr<FECErrorCodeTreeNode>>::ConstructInternal(
			STableRow::FArguments().ShowSelection(true),
			InOwnerTableView);
	}

	FSlateColor GetTextColor() const
	{
		const TSharedPtr< ITypedTableView< TSharedPtr<FECErrorCodeTreeNode> > > OwnerWidget = OwnerTablePtr.Pin();
		const bool bIsSelected = OwnerWidget->Private_IsItemSelected(Node);

		if (bIsSelected)
		{
			return SelectedColor;
		}
		return FSlateColor::UseForeground();
	}

private:
	FSlateColor SelectedColor;
	TSharedPtr<FECErrorCodeTreeNode> Node;
};


//------------------------------------------------------------------------------------------------------------
// TREE WIDGET

class SECErrorCodeTreeWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SECErrorCodeTreeWidget)
		: _bAutoFocus(true)
	{
	}

	SLATE_ARGUMENT(FString, FilterString)
	SLATE_ARGUMENT(FECErrorCode, DefaultSelection)
	SLATE_EVENT(FECErrorCodeChangedDelegate, PostErrorCodeSelected)
	SLATE_ARGUMENT(bool, bAutoFocus)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	static void GetErrorCodeSearchTerms(const FECErrorCodeTreeNode& Node, TArray<FString>& OutSearchTerms)
	{
		OutSearchTerms.Emplace(Node.GetSearchTerms());
	}

	TSharedPtr<SSearchBox> GetSearchBox() const { return SearchBox; }

private:
	void UpdateErrorCodeOptions();
	void FilterErrorCodeOptions();

	void UpdateFilterText(const FText& InFilterText);

	TSharedRef<ITableRow> GenerateTreeRowWidget(TSharedPtr<FECErrorCodeTreeNode> Data,
		const TSharedRef<STableViewBase>& OwnerTable
		);

	void GetChildrenForTreeRow(TSharedPtr<FECErrorCodeTreeNode> Node, TArray<TSharedPtr<FECErrorCodeTreeNode>>& OutChildren);

	void BroadcastErrorCodeSelected(TSharedPtr<FECErrorCodeTreeNode> Item, ESelectInfo::Type SelectInfo);

	void UpdateExpansionForBranch(TSharedPtr<FECErrorCodeTreeNode> Root, bool bExpanded);

	void ExpandFilteredRootNodes();

	bool IsErrorCodeNode(TSharedPtr<FECErrorCodeTreeNode> Node) const;
	
	using FECErrorCodeTextFilter = TTextFilter<const FECErrorCodeTreeNode&>;
	
	TSharedPtr<SSearchBox> SearchBox;

	TSharedPtr<STreeView<TSharedPtr<FECErrorCodeTreeNode>>> ErrorCodeTreeView;

	TArray<TSharedPtr<FECErrorCodeTreeNode>> ErrorCodeNodes;
	TArray<TSharedPtr<FECErrorCodeTreeNode>> RootNodes;
	TArray<TSharedPtr<FECErrorCodeTreeNode>> FilteredRootNodes;

	TSharedPtr<FECErrorCodeTextFilter> SearchFilter;
	
	FECErrorCode SelectedErrorCode;
	
	FString FilterString;
	
	FECErrorCodeChangedDelegate PostErrorCodeSelected;

	bool bAwaitingFocus = false;
};

void SECErrorCodeTreeWidget::Construct(const FArguments& InArgs)
{
	FilterString = InArgs._FilterString;
	SelectedErrorCode = InArgs._DefaultSelection;
	PostErrorCodeSelected = InArgs._PostErrorCodeSelected;
	bAwaitingFocus = InArgs._bAutoFocus;
	SearchFilter = MakeShareable(new FECErrorCodeTextFilter(
		FECErrorCodeTextFilter::FItemToStringArray::CreateStatic(&SECErrorCodeTreeWidget::GetErrorCodeSearchTerms)));

	UpdateErrorCodeOptions();
	FilterErrorCodeOptions();

	ChildSlot
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(SearchBox, SSearchBox)
				.HintText(LOCTEXT("ErrorCodeList_SearchBoxHint", "Search Error Codes"))
				.OnTextChanged(this, &SECErrorCodeTreeWidget::UpdateFilterText)
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
				SAssignNew(ErrorCodeTreeView, STreeView<TSharedPtr<FECErrorCodeTreeNode>>)
				.Visibility(EVisibility::Visible)
				.SelectionMode(ESelectionMode::Single)
				.TreeItemsSource(&FilteredRootNodes)
				.OnIsSelectableOrNavigable(this, &SECErrorCodeTreeWidget::IsErrorCodeNode)
				.OnGenerateRow(this, &SECErrorCodeTreeWidget::GenerateTreeRowWidget)
				.OnGetChildren(this, &SECErrorCodeTreeWidget::GetChildrenForTreeRow)
				.OnSelectionChanged(this, &SECErrorCodeTreeWidget::BroadcastErrorCodeSelected)
				.OnSetExpansionRecursive(this, &SECErrorCodeTreeWidget::UpdateExpansionForBranch)
			]
		];

	TSharedPtr<FECErrorCodeTreeNode>* SelectedEntryPtr = ErrorCodeNodes.FindByPredicate([this](const TSharedPtr<FECErrorCodeTreeNode>& Entry)
	{
		return Entry->GetErrorCode() == SelectedErrorCode;
	});
	if (SelectedEntryPtr)
	{
		ErrorCodeTreeView->SetSelection(*SelectedEntryPtr, ESelectInfo::Direct);
	}

	ExpandFilteredRootNodes();
}

void SECErrorCodeTreeWidget::Tick(const FGeometry& AllottedGeometry,
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

void SECErrorCodeTreeWidget::UpdateErrorCodeOptions()
{
	ErrorCodeNodes.Reset();
	TSharedPtr<FECErrorCodeTreeNode> SuccessCode = MakeShareable(new FECErrorCodeTreeNode(FECErrorCode::Success()));
	RootNodes.Emplace(SuccessCode);

	TArray<const UEnum*> ErrorCategories;
	ErrorCodes::FindAllErrorCategories(ErrorCategories);

	for (const UEnum* Category : ErrorCategories)
	{
		check(IsValid(Category));

		TSharedPtr<FECErrorCodeTreeNode> CategoryNode = MakeShareable(new FECErrorCodeTreeNode(*Category));
		RootNodes.Emplace(CategoryNode);

		// Skip the last entry, as it's always reserved for 'MAX'
		const int32 NumValues = Category->NumEnums() - 1;
		for (int32 Idx = 0; Idx < NumValues; ++Idx)
		{
			int64 Value = Category->GetValueByIndex(Idx);
			if (Value == FECErrorCode::Success().GetCode())
			{
				continue;
			}

			if (Category->HasMetaData(TEXT("Hidden"), Idx))
			{
				continue;
			}

			TSharedPtr<FECErrorCodeTreeNode> Entry = MakeShareable(new FECErrorCodeTreeNode(FECErrorCode(*Category, Value)));
			CategoryNode->AddChild(Entry);
			ErrorCodeNodes.Emplace(Entry);
		}
	}

	// Sort all error codes; always leave 'Success' at the top
	if (ErrorCodeNodes.Num() > 1)
	{
		TArrayView<TSharedPtr<FECErrorCodeTreeNode>> ErrorCodes(ErrorCodeNodes.GetData() + 1, ErrorCodeNodes.Num() - 1);
		ErrorCodes.Sort([](const TSharedPtr<FECErrorCodeTreeNode>& A, const TSharedPtr<FECErrorCodeTreeNode>& B)
		{
			const FString StrA = A->GetSortString();
			const FString StrB = B->GetSortString();
			return StrA.Compare(StrB) < 0;
		});
	}
}

void SECErrorCodeTreeWidget::FilterErrorCodeOptions()
{
	FilteredRootNodes.Reset();
	for (const TSharedPtr<FECErrorCodeTreeNode>& RootNode : RootNodes)
	{
		// Just the 'Success' node
		if (RootNode->GetMode() == EECErrorCodeTreeNodeType::ErrorCode)
		{
			if (SearchFilter.IsValid() && !SearchFilter->PassesFilter(*RootNode))
			{
				continue;
			}

			FilteredRootNodes.Emplace(RootNode);
		}
		else // All categories
		{
			RootNode->SetFilteredChildren(RootNode->GetChildren().FilterByPredicate([this](const TSharedPtr<FECErrorCodeTreeNode>& Node)
			{
				if (SearchFilter.IsValid() && !SearchFilter->PassesFilter(*Node))
				{
					return false;
				}

				return true;
			}));

			if (RootNode->GetFilteredChildren().Num() > 0)
			{
				FilteredRootNodes.Emplace(RootNode);
			}
		}
	}
}

void SECErrorCodeTreeWidget::UpdateFilterText(const FText& InFilterText)
{
	SearchFilter->SetRawFilterText(InFilterText);
	SearchBox->SetError(SearchFilter->GetFilterErrorText());

	FilterErrorCodeOptions();
	ErrorCodeTreeView->RequestTreeRefresh();
	ExpandFilteredRootNodes();
}

TSharedRef<ITableRow> SECErrorCodeTreeWidget::GenerateTreeRowWidget(TSharedPtr<FECErrorCodeTreeNode> Data,
	const TSharedRef<STableViewBase>& OwnerTable
	)
{
	return SNew(SECErrorCodeTreeEntry, OwnerTable, Data.ToSharedRef())
		.HighlightText(SearchBox.ToSharedRef(), &SSearchBox::GetSearchText);
}

void SECErrorCodeTreeWidget::GetChildrenForTreeRow(TSharedPtr<FECErrorCodeTreeNode> Node,
	TArray<TSharedPtr<FECErrorCodeTreeNode>>& OutChildren
	)
{
	OutChildren = Node->GetFilteredChildren();
}

void SECErrorCodeTreeWidget::BroadcastErrorCodeSelected(TSharedPtr<FECErrorCodeTreeNode> Item,
	ESelectInfo::Type SelectInfo
	)
{
	if (SelectInfo == ESelectInfo::Direct)
	{
		return;
	}

	// If it was a category, we ignore the selection (categories are only for organization)
	if (Item->GetMode() == EECErrorCodeTreeNodeType::Category)
	{
		return;
	}

	SelectedErrorCode = *Item->GetErrorCode();
	PostErrorCodeSelected.ExecuteIfBound(*Item->GetErrorCode());
}

void SECErrorCodeTreeWidget::UpdateExpansionForBranch(TSharedPtr<FECErrorCodeTreeNode> Root, bool bExpanded)
{
	ErrorCodeTreeView->SetItemExpansion(Root, bExpanded);

	for (const TSharedPtr<FECErrorCodeTreeNode>& Child : Root->GetChildren())
	{
		UpdateExpansionForBranch(Child, bExpanded);
	}
}

void SECErrorCodeTreeWidget::ExpandFilteredRootNodes()
{
	for (const TSharedPtr<FECErrorCodeTreeNode>& CategoryNode : FilteredRootNodes)
	{
		ErrorCodeTreeView->SetItemExpansion(CategoryNode, true);
	}
}

bool SECErrorCodeTreeWidget::IsErrorCodeNode(TSharedPtr<FECErrorCodeTreeNode> Node) const
{
	return Node->GetMode() == EECErrorCodeTreeNodeType::ErrorCode;
}


//------------------------------------------------------------------------------------------------------------
// LIST ENTRY

class SECErrorCodeEntry : public SComboRow<TSharedPtr<FECErrorCode>>
{
public:
	SLATE_BEGIN_ARGS(SECErrorCodeEntry)
		: _HighlightText()
		, _SelectedColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
	{}
	/** The text this item should highlight, if any. */
	SLATE_ARGUMENT(FText, HighlightText)
	/** The color to use when this item is selected. */
	SLATE_ARGUMENT(FSlateColor, SelectedColor)
	/** The node this item is associated with. */
	SLATE_ARGUMENT(TSharedPtr<FECErrorCode>, ErrorCode)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		ErrorCode = InArgs._ErrorCode;
		SelectedColor = InArgs._SelectedColor;

		ChildSlot
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.FillWidth(1.f)
				.Padding(2.f, 3.f, 6.f, 3.f)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(GetErrorCodeDisplayNameText(*ErrorCode))
					.ToolTipText(FormatErrorCodeTooltip(*ErrorCode))
					.HighlightText(InArgs._HighlightText)
					.ColorAndOpacity(this, &SECErrorCodeEntry::GetTextColor)
					.IsEnabled(true)
				]
			];

		STableRow<TSharedPtr<FECErrorCode>>::ConstructInternal(
			STableRow::FArguments().ShowSelection(true),
			InOwnerTableView);
	}

	FSlateColor GetTextColor() const
	{
		const TSharedPtr< ITypedTableView< TSharedPtr<FECErrorCode> > > OwnerWidget = OwnerTablePtr.Pin();
		const bool bIsSelected = OwnerWidget->Private_IsItemSelected(ErrorCode);

		if (bIsSelected)
		{
			return SelectedColor;
		}
		return FSlateColor::UseForeground();
	}

private:
	FSlateColor SelectedColor;
	TSharedPtr<FECErrorCode> ErrorCode;
};


//------------------------------------------------------------------------------------------------------------
// LIST WIDGET

class SECErrorCodeListWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SECErrorCodeListWidget)
		: _bAutoFocus(true)
	{
	}

	SLATE_ARGUMENT(FString, FilterString)
	SLATE_ARGUMENT(FECErrorCode, DefaultSelection)
	SLATE_EVENT(FECErrorCodeChangedDelegate, PostErrorCodeSelected)
	SLATE_ARGUMENT(bool, bAutoFocus)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	static void GetErrorCodeSearchTerms(const FECErrorCode& ErrorCode, TArray<FString>& OutSearchTerms)
	{
		OutSearchTerms.Emplace(ErrorCode.ToString());
	}

	TSharedPtr<SSearchBox> GetSearchBox() const { return SearchBox; }

private:
	void UpdateErrorCodeOptions();
	void FilterErrorCodeOptions();

	void UpdateFilterText(const FText& InFilterText);

	TSharedRef<ITableRow> GenerateListRowWidget(TSharedPtr<FECErrorCode> Data,
		const TSharedRef<STableViewBase>& OwnerTable
		);

	void BroadcastErrorCodeSelected(TSharedPtr<FECErrorCode> Item, ESelectInfo::Type SelectInfo);
	
	using FECErrorCodeTextFilter = TTextFilter<const FECErrorCode&>;
	
	TSharedPtr<SSearchBox> SearchBox;

	TSharedPtr<SListView<TSharedPtr<FECErrorCode>>> ErrorCodeListView;

	TArray<TSharedPtr<FECErrorCode>> AllOptions;
	TArray<TSharedPtr<FECErrorCode>> FilteredOptions;

	TSharedPtr<FECErrorCodeTextFilter> SearchFilter;

	FECErrorCode SelectedErrorCode;
	
	FString FilterString;
	
	FECErrorCodeChangedDelegate PostErrorCodeSelected;

	bool bAwaitingFocus = false;
};

void SECErrorCodeListWidget::Construct(const FArguments& InArgs)
{
	FilterString = InArgs._FilterString;
	SelectedErrorCode = InArgs._DefaultSelection;
	PostErrorCodeSelected = InArgs._PostErrorCodeSelected;
	bAwaitingFocus = InArgs._bAutoFocus;
	SearchFilter = MakeShareable(new FECErrorCodeTextFilter(
		FECErrorCodeTextFilter::FItemToStringArray::CreateStatic(&SECErrorCodeListWidget::GetErrorCodeSearchTerms)));

	UpdateErrorCodeOptions();
	FilterErrorCodeOptions();

	ChildSlot
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(SearchBox, SSearchBox)
				.HintText(LOCTEXT("ErrorCodeList_SearchBoxHint", "Search Error Codes"))
				.OnTextChanged(this, &SECErrorCodeListWidget::UpdateFilterText)
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
				SAssignNew(ErrorCodeListView, SListView<TSharedPtr<FECErrorCode>>)
				.Visibility(EVisibility::Visible)
				.SelectionMode(ESelectionMode::Single)
				.ListItemsSource(&FilteredOptions)
				.OnGenerateRow(this, &SECErrorCodeListWidget::GenerateListRowWidget)
				.OnSelectionChanged(this, &SECErrorCodeListWidget::BroadcastErrorCodeSelected)
			]
		];

	TSharedPtr<FECErrorCode>* SelectedEntryPtr = AllOptions.FindByPredicate([this](const TSharedPtr<FECErrorCode>& Entry)
	{
		return *Entry == SelectedErrorCode;
	});
	if (SelectedEntryPtr)
	{
		ErrorCodeListView->SetSelection(*SelectedEntryPtr, ESelectInfo::Direct);
	}
}

void SECErrorCodeListWidget::Tick(const FGeometry& AllottedGeometry,
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

void SECErrorCodeListWidget::UpdateErrorCodeOptions()
{
	AllOptions.Reset();
	TSharedPtr<FECErrorCode> SuccessCode = MakeShareable(new FECErrorCode(FECErrorCode::Success()));

	AllOptions.Emplace(SuccessCode);

	TArray<const UEnum*> ErrorCategories;
	ErrorCodes::FindAllErrorCategories(ErrorCategories);

	for (const UEnum* Category : ErrorCategories)
	{
		check(IsValid(Category));

		// Skip the last entry, as it's always reserved for 'MAX'
		const int32 NumValues = Category->NumEnums() - 1;
		for (int32 Idx = 0; Idx < NumValues; ++Idx)
		{
			int64 Value = Category->GetValueByIndex(Idx);
			if (Value == FECErrorCode::Success().GetCode())
			{
				continue;
			}

			if (Category->HasMetaData(TEXT("Hidden"), Idx))
			{
				continue;
			}

			TSharedPtr<FECErrorCode> Entry = MakeShareable(new FECErrorCode(*Category, Value));
			AllOptions.Emplace(Entry);
		}
	}

	// Sort all error codes; always leave 'Success' at the top
	if (AllOptions.Num() > 1)
	{
		TArrayView<TSharedPtr<FECErrorCode>> ErrorCodes(AllOptions.GetData() + 1, AllOptions.Num() - 1);
		ErrorCodes.Sort([](const TSharedPtr<FECErrorCode>& A, const TSharedPtr<FECErrorCode>& B)
		{
			const FString StrA = A->ToShortString();
			const FString StrB = B->ToShortString();
			return StrA.Compare(StrB) < 0;
		});
	}
}

void SECErrorCodeListWidget::FilterErrorCodeOptions()
{
	FilteredOptions = AllOptions.FilterByPredicate([this](const TSharedPtr<FECErrorCode>& Entry)
	{
		if (SearchFilter.IsValid() && !SearchFilter->PassesFilter(*Entry))
		{
			return false;
		}

		return true;
	});
}

void SECErrorCodeListWidget::UpdateFilterText(const FText& InFilterText)
{
	SearchFilter->SetRawFilterText(InFilterText);
	SearchBox->SetError(SearchFilter->GetFilterErrorText());

	FilterErrorCodeOptions();
}

TSharedRef<ITableRow> SECErrorCodeListWidget::GenerateListRowWidget(TSharedPtr<FECErrorCode> Data,
	const TSharedRef<STableViewBase>& OwnerTable
	)
{
	return SNew(SECErrorCodeEntry, OwnerTable)
		.HighlightText(SearchBox->GetText())
		.SelectedColor(FLinearColor(1.f, 1.f, 1.f, 1.f))
		.ErrorCode(Data);
}

void SECErrorCodeListWidget::BroadcastErrorCodeSelected(TSharedPtr<FECErrorCode> Item,
	ESelectInfo::Type SelectInfo
	)
{
	// Set this widget
	if (SelectInfo == ESelectInfo::Direct)
	{
		return;
	}
	
	SelectedErrorCode = *Item;
	PostErrorCodeSelected.ExecuteIfBound(*Item);
}

void SECErrorCodeWidget::Construct(const FArguments& InArgs)
{
	FilterString = InArgs._FilterString;
	SelectedErrorCode = InArgs._DefaultValue;
	PostErrorCodeChanged = InArgs._PostErrorCodeChanged;

	ChildSlot
		[
			SAssignNew(ComboButton, SComboButton)
			.OnGetMenuContent(this, &SECErrorCodeWidget::GenerateDropdownWidget)
			.ContentPadding(0.f)
			.ToolTipText(this, &SECErrorCodeWidget::FormatToolTipText)
			.ButtonContent()
			[
				SNew(STextBlock)
				.Text(this, &SECErrorCodeWidget::GetSelectedValueTitle)
				.Font(IDetailLayoutBuilder::GetDetailFont())
			]
		];
}

void SECErrorCodeWidget::SetSelectedErrorCode(const FECErrorCode& ErrorCode)
{
	SelectedErrorCode = ErrorCode;
}

TSharedRef<SWidget> SECErrorCodeWidget::GenerateDropdownWidget()
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
				SNew(SECErrorCodeTreeWidget)
				.PostErrorCodeSelected(this, &SECErrorCodeWidget::BroadcastErrorCodeChanged)
				.FilterString(FilterString)
				.DefaultSelection(SelectedErrorCode)
				.bAutoFocus(true)
			]
		];

	// List Widget
	//return SNew(SBox)
	// .WidthOverride(280.f)
	// [
	// 	SNew(SVerticalBox)
	// 	+SVerticalBox::Slot()
	// 	.AutoHeight()
	// 	.MaxHeight(500)
	// 	[
	// 		SNew(SECErrorCodeListWidget)
	// 		.PostErrorCodeSelected(this, &SECErrorCodeWidget::BroadcastErrorCodeChanged)
	// 		.FilterString(FilterString)
	// 		.DefaultSelection(SelectedErrorCode)
	// 		.bAutoFocus(true)
	// 	]
	// ];
}

FText SECErrorCodeWidget::FormatToolTipText() const
{
	return FormatErrorCodeTooltip(SelectedErrorCode);
}

FText SECErrorCodeWidget::GetSelectedValueCategoryAndTitle() const
{
	return GetErrorCodeDisplayNameText(SelectedErrorCode);
}

FText SECErrorCodeWidget::GetSelectedValueTitle() const
{
	return SelectedErrorCode.GetTitle();
}

void SECErrorCodeWidget::BroadcastErrorCodeChanged(FECErrorCode NewErrorCode)
{
	PostErrorCodeChanged.ExecuteIfBound(NewErrorCode);
	SelectedErrorCode = NewErrorCode;
	ComboButton->SetIsOpen(false);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
