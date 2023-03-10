// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class UECK2Node_SwitchErrorCode;

class ERRORCODESEDITOR_API FECCustomization_ErrorCodeSwitchNode : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	TSharedRef<SWidget> GenerateAddCategoryMenu();

	void AddUniqueCodesFromCategory(const UEnum* ErrorCategory);
	
	TWeakObjectPtr<UECK2Node_SwitchErrorCode> TargetNode;

	TSharedPtr<SComboButton> AddCategoryComboButton;
	
};
