#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class UECErrorCategory;

class FECErrorCodesEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
	void BroadcastErrorCategoryChanged(const UECErrorCategory& ErrorCategory,
		const TArray<TPair<FName, int64>>& OldNames,
		bool bResolveData
	);
	
	TSharedPtr<class FECGraphPinFactory_Result> ErrorCodePinFactory;
	TSharedPtr<class FECGraphNodeFactory_SwitchResult> SwitchErrorCodeNodeFactory;

	TSharedPtr<class IAssetTypeActions> ErrorCategoryAssetActions;
};
