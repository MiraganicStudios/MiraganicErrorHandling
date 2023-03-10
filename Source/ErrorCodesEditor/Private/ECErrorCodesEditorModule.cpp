// Copyright 2022 Miraganic Studios. All rights reserved.

#include "ECErrorCodesEditorModule.h"

#include "AssetToolsModule.h"
#include "ECAssetTypeActions_ErrorEnum.h"
#include "ECCustomization_ErrorCode.h"
#include "ECCustomization_ErrorCodeSwitchNode.h"
#include "ECEditorLogging.h"
#include "ECErrorCategoryUtils.h"
#include "ECGraphPinFactory_ErrorCode.h"
#include "ECGraphNodeFactory_SwitchErrorCode.h"
#include "EdGraphUtilities.h"
#include "ECErrorCode.h"
#include "ECK2Node_SwitchErrorCode.h"

void FECErrorCodesEditorModule::StartupModule()
{
	ErrorCodePinFactory = MakeShareable(new FECGraphPinFactory_ErrorCode());
	FEdGraphUtilities::RegisterVisualPinFactory(ErrorCodePinFactory);
	SwitchErrorCodeNodeFactory = MakeShareable(new FECGraphNodeFactory_SwitchErrorCode());
	FEdGraphUtilities::RegisterVisualNodeFactory(SwitchErrorCodeNodeFactory);

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout(FECErrorCode::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FECCustomization_ErrorCode::MakeInstance));
	PropertyModule.RegisterCustomClassLayout(UECK2Node_SwitchErrorCode::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FECCustomization_ErrorCodeSwitchNode::MakeInstance));

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	ErrorCategoryAssetActions = MakeShareable(new FECAssetTypeActions_ErrorEnum());
	AssetTools.RegisterAssetTypeActions(ErrorCategoryAssetActions.ToSharedRef());

	UECErrorCategoryEnum::PostChangedInEditor().AddRaw(this, &FECErrorCodesEditorModule::BroadcastErrorCategoryChanged);
}

void FECErrorCodesEditorModule::ShutdownModule()
{
	if (ErrorCodePinFactory.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualPinFactory(ErrorCodePinFactory);
		ErrorCodePinFactory.Reset();		
	}

	if (SwitchErrorCodeNodeFactory.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualNodeFactory(SwitchErrorCodeNodeFactory);
		SwitchErrorCodeNodeFactory.Reset();
	}

	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout(FECErrorCode::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomClassLayout(UECK2Node_SwitchErrorCode::StaticClass()->GetFName());
	}

	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		if (ErrorCategoryAssetActions.IsValid())
		{
			AssetTools.UnregisterAssetTypeActions(ErrorCategoryAssetActions.ToSharedRef());
		}
	}

	UECErrorCategoryEnum::PostChangedInEditor().RemoveAll(this);
}

void FECErrorCodesEditorModule::BroadcastErrorCategoryChanged(const UECErrorCategoryEnum& ErrorCategory,
	const TArray<TPair<FName, int64>>& OldNames,
	bool bResolveData
	)
{
	ErrorCodes::BroadcastChanges(ErrorCategory, OldNames, bResolveData);
}

IMPLEMENT_MODULE(FECErrorCodesEditorModule, ErrorCodesEditor)
