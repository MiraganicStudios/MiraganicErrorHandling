﻿// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "ECErrorCategoryEnum.h"

/**
 * 
 */
class FECAssetTypeActions_ErrorEnum : public FAssetTypeActions_Base
{
public:
	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return NSLOCTEXT("ErrorCodesEditor_ErrorCategoryAssetActions", "DisplayName", "Error Category"); }
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override;
	virtual FColor GetTypeColor() const override { return FColor(209, 71, 71); }
	virtual UClass* GetSupportedClass() const override { return UECErrorCategoryEnum::StaticClass(); }
	virtual uint32 GetCategories() override { return EAssetTypeCategories::Blueprint; }
	virtual bool CanLocalize() const override { return false; }

	virtual void OpenAssetEditor( const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> InitToolkitHost = TSharedPtr<IToolkitHost>() ) override;
};
