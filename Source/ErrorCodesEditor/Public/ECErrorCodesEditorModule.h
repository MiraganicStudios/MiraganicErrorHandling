#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class UECErrorCategoryEnum;

class FECErrorCodesEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
	void BroadcastErrorCategoryChanged(const UECErrorCategoryEnum& ErrorCategory,
		const TArray<TPair<FName, int64>>& OldNames,
		bool bResolveData
	);
	
	TSharedPtr<class FECGraphPinFactory_ErrorCode> ErrorCodePinFactory;

	TSharedPtr<class IAssetTypeActions> ErrorCategoryAssetActions;
};
