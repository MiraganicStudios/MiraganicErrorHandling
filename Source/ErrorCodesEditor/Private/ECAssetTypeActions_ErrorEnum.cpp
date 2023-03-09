// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECAssetTypeActions_ErrorEnum.h"

#include "ECAssetEditor_ErrorCategory.h"

FText FECAssetTypeActions_ErrorEnum::GetAssetDescription(const FAssetData& AssetData) const
{
	return AssetData.GetTagValueRef<FText>(GET_MEMBER_NAME_CHECKED(UECErrorCategoryEnum, EnumDescription));
}

void FECAssetTypeActions_ErrorEnum::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> InitToolkitHost
	)
{
	const EToolkitMode::Type Mode = InitToolkitHost.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UECErrorCategoryEnum* ErrorCategory = Cast<UECErrorCategoryEnum>(*ObjIt))
		{
			TSharedRef<FECAssetEditor_ErrorCategory> ErrorCategoryAssetEditor = MakeShareable(new FECAssetEditor_ErrorCategory());
			ErrorCategoryAssetEditor->InitEditor(Mode, InitToolkitHost, ErrorCategory);
		}
	}
}
