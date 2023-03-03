// Copyright 2022 Miraganic Studios. All rights reserved.


#include "SECErrorCodeWidget.h"

#include "MGErrorCategory.h"
#include "SlateOptMacros.h"
#include "Misc/TextFilter.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SSeparator.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

FString GetErrorCodeDisplayName(const FMGErrorCode& ErrorCode)
{
	if (ErrorCode.IsSuccess())
	{
		return TEXT("Success");
	}
	else
	{
		return FString::Printf(TEXT("%s:%s"), *ErrorCode.Category->GetTrimmedName(), *ErrorCode.GetErrorTitle().ToString());
	}
}

struct FECErrorCodeListData
{
public:
	FECErrorCodeListData(FMGErrorCode InErrorCode, FString&& InDisplayName)
		: ErrorCode(InErrorCode)
		, DisplayName(MakeShareable(new FString(MoveTemp(InDisplayName))))
	{}

	FMGErrorCode ErrorCode;
	TSharedPtr<FString> DisplayName;
};

class SECErrorCodeEntry : public SComboRow<TSharedPtr<FECErrorCodeListData>>
{
public:
	SLATE_BEGIN_ARGS(SECErrorCodeEntry)
		: _HighlightText()
		, _TextColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
	{}
	/** The text this item should highlight, if any. */
	SLATE_ARGUMENT(FText, HighlightText)
	/** The color text this item will use. */
	SLATE_ARGUMENT(FSlateColor, TextColor)
	/** The node this item is associated with. */
	SLATE_ARGUMENT(TSharedPtr<FECErrorCodeListData>, ListData)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		ListData = InArgs._ListData;
		TextColor = InArgs._TextColor;

		ChildSlot
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.FillWidth(1.f)
				.Padding(0.f, 3.f, 6.f, 3.f)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(*ListData->DisplayName))
					.HighlightText(InArgs._HighlightText)
					.ColorAndOpacity(this, &SECErrorCodeEntry::GetTextColor)
					.IsEnabled(true)
				]
			];

		STableRow<TSharedPtr<FECErrorCodeListData>>::ConstructInternal(
			STableRow::FArguments().ShowSelection(true),
			InOwnerTableView);
	}

	FSlateColor GetTextColor() const
	{
		const TSharedPtr< ITypedTableView< TSharedPtr<FECErrorCodeListData> > > OwnerWidget = OwnerTablePtr.Pin();
		const bool bIsSelected = OwnerWidget->Private_IsItemSelected(ListData);

		if (bIsSelected)
		{
			return FSlateColor::UseForeground();
		}
		return TextColor;
	}

private:
	FSlateColor TextColor;
	TSharedPtr<FECErrorCodeListData> ListData;
};

class SECErrorCodeListWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SECErrorCodeListWidget)
	{
	}

	SLATE_ARGUMENT(FString, FilterString)
	SLATE_EVENT(FECErrorCodeChangedDelegate, PostErrorCodeSelected)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual ~SECErrorCodeListWidget()
	{
		if (PostErrorCodeSelected.IsBound())
		{
			PostErrorCodeSelected.Unbind();
		}
	}

	static void GetErrorCodeSearchTerms(const FMGErrorCode& ErrorCode, TArray<FString>& OutSearchTerms)
	{
		if (ErrorCode.IsSuccess())
		{
			OutSearchTerms.Emplace(TEXT("Success"));
		}
		else
		{
			OutSearchTerms.Add(FString::Printf(TEXT("%s:%s\n%s"), *ErrorCode.Category->GetTrimmedName(),
				*ErrorCode.GetErrorTitle().ToString(), *ErrorCode.GetErrorMessage().ToString()));
		}
	}

private:
	void UpdateErrorCodeOptions();

	void UpdateFilterText(const FText& InFilterText);

	TSharedRef<ITableRow> GenerateListRowWidget(TSharedPtr<FECErrorCodeListData> Data,
		const TSharedRef<STableViewBase>& OwnerTable
		);

	void BroadcastErrorCodeSelected(TSharedPtr<FECErrorCodeListData> Item, ESelectInfo::Type SelectInfo);
	
	using FECErrorCodeTextFilter = TTextFilter<const FMGErrorCode&>;
	
	TSharedPtr<SSearchBox> SearchBox;

	TSharedPtr<SListView<TSharedPtr<FECErrorCodeListData>>> ErrorCodeListView;

	TArray<TSharedPtr<FECErrorCodeListData>> ListDatum;

	TSharedPtr<FECErrorCodeTextFilter> SearchFilter;
	
	FString FilterString;
	
	FECErrorCodeChangedDelegate PostErrorCodeSelected;
};

void SECErrorCodeListWidget::Construct(const FArguments& InArgs)
{
	FilterString = InArgs._FilterString;
	PostErrorCodeSelected = InArgs._PostErrorCodeSelected;
	SearchFilter = MakeShareable(new FECErrorCodeTextFilter(
		FECErrorCodeTextFilter::FItemToStringArray::CreateStatic(&SECErrorCodeListWidget::GetErrorCodeSearchTerms)));

	UpdateErrorCodeOptions();

	ChildSlot
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(SearchBox, SSearchBox)
				.HintText(NSLOCTEXT("ErrorCodes", "ErrorCodeList_SearchBoxHint", "Search Error Codes"))
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
				SAssignNew(ErrorCodeListView, SListView<TSharedPtr<FECErrorCodeListData>>)
				.Visibility(EVisibility::Visible)
				.SelectionMode(ESelectionMode::Single)
				.ListItemsSource(&ListDatum)
				.OnGenerateRow(this, &SECErrorCodeListWidget::GenerateListRowWidget)
				.OnSelectionChanged(this, &SECErrorCodeListWidget::BroadcastErrorCodeSelected)
			]
		];
}

void SECErrorCodeListWidget::UpdateErrorCodeOptions()
{
	ListDatum.Reset();
	TSharedPtr<FECErrorCodeListData> SuccessCode = MakeShareable(new FECErrorCodeListData(FMGErrorCode::Success(), TEXT("Success")));

	ListDatum.Emplace(SuccessCode);

	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		UClass* Class = *ClassIt;
		if (!IsValid(Class) || !Class->IsChildOf(UMGErrorCategory::StaticClass()))
		{
			continue;
		}

		// Ignore dead classes or duplicates of blueprint classes
		if (Class->GetClass()->IsChildOf<UBlueprintGeneratedClass>() && (!IsValid(Class->ClassGeneratedBy) ||
			Class->GetAuthoritativeClass() != Class))
		{
			continue;
		}

		const UMGErrorCategory* CategoryCDO = Class->GetDefaultObject<UMGErrorCategory>();
		if (!CategoryCDO)
		{
			continue;
		}

		for (const auto& ErrorCodePair : CategoryCDO->Errors)
		{
			FMGErrorCode ErrorCode(*CategoryCDO, ErrorCodePair.Key);
			if (SearchFilter.IsValid() && !SearchFilter->PassesFilter(ErrorCode))
			{
				continue;
			}

			TSharedPtr<FECErrorCodeListData> Entry = MakeShareable(new FECErrorCodeListData(ErrorCode,
				GetErrorCodeDisplayName(ErrorCode)));
			ListDatum.Emplace(Entry);
		}
	}
}

void SECErrorCodeListWidget::UpdateFilterText(const FText& InFilterText)
{
	SearchFilter->SetRawFilterText(InFilterText);
	SearchBox->SetError(SearchFilter->GetFilterErrorText());

	UpdateErrorCodeOptions();
}

TSharedRef<ITableRow> SECErrorCodeListWidget::GenerateListRowWidget(TSharedPtr<FECErrorCodeListData> Data,
	const TSharedRef<STableViewBase>& OwnerTable
	)
{
	return SNew(SECErrorCodeEntry, OwnerTable)
		.HighlightText(SearchBox->GetText())
		.TextColor(FLinearColor(1.f, 1.f, 1.f, 1.f))
		.ListData(Data);
}

void SECErrorCodeListWidget::BroadcastErrorCodeSelected(TSharedPtr<FECErrorCodeListData> Item,
	ESelectInfo::Type SelectInfo
	)
{
	PostErrorCodeSelected.ExecuteIfBound(Item->ErrorCode);
}

void SECErrorCodeWidget::Construct(const FArguments& InArgs)
{
	FilterString = InArgs._FilterString;
	SelectedErrorCode = InArgs._DefaultValue;
	PostErrorCodeChanged = InArgs._PostErrorCodeChanged;

	SAssignNew(ComboButton, SComboButton)
		.OnGetMenuContent(this, &SECErrorCodeWidget::GenerateDropdownWidget)
		.ContentPadding(FMargin(2.f, 2.f))
		.ToolTipText(this, &SECErrorCodeWidget::FormatToolTipText)
		.ButtonContent()
		[
			SNew(STextBlock)
			.Text(this, &SECErrorCodeWidget::GetSelectedValueAsText)
		];

	ChildSlot
		[
			ComboButton.ToSharedRef()
		];
}

TSharedRef<SWidget> SECErrorCodeWidget::GenerateDropdownWidget()
{
	return SNew(SBox)
		.WidthOverride(280.f)
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			.MaxHeight(500)
			[
				SNew(SECErrorCodeListWidget)
				.PostErrorCodeSelected(this, &SECErrorCodeWidget::BroadcastErrorCodeChanged)
				.FilterString(FilterString)
			]
		];
}

FText SECErrorCodeWidget::FormatToolTipText() const
{
	if (SelectedErrorCode.IsSuccess())
	{
		return NSLOCTEXT("ErrorCodes", "ErrorCodeWidget_SuccessText", "Success");
	}

	// Category, Title, and Message
	return FText::Format(NSLOCTEXT("ErrorCodes", "ErrorCodeWidget_TooltipFormat", "{0}:{1}\n{2}"),
		{FText::FromString(SelectedErrorCode.Category->GetTrimmedName()), SelectedErrorCode.GetErrorTitle(), SelectedErrorCode.GetErrorMessage()});
}

FText SECErrorCodeWidget::GetSelectedValueAsText() const
{
	if (SelectedErrorCode.IsSuccess())
	{
		return NSLOCTEXT("ErrorCodes", "ErrorCodeWidget_SuccessText", "Success");
	}

	// Category and Title
	return FText::Format(NSLOCTEXT("ErrorCodes", "ErrorCodeWidget_ValueFormat", "{0}:{1}"),
		{FText::FromString(SelectedErrorCode.Category->GetTrimmedName()), SelectedErrorCode.GetErrorTitle()});
}

void SECErrorCodeWidget::BroadcastErrorCodeChanged(FMGErrorCode NewErrorCode)
{
	PostErrorCodeChanged.ExecuteIfBound(NewErrorCode);
	SelectedErrorCode = NewErrorCode;
	ComboButton->SetIsOpen(false);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
