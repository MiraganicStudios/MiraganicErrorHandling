// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "ECResultCode.h"

DECLARE_DELEGATE_OneParam(FECResultCodeChangedDelegate, FECResultCode);

/**
 * Widget which selects a single Error Code from a dropdown list.
 */
class SECWidget_ResultCode : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SECWidget_ResultCode) {}
	SLATE_ARGUMENT(FString, FilterString)
	SLATE_ARGUMENT(FECResultCode, DefaultValue)
	SLATE_EVENT(FECResultCodeChangedDelegate, PostResultCodeChanged)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void SetSelectedResultCode(const FECResultCode& ResultCode);

protected:
	TSharedRef<SWidget> GenerateDropdownWidget();
	FText FormatToolTipText() const;
	FText GetSelectedValueCategoryAndTitle() const;
	FText GetSelectedValueTitle() const;
	void BroadcastResultCodeChanged(FECResultCode NewResultCode);

	FECResultCodeChangedDelegate PostResultCodeChanged;

	/** String used to filter error codes*/
	FString FilterString;

	FECResultCode SelectedResultCode;

	TSharedPtr<class SComboButton> ComboButton;
};
