// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphPin.h"
#include "Widgets/SCompoundWidget.h"

struct FECErrorCode;

/**
 * Graph pin for error codes.
 */
class SECGraphPin_ErrorCode : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SECGraphPin_ErrorCode) {}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

	virtual TSharedRef<SWidget> GetDefaultValueWidget() override;

	void UpdatePinValue(FECErrorCode NewValue);
};
