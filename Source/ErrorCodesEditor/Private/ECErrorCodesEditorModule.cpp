#include "ECErrorCodesEditorModule.h"

#include "AssetToolsModule.h"
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
	
	//IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	
}

void FECErrorCodesEditorModule::ShutdownModule()
{
	if (ErrorCodePinFactory.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualPinFactory(ErrorCodePinFactory);
		ErrorCodePinFactory.Reset();		
	}

	if (FPropertyEditorModule* PropertyModule = FModuleManager::LoadModulePtr<FPropertyEditorModule>("PropertyEditor"))
	{
		PropertyModule->UnregisterCustomPropertyTypeLayout(FECErrorCode::StaticStruct()->GetFName());
	}
}
    
IMPLEMENT_MODULE(FECErrorCodesEditorModule, ErrorCodesEditor)