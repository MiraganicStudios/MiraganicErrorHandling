// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "ECErrorCategory.h"

/**
 * 
 */
class FECAssetTypeActions_ErrorCategory : public FAssetTypeActions_Base
{
public:
	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return NSLOCTEXT("ErrorCodesEditor_ErrorCategoryAssetActions", "DisplayName", "Error Category"); }
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override;
	virtual FColor GetTypeColor() const override { return FColor(209, 71, 71); }
	virtual UClass* GetSupportedClass() const override { return UECErrorCategory::StaticClass(); }
	virtual uint32 GetCategories() override { return EAssetTypeCategories::Blueprint; }
	virtual bool CanLocalize() const override { return false; }

	virtual void OpenAssetEditor( const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> InitToolkitHost = TSharedPtr<IToolkitHost>() ) override;
};
