// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class UECK2Node_SwitchResultCode;

class ERRORCODESEDITOR_API FECCustomization_ResultCodeSwitchNode : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	TSharedRef<SWidget> GenerateAddCategoryMenu();

	void AddUniqueCodesFromCategory(const UEnum* ErrorCategory);
	
	TWeakObjectPtr<UECK2Node_SwitchResultCode> TargetNode;

	TSharedPtr<SComboButton> AddCategoryComboButton;
	
};
