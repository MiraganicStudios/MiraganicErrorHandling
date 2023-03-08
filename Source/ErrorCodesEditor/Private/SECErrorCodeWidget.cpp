// Copyright 2022 Miraganic Studios. All rights reserved.


#include "SECErrorCodeWidget.h"

#include "ClassViewerModule.h"
#include "ECErrorCategory.h"
#include "SlateOptMacros.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/TextFilter.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SSeparator.h"

#define LOCTEXT_NAMESPACE "ErrorCodes"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

FString GetErrorCodeDisplayName(const FECErrorCode& ErrorCode)
{
	return ErrorCode.ToShortString();
}

FText GetErrorCodeDisplayNameText(const FECErrorCode& ErrorCode)
{
	if (ErrorCode.IsError())
	{
		return FText::Format(LOCTEXT("ErrorCodeWidget_TooltipFormat", "{0}:{1}"),
		{FText::FromString(ErrorCode.GetCategory()->GetTrimmedName()), ErrorCode.GetTitle()});
	}
	else
	{
		return ErrorCode.GetTitle();
	}
}

FText GetErrorCodeTooltip(const FECErrorCode& ErrorCode)
{
	return ErrorCode.GetFormattedMessage();
}

class SECErrorCodeEntry : public SComboRow<TSharedPtr<FECErrorCode>>
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
	SLATE_ARGUMENT(TSharedPtr<FECErrorCode>, ErrorCode)
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
			return FSlateColor::UseForeground();
		}
		return TextColor;
	}

private:
	FSlateColor TextColor;
	TSharedPtr<FECErrorCode> ErrorCode;
};

class SECErrorCodeListWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SECErrorCodeListWidget)
		: _bAutoFocus(true)
	{
	}

	SLATE_ARGUMENT(FString, FilterString)
	SLATE_EVENT(FECErrorCodeChangedDelegate, PostErrorCodeSelected)
	SLATE_ARGUMENT(bool, bAutoFocus)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	virtual ~SECErrorCodeListWidget()
	{
		if (PostErrorCodeSelected.IsBound())
		{
			PostErrorCodeSelected.Unbind();
		}
	}

	static void GetErrorCodeSearchTerms(const FECErrorCode& ErrorCode, TArray<FString>& OutSearchTerms)
	{
		OutSearchTerms.Emplace(ErrorCode.ToString());
	}

	TSharedPtr<SSearchBox> GetSearchBox() const { return SearchBox; }

private:
	void UpdateErrorCodeOptions();

	void UpdateFilterText(const FText& InFilterText);

	TSharedRef<ITableRow> GenerateListRowWidget(TSharedPtr<FECErrorCode> Data,
		const TSharedRef<STableViewBase>& OwnerTable
		);

	void BroadcastErrorCodeSelected(TSharedPtr<FECErrorCode> Item, ESelectInfo::Type SelectInfo);
	
	using FECErrorCodeTextFilter = TTextFilter<const FECErrorCode&>;
	
	TSharedPtr<SSearchBox> SearchBox;

	TSharedPtr<SListView<TSharedPtr<FECErrorCode>>> ErrorCodeListView;

	TArray<TSharedPtr<FECErrorCode>> ListRows;

	TSharedPtr<FECErrorCodeTextFilter> SearchFilter;
	
	FString FilterString;
	
	FECErrorCodeChangedDelegate PostErrorCodeSelected;

	bool bAwaitingFocus = false;
};

void SECErrorCodeListWidget::Construct(const FArguments& InArgs)
{
	FilterString = InArgs._FilterString;
	PostErrorCodeSelected = InArgs._PostErrorCodeSelected;
	bAwaitingFocus = InArgs._bAutoFocus;
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
				SAssignNew(ErrorCodeListView, SListView<TSharedPtr<FECErrorCode>>)
				.Visibility(EVisibility::Visible)
				.SelectionMode(ESelectionMode::Single)
				.ListItemsSource(&ListRows)
				.OnGenerateRow(this, &SECErrorCodeListWidget::GenerateListRowWidget)
				.OnSelectionChanged(this, &SECErrorCodeListWidget::BroadcastErrorCodeSelected)
			]
		];
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
	ListRows.Reset();
	TSharedPtr<FECErrorCode> SuccessCode = MakeShareable(new FECErrorCode(FECErrorCode::Success()));

	ListRows.Emplace(SuccessCode);

	// Gather all C++ classes
	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		UClass* Class = *ClassIt;
		if (!IsValid(Class) || !Class->IsChildOf(UECErrorCategory::StaticClass()))
		{
			continue;
		}

		// BP classes will be added below
		if (Cast<UBlueprintGeneratedClass>(Class))
		{
			continue;
		}

		const UECErrorCategory* CategoryCDO = Class->GetDefaultObject<UECErrorCategory>();
		if (!CategoryCDO)
		{
			continue;
		}

		for (const auto& ErrorCodePair : CategoryCDO->Errors)
		{
			FECErrorCode ErrorCode(Class, ErrorCodePair.Key);
			if (SearchFilter.IsValid() && !SearchFilter->PassesFilter(ErrorCode))
			{
				continue;
			}

			TSharedPtr<FECErrorCode> Entry = MakeShareable(new FECErrorCode(ErrorCode));
			ListRows.Emplace(Entry);
		}
	}

	// Gather loaded and unloaded ErrorCategory blueprints
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	TArray<FAssetData> BlueprintList;
	FARFilter AssetFilter;
	AssetFilter.ClassNames.Add(UBlueprint::StaticClass()->GetFName());
	//AssetFilter.ClassNames.Add(UBlueprintGeneratedClass::StaticClass()->GetFName());
	AssetFilter.bRecursiveClasses = true;
	AssetRegistryModule.Get().GetAssets(AssetFilter, BlueprintList);
	UClass* ErrorCategoryClass = UECErrorCategory::StaticClass();
	// Tag added by blueprints
	const FString ErrorCategoryNativeClassName = FString::Printf(TEXT("%s'%s'"), *ErrorCategoryClass->GetClass()->GetName(),
		*ErrorCategoryClass->GetPathName());
	for (const FAssetData& AssetData : BlueprintList)
	{
		// Filter by tag so we don't load every blueprint
		FString NativeParentClassName;
		const bool bFoundTag = AssetData.GetTagValue(FBlueprintTags::NativeParentClassPath, NativeParentClassName);
		if (!bFoundTag || NativeParentClassName != ErrorCategoryNativeClassName)
		{
			continue;
		}
		
		UBlueprint* Blueprint = Cast<UBlueprint>(AssetData.FastGetAsset(true));
		if (!IsValid(Blueprint))
		{
			continue;
		}
		
		UBlueprintGeneratedClass* Class = Cast<UBlueprintGeneratedClass>(Blueprint->GeneratedClass);
		if (!IsValid(Class) || !Class->IsChildOf<UECErrorCategory>() ||
			Class->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists | CLASS_Hidden) ||
			FKismetEditorUtilities::IsClassABlueprintSkeleton(Class))
		{
			continue;
		}
	
		const UECErrorCategory* CategoryCDO = Class->GetDefaultObject<UECErrorCategory>();
		if (!CategoryCDO)
		{
			continue;
		}
		
		for (const auto& ErrorCodePair : CategoryCDO->Errors)
		{
			FECErrorCode ErrorCode(Class, ErrorCodePair.Key);
			if (SearchFilter.IsValid() && !SearchFilter->PassesFilter(ErrorCode))
			{
				continue;
			}
	
			TSharedPtr<FECErrorCode> Entry = MakeShareable(new FECErrorCode(ErrorCode));
			ListRows.Emplace(Entry);
		}
	}

	// Sort all error codes; always leave 'Success' at the top
	if (ListRows.Num() > 1)
	{
		TArrayView<TSharedPtr<FECErrorCode>> ErrorCodes(ListRows.GetData() + 1, ListRows.Num() - 1);
		ErrorCodes.Sort([](const TSharedPtr<FECErrorCode>& A, const TSharedPtr<FECErrorCode>& B)
		{
			const FString StrA = A->ToShortString();
			const FString StrB = B->ToShortString();
			return StrA.Compare(StrB) < 0;
		});
	}
}

void SECErrorCodeListWidget::UpdateFilterText(const FText& InFilterText)
{
	SearchFilter->SetRawFilterText(InFilterText);
	SearchBox->SetError(SearchFilter->GetFilterErrorText());

	UpdateErrorCodeOptions();
}

TSharedRef<ITableRow> SECErrorCodeListWidget::GenerateListRowWidget(TSharedPtr<FECErrorCode> Data,
	const TSharedRef<STableViewBase>& OwnerTable
	)
{
	return SNew(SECErrorCodeEntry, OwnerTable)
		.HighlightText(SearchBox->GetText())
		.TextColor(FLinearColor(1.f, 1.f, 1.f, 1.f))
		.ErrorCode(Data);
}

void SECErrorCodeListWidget::BroadcastErrorCodeSelected(TSharedPtr<FECErrorCode> Item,
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

void SECErrorCodeWidget::SetCurrentValue(const FECErrorCode& ErrorCode)
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
				.bAutoFocus(true)
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

void SECErrorCodeWidget::BroadcastErrorCodeChanged(FECErrorCode NewErrorCode)
{
	PostErrorCodeChanged.ExecuteIfBound(NewErrorCode);
	SelectedErrorCode = NewErrorCode;
	ComboButton->SetIsOpen(false);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
