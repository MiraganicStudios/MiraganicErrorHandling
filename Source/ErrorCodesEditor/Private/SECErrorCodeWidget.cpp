// Copyright 2022 Miraganic Studios. All rights reserved.


#include "SECErrorCodeWidget.h"

#include "MGErrorCategory.h"
#include "SlateOptMacros.h"
#include "Misc/TextFilter.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SSeparator.h"

#define LOCTEXT_NAMESPACE "ErrorCodes"

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

FText GetErrorCodeDisplayNameText(const FMGErrorCode& ErrorCode)
{
	if (ErrorCode.IsSuccess())
	{
		return LOCTEXT("ErrorCodeWidget_Success", "Success");
	}
	else
	{
		return FText::Format(LOCTEXT("ErrorCodeWidget_TooltipFormat", "{0}:{1}"),
		{FText::FromString(ErrorCode.Category->GetTrimmedName()), ErrorCode.GetErrorTitle()});
	}
}

FText GetErrorCodeTooltip(const FMGErrorCode& ErrorCode)
{
	if (ErrorCode.IsSuccess())
	{
		return LOCTEXT("ErrorCodeWidget_Success", "Success");
	}
	else
	{
		return FText::Format(LOCTEXT("ErrorCodeWidget_TooltipFormat", "{0}:{1}\n{2}"),
		{FText::FromString(ErrorCode.Category->GetTrimmedName()), ErrorCode.GetErrorTitle(),
		ErrorCode.GetErrorMessage()});
	}
}

class SECErrorCodeEntry : public SComboRow<TSharedPtr<FMGErrorCode>>
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
	SLATE_ARGUMENT(TSharedPtr<FMGErrorCode>, ErrorCode)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		ErrorCode = InArgs._ErrorCode;
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
					.Text(GetErrorCodeDisplayNameText(*ErrorCode))
					.ToolTipText(GetErrorCodeTooltip(*ErrorCode))
					.HighlightText(InArgs._HighlightText)
					.ColorAndOpacity(this, &SECErrorCodeEntry::GetTextColor)
					.IsEnabled(true)
				]
			];

		STableRow<TSharedPtr<FMGErrorCode>>::ConstructInternal(
			STableRow::FArguments().ShowSelection(true),
			InOwnerTableView);
	}

	FSlateColor GetTextColor() const
	{
		const TSharedPtr< ITypedTableView< TSharedPtr<FMGErrorCode> > > OwnerWidget = OwnerTablePtr.Pin();
		const bool bIsSelected = OwnerWidget->Private_IsItemSelected(ErrorCode);

		if (bIsSelected)
		{
			return FSlateColor::UseForeground();
		}
		return TextColor;
	}

private:
	FSlateColor TextColor;
	TSharedPtr<FMGErrorCode> ErrorCode;
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

	TSharedRef<ITableRow> GenerateListRowWidget(TSharedPtr<FMGErrorCode> Data,
		const TSharedRef<STableViewBase>& OwnerTable
		);

	void BroadcastErrorCodeSelected(TSharedPtr<FMGErrorCode> Item, ESelectInfo::Type SelectInfo);
	
	using FECErrorCodeTextFilter = TTextFilter<const FMGErrorCode&>;
	
	TSharedPtr<SSearchBox> SearchBox;

	TSharedPtr<SListView<TSharedPtr<FMGErrorCode>>> ErrorCodeListView;

	TArray<TSharedPtr<FMGErrorCode>> ListDatum;

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
				SAssignNew(ErrorCodeListView, SListView<TSharedPtr<FMGErrorCode>>)
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
	TSharedPtr<FMGErrorCode> SuccessCode = MakeShareable(new FMGErrorCode(FMGErrorCode::Success()));

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

			TSharedPtr<FMGErrorCode> Entry = MakeShareable(new FMGErrorCode(ErrorCode));
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

TSharedRef<ITableRow> SECErrorCodeListWidget::GenerateListRowWidget(TSharedPtr<FMGErrorCode> Data,
	const TSharedRef<STableViewBase>& OwnerTable
	)
{
	return SNew(SECErrorCodeEntry, OwnerTable)
		.HighlightText(SearchBox->GetText())
		.TextColor(FLinearColor(1.f, 1.f, 1.f, 1.f))
		.ErrorCode(Data);
}

void SECErrorCodeListWidget::BroadcastErrorCodeSelected(TSharedPtr<FMGErrorCode> Item,
	ESelectInfo::Type SelectInfo
	)
{
	PostErrorCodeSelected.ExecuteIfBound(*Item);
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
			.Text(this, &SECErrorCodeWidget::GetSelectedValueDisplayName)
		];

	ChildSlot
		[
			ComboButton.ToSharedRef()
		];
}

void SECErrorCodeWidget::SetCurrentValue(const FMGErrorCode& ErrorCode)
{
	SelectedErrorCode = ErrorCode;
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
	return GetErrorCodeTooltip(SelectedErrorCode);
}

FText SECErrorCodeWidget::GetSelectedValueDisplayName() const
{
	return GetErrorCodeDisplayNameText(SelectedErrorCode);
}

void SECErrorCodeWidget::BroadcastErrorCodeChanged(FMGErrorCode NewErrorCode)
{
	PostErrorCodeChanged.ExecuteIfBound(NewErrorCode);
	SelectedErrorCode = NewErrorCode;
	ComboButton->SetIsOpen(false);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
