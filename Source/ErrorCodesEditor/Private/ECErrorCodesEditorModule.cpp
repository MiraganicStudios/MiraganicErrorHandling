// Copyright 2022 Miraganic Studios. All rights reserved.

#include "ECErrorCodesEditorModule.h"

#include "AssetToolsModule.h"
#include "ECAssetTypeActions_ErrorEnum.h"
#include "ECCustomization_ErrorCategory.h"
#include "ECCustomization_ErrorCode.h"
#include "ECEditorLogging.h"
#include "ECErrorCategory.h"
#include "ECErrorCategoryUtils.h"
#include "ECGraphPinFactory_ErrorCode.h"
#include "EdGraphUtilities.h"
#include "ECErrorCode.h"

void FECErrorCodesEditorModule::StartupModule()
{
	ErrorCodePinFactory = MakeShareable(new FECGraphPinFactory_ErrorCode());
	FEdGraphUtilities::RegisterVisualPinFactory(ErrorCodePinFactory);

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout(FECErrorCode::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FECCustomization_ErrorCode::MakeInstance));
	PropertyModule.RegisterCustomClassLayout(UECErrorCategory::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FECCustomization_ErrorCategory::MakeInstance));

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	ErrorCategoryAssetActions = MakeShareable(new FECAssetTypeActions_ErrorEnum());
	AssetTools.RegisterAssetTypeActions(ErrorCategoryAssetActions.ToSharedRef());

	UECErrorCategoryEnum::PostChanged().AddRaw(this, &FECErrorCodesEditorModule::BroadcastErrorCategoryChanged);
}

void FECErrorCodesEditorModule::ShutdownModule()
{
	if (ErrorCodePinFactory.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualPinFactory(ErrorCodePinFactory);
		ErrorCodePinFactory.Reset();		
	}

	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout(FECErrorCode::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomClassLayout(UECErrorCategory::StaticClass()->GetFName());
	}

	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		if (ErrorCategoryAssetActions.IsValid())
		{
			AssetTools.UnregisterAssetTypeActions(ErrorCategoryAssetActions.ToSharedRef());
		}
	}

	UECErrorCategoryEnum::PostChanged().RemoveAll(this);
}

void FECErrorCodesEditorModule::BroadcastErrorCategoryChanged(const UECErrorCategoryEnum& ErrorCategory,
	const TArray<TPair<FName, int64>>& OldNames,
	bool bResolveData
	)
{
	ErrorCodes::BroadcastChanges(ErrorCategory, OldNames, bResolveData);
}

IMPLEMENT_MODULE(FECErrorCodesEditorModule, ErrorCodesEditor)
