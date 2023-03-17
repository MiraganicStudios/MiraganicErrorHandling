// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "ECResult.h"

DECLARE_DELEGATE_OneParam(FECResultChangedDelegate, FECResult);

/**
 * Widget which selects a single Error Code from a dropdown list.
 */
class SECWidget_Result : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SECWidget_Result) {}
	SLATE_ARGUMENT(FString, FilterString)
	SLATE_ARGUMENT(FECResult, DefaultValue)
	SLATE_EVENT(FECResultChangedDelegate, PostResultCodeChanged)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void SetSelectedResultCode(const FECResult& Result);

protected:
	TSharedRef<SWidget> GenerateDropdownWidget();
	FText FormatToolTipText() const;
	FText GetSelectedValueCategoryAndTitle() const;
	FText GetSelectedValueTitle() const;
	void BroadcastResultCodeChanged(FECResult NewResult);

	FECResultChangedDelegate PostResultChanged;

	/** String used to filter error codes*/
	FString FilterString;

	FECResult SelectedResult;

	TSharedPtr<class SComboButton> ComboButton;
};
