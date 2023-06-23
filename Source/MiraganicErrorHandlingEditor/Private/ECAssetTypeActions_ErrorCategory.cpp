// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.


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
