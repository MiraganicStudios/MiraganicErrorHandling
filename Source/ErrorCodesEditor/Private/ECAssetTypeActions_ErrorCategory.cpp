// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECAssetTypeActions_ErrorCategory.h"

#include "ECErrorCategory.h"

UClass* FECAssetTypeActions_ErrorCategory::GetSupportedClass() const
{
	return UECErrorCategory::StaticClass();
}

FText FECAssetTypeActions_ErrorCategory::GetAssetDescription(const FAssetData& AssetData) const
{
	return NSLOCTEXT("ErrorCodes", "AssetDescription_ErrorCategory", "A collection of error codes; stores title and message for each error code.");
}

uint32 FECAssetTypeActions_ErrorCategory::GetCategories()
{
	return EAssetTypeCategories::Misc;
}
