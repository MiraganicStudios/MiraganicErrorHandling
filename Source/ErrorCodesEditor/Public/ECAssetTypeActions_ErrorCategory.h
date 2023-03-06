// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

class ERRORCODESEDITOR_API FECAssetTypeActions_ErrorCategory : public FAssetTypeActions_Base
{
public:
	virtual FText GetName() const override { return NSLOCTEXT("ErrorCodes", "AssetTypeActions_ErrorCategory_Name", "Error Category"); }
	virtual FColor GetTypeColor() const override { return FColor(209, 71, 71); }
	virtual UClass* GetSupportedClass() const override;
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override;
	virtual uint32 GetCategories() override;
};
