// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "MGErrorCode.h"

DECLARE_DELEGATE_OneParam(FECErrorCodeChangedDelegate, FMGErrorCode);

/**
 * Widget which handles an error code.
 */
class ERRORCODESEDITOR_API SECErrorCodeWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SECErrorCodeWidget) {}
	SLATE_ARGUMENT(FString, FilterString)
	SLATE_ARGUMENT(FMGErrorCode, DefaultValue)
	SLATE_EVENT(FECErrorCodeChangedDelegate, PostErrorCodeChanged)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void SetCurrentValue(const FMGErrorCode& ErrorCode);

protected:
	TSharedRef<SWidget> GenerateDropdownWidget();
	FText FormatToolTipText() const;
	FText GetSelectedValueDisplayName() const;
	void BroadcastErrorCodeChanged(FMGErrorCode NewErrorCode);

	FECErrorCodeChangedDelegate PostErrorCodeChanged;

	/** String used to filter error codes*/
	FString FilterString;

	FMGErrorCode SelectedErrorCode;

	TSharedPtr<class SComboButton> ComboButton;
	
};
