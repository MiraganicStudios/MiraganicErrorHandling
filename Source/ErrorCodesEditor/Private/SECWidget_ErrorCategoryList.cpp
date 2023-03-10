// Copyright 2022 Miraganic Studios. All rights reserved.


#include "SECWidget_ErrorCategoryList.h"

#include "ECErrorCategoryUtils.h"
#include "SlateOptMacros.h"
#include "Slate/SObjectTableRow.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SSeparator.h"

#define LOCTEXT_NAMESPACE "ErrorCodesEditor_ErrorCategoryListWidget"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

class SECErrorCategoryEntry : public SComboRow<const UEnum*>
{
public:
	SLATE_BEGIN_ARGS(SECErrorCategoryEntry)
		: _HighlightText()
		, _ErrorCategory(nullptr)
	{}
	/** The text this item should highlight, if any. */
	SLATE_ARGUMENT(FText, HighlightText)
	/** The node this item is associated with. */
	SLATE_ARGUMENT(const UEnum*, ErrorCategory)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		ErrorCategory = InArgs._ErrorCategory;

		ChildSlot
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.FillWidth(1.f)
				.Padding(0.f, 3.f, 6.f, 3.f)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(ErrorCategory->GetDisplayNameText())
					.ToolTipText(ErrorCategory->GetToolTipText())
					.HighlightText(InArgs._HighlightText)
					.ColorAndOpacity(FSlateColor::UseForeground())
					.IsEnabled(true)
				]
			];

		STableRow<const UEnum*>::ConstructInternal(
			STableRow::FArguments().ShowSelection(true),
			InOwnerTableView);
	}

private:
	const UEnum* ErrorCategory = nullptr;
};

void SECWidget_ErrorCategoryList::Construct(const FArguments& InArgs)
{
	PostErrorCategoryPicked = InArgs._PostErrorCategoryPicked;
	bAwaitingFocus = InArgs._bAutoFocus;
	SearchFilter = MakeShareable(new FECErrorCategoryTextFilter(
		FECErrorCategoryTextFilter::FItemToStringArray::CreateStatic(
			&SECWidget_ErrorCategoryList::GetErrorCategorySearchTerms)));

	UpdateErrorCategoryOptions();
	FilterErrorCategoryOptions();

	ChildSlot
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(SearchBox, SSearchBox)
				.HintText(LOCTEXT("SearchBoxHint", "Search Error Codes"))
				.OnTextChanged(this, &SECWidget_ErrorCategoryList::UpdateFilterText)
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
				SAssignNew(ErrorCategoryListView, SListView<const UEnum*>)
				.Visibility(EVisibility::Visible)
				.SelectionMode(ESelectionMode::Single)
				.ListItemsSource(&FilteredCategoryOptions)
				.OnGenerateRow(this, &SECWidget_ErrorCategoryList::GenerateListRowWidget)
				.OnSelectionChanged(this, &SECWidget_ErrorCategoryList::BroadcastErrorCategoryPicked)
			]
		];
}

void SECWidget_ErrorCategoryList::Tick(const FGeometry& AllottedGeometry,
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

void SECWidget_ErrorCategoryList::GetErrorCategorySearchTerms(const UEnum* ErrorCategory,
	TArray<FString>& OutSearchTerms
)
{
	if (!IsValid(ErrorCategory))
	{
		return;
	}
	
	OutSearchTerms.Emplace(ErrorCategory->GetName());
}

void SECWidget_ErrorCategoryList::UpdateErrorCategoryOptions()
{
	ErrorCodes::FindAllErrorCategories(AllCategoryOptions);
	AllCategoryOptions.Sort([](const UEnum& A, const UEnum& B)
	{
		return A.GetFName().LexicalLess(B.GetFName());
	});
}

void SECWidget_ErrorCategoryList::FilterErrorCategoryOptions()
{
	FilteredCategoryOptions = AllCategoryOptions.FilterByPredicate([this](const UEnum* Category)
	{
		if (SearchFilter.IsValid() && !SearchFilter->PassesFilter(Category))
		{
			return false;
		}

		return true;
	});
}

void SECWidget_ErrorCategoryList::UpdateFilterText(const FText& InFilterText)
{
	SearchFilter->SetRawFilterText(InFilterText);
	SearchBox->SetError(SearchFilter->GetFilterErrorText());

	FilterErrorCategoryOptions();
}

TSharedRef<ITableRow> SECWidget_ErrorCategoryList::GenerateListRowWidget(const UEnum* ErrorCategory,
	const TSharedRef<STableViewBase>& OwnerTable
)
{
	return SNew(STableRow<const UEnum*>, OwnerTable)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.FillWidth(1.f)
			.Padding(0.f, 3.f, 6.f, 3.f)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(ErrorCategory->GetDisplayNameText())
				.ToolTipText(ErrorCategory->GetToolTipText())
				.HighlightText(SearchBox->GetText())
				.ColorAndOpacity(FSlateColor::UseForeground())
				.IsEnabled(true)
			]
		];
	
	// return SNew(SECErrorCategoryEntry, OwnerTable)
	// 	.HighlightText(SearchBox->GetText())
	// 	.TextColor(FLinearColor(1.f, 1.f, 1.f, 1.f))
	// 	.ErrorCategory(ErrorCategory);
}

void SECWidget_ErrorCategoryList::BroadcastErrorCategoryPicked(const UEnum* ErrorCategory,
	ESelectInfo::Type SelectInfo
)
{
	PostErrorCategoryPicked.ExecuteIfBound(ErrorCategory);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
