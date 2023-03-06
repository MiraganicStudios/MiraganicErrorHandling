#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FECErrorCodesEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
	TSharedPtr<class FECGraphPinFactory_ErrorCode> ErrorCodePinFactory;

	TSharedPtr<class IAssetTypeActions> ErrorCategoryAssetActions;
};
