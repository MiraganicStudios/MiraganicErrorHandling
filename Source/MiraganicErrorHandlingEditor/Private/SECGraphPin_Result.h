// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include "CoreMinimal.h"
#include "SGraphPin.h"
#include "Widgets/SCompoundWidget.h"

struct FECResult;

/**
 * Graph pin for error codes.
 */
class SECGraphPin_Result : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SECGraphPin_Result) {}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

	virtual TSharedRef<SWidget> GetDefaultValueWidget() override;

	void UpdatePinValue(FECResult NewValue);

};
