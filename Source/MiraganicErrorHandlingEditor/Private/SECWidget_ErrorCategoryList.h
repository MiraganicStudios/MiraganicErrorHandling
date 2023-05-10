// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/TextFilter.h"
#include "Widgets/SCompoundWidget.h"

DECLARE_DELEGATE_OneParam(FECErrorCategoryPickedDelegate, const UEnum*);

/**
 * Widget which selects a single Error Category from a dropdown list.
 */
class SECWidget_ErrorCategoryList : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SECWidget_ErrorCategoryList)
		: _bAutoFocus(true)
	{}
	SLATE_ARGUMENT(bool, bAutoFocus)
	SLATE_EVENT(FECErrorCategoryPickedDelegate, PostErrorCategoryPicked)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	static void GetErrorCategorySearchTerms(const UEnum* ErrorCategory, TArray<FString>& OutSearchTerms);

private:
	void UpdateErrorCategoryOptions();
	void FilterErrorCategoryOptions();
	void UpdateFilterText(const FText& InFilterText);

	TSharedRef<ITableRow> GenerateListRowWidget(const UEnum* ErrorCategory,
		const TSharedRef<STableViewBase>& OwnerTable
	);

	void BroadcastErrorCategoryPicked(const UEnum* ErrorCategory, ESelectInfo::Type SelectInfo);

	using FECErrorCategoryTextFilter = TTextFilter<const UEnum*>;

	TSharedPtr<SSearchBox> SearchBox;

	TSharedPtr<SListView<const UEnum*>> ErrorCategoryListView;

	TArray<const UEnum*> AllCategoryOptions;
	TArray<const UEnum*> FilteredCategoryOptions;

	TSharedPtr<FECErrorCategoryTextFilter> SearchFilter;

	FECErrorCategoryPickedDelegate PostErrorCategoryPicked;

	bool bAwaitingFocus = false;
};
