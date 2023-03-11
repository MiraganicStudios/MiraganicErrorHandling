// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECAssetTypeActions_ErrorCategory.h"

#include "ECAssetEditor_ErrorCategory.h"

FText FECAssetTypeActions_ErrorCategory::GetAssetDescription(const FAssetData& AssetData) const
{
	return AssetData.GetTagValueRef<FText>(GET_MEMBER_NAME_CHECKED(UECErrorCategory, EnumDescription));
}

void FECAssetTypeActions_ErrorCategory::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> InitToolkitHost
	)
{
	const EToolkitMode::Type Mode = InitToolkitHost.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UECErrorCategory* ErrorCategory = Cast<UECErrorCategory>(*ObjIt))
		{
			TSharedRef<FECAssetEditor_ErrorCategory> ErrorCategoryAssetEditor = MakeShareable(new FECAssetEditor_ErrorCategory());
			ErrorCategoryAssetEditor->InitEditor(Mode, InitToolkitHost, ErrorCategory);
		}
	}
}
