#include "ECErrorCodesEditorModule.h"

#include "AssetToolsModule.h"
#include "ECAssetTypeActions_ErrorCategory.h"
#include "ECCustomization_ErrorCategory.h"
#include "ECCustomization_ErrorCode.h"
#include "ECErrorCategory.h"
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
	PropertyModule.RegisterCustomClassLayout("ECErrorCategory",
		FOnGetDetailCustomizationInstance::CreateStatic(&FECCustomization_ErrorCategory::MakeInstance));
	
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	ErrorCategoryAssetActions = MakeShareable(new FECAssetTypeActions_ErrorCategory());
	AssetTools.RegisterAssetTypeActions(ErrorCategoryAssetActions.ToSharedRef());
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
	}

	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		if (ErrorCategoryAssetActions.IsValid())
		{
			AssetTools.UnregisterAssetTypeActions(ErrorCategoryAssetActions.ToSharedRef());
		}
	}
}
    
IMPLEMENT_MODULE(FECErrorCodesEditorModule, ErrorCodesEditor)