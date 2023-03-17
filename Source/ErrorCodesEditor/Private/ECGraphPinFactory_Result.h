// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"

class FECGraphPinFactory_Result : public FGraphPanelPinFactory
{
public:
	virtual TSharedPtr<SGraphPin> CreatePin(UEdGraphPin* Pin) const override;
};
