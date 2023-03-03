// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphPin.h"
#include "Widgets/SCompoundWidget.h"

struct FMGErrorCode;
/**
 * 
 */
class ERRORCODESEDITOR_API SECErrorCodeGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SECErrorCodeGraphPin) {}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

	virtual TSharedRef<SWidget> GetDefaultValueWidget() override;

	void UpdatePinValue(FMGErrorCode NewValue);
};
