// Copyright 2022 Miraganic Studios. All rights reserved.

#include "ECErrorHandlingEditorModule.h"

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

void FECErrorHandlingEditorModule::StartupModule()
{
	ResultPinFactory = MakeShareable(new FECGraphPinFactory_Result());
	FEdGraphUtilities::RegisterVisualPinFactory(ResultPinFactory);
	SwitchResultNodeFactory = MakeShareable(new FECGraphNodeFactory_SwitchResult());
	FEdGraphUtilities::RegisterVisualNodeFactory(SwitchResultNodeFactory);

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout(FECResult::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FECCustomization_Result::MakeInstance));
	PropertyModule.RegisterCustomClassLayout(UECK2Node_SwitchResult::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FECCustomization_ResultSwitchNode::MakeInstance));

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	ErrorCategoryAssetActions = MakeShareable(new FECAssetTypeActions_ErrorCategory());
	AssetTools.RegisterAssetTypeActions(ErrorCategoryAssetActions.ToSharedRef());

	UECErrorCategory::PostChangedInEditor().AddRaw(this, &FECErrorHandlingEditorModule::BroadcastErrorCategoryChanged);
}

void FECErrorHandlingEditorModule::ShutdownModule()
{
	if (ResultPinFactory.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualPinFactory(ResultPinFactory);
		ResultPinFactory.Reset();
	}

	if (SwitchResultNodeFactory.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualNodeFactory(SwitchResultNodeFactory);
		SwitchResultNodeFactory.Reset();
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

void FECErrorHandlingEditorModule::BroadcastErrorCategoryChanged(
	const UECErrorCategory& ErrorCategory,
	const TArray<TPair<FName, int64>>& OldNames,
	bool bResolveData
)
{
	ErrorHandling::BroadcastPostChange(ErrorCategory, OldNames, bResolveData);
}

IMPLEMENT_MODULE(FECErrorHandlingEditorModule, MiraganicErrorHandlingEditor)
