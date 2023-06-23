// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class UECK2Node_SwitchResult;

class FECCustomization_ResultSwitchNode : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	TSharedRef<SWidget> GenerateAddCategoryMenu();

	void AddUniqueCodesFromCategory(const UEnum* ErrorCategory);
	
	TWeakObjectPtr<UECK2Node_SwitchResult> TargetNode;

	TSharedPtr<SComboButton> AddCategoryComboButton;
	
};
