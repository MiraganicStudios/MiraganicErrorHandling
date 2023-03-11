// Copyright 2022 Miraganic Studios. All rights reserved.

#include "ECErrorCodesEditorModule.h"

#include "AssetToolsModule.h"
#include "ECAssetTypeActions_ErrorCategory.h"
#include "ECCustomization_ResultCode.h"
#include "ECCustomization_ResultCodeSwitchNode.h"
#include "ECEditorLogging.h"
#include "ECErrorCategoryUtils.h"
#include "ECGraphPinFactory_ResultCode.h"
#include "ECGraphNodeFactory_SwitchResultCode.h"
#include "EdGraphUtilities.h"
#include "ECResultCode.h"
#include "ECK2Node_SwitchResultCode.h"

void FECErrorCodesEditorModule::StartupModule()
{
	ErrorCodePinFactory = MakeShareable(new FECGraphPinFactory_ResultCode());
	FEdGraphUtilities::RegisterVisualPinFactory(ErrorCodePinFactory);
	SwitchErrorCodeNodeFactory = MakeShareable(new FECGraphNodeFactory_SwitchResultCode());
	FEdGraphUtilities::RegisterVisualNodeFactory(SwitchErrorCodeNodeFactory);

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout(FECResultCode::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FECCustomization_ResultCode::MakeInstance));
	PropertyModule.RegisterCustomClassLayout(UECK2Node_SwitchResultCode::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FECCustomization_ResultCodeSwitchNode::MakeInstance));

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	ErrorCategoryAssetActions = MakeShareable(new FECAssetTypeActions_ErrorCategory());
	AssetTools.RegisterAssetTypeActions(ErrorCategoryAssetActions.ToSharedRef());

	UECErrorCategory::PostChangedInEditor().AddRaw(this, &FECErrorCodesEditorModule::BroadcastErrorCategoryChanged);
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
		PropertyModule.UnregisterCustomPropertyTypeLayout(FECResultCode::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomClassLayout(UECK2Node_SwitchResultCode::StaticClass()->GetFName());
	}

	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		if (ErrorCategoryAssetActions.IsValid())
		{
			AssetTools.UnregisterAssetTypeActions(ErrorCategoryAssetActions.ToSharedRef());
		}
	}

	UECErrorCategory::PostChangedInEditor().RemoveAll(this);
}

void FECErrorCodesEditorModule::BroadcastErrorCategoryChanged(const UECErrorCategory& ErrorCategory,
	const TArray<TPair<FName, int64>>& OldNames,
	bool bResolveData
	)
{
	ErrorCodes::BroadcastPostChange(ErrorCategory, OldNames, bResolveData);
}

IMPLEMENT_MODULE(FECErrorCodesEditorModule, ErrorCodesEditor)
