// Copyright 2022 Miraganic Studios. All rights reserved.

#include "ECErrorCodesEditorModule.h"

#include "AssetToolsModule.h"
#include "ECAssetTypeActions_ErrorCategory.h"
#include "ECCustomization_Result.h"
#include "ECCustomization_ResultSwitchNode.h"
#include "ECEditorLogging.h"
#include "ECErrorCategoryUtils.h"
#include "ECGraphPinFactory_Result.h"
#include "ECGraphNodeFactory_SwitchResult.h"
#include "EdGraphUtilities.h"
#include "ECResult.h"
#include "ECK2Node_SwitchResult.h"

void FECErrorCodesEditorModule::StartupModule()
{
	ErrorCodePinFactory = MakeShareable(new FECGraphPinFactory_Result());
	FEdGraphUtilities::RegisterVisualPinFactory(ErrorCodePinFactory);
	SwitchErrorCodeNodeFactory = MakeShareable(new FECGraphNodeFactory_SwitchResult());
	FEdGraphUtilities::RegisterVisualNodeFactory(SwitchErrorCodeNodeFactory);

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout(FECResult::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FECCustomization_Result::MakeInstance));
	PropertyModule.RegisterCustomClassLayout(UECK2Node_SwitchResult::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FECCustomization_ResultSwitchNode::MakeInstance));

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
		PropertyModule.UnregisterCustomPropertyTypeLayout(FECResult::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomClassLayout(UECK2Node_SwitchResult::StaticClass()->GetFName());
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
