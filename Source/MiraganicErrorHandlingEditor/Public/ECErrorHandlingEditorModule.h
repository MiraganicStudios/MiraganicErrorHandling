// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class UECErrorCategory;

class FECErrorHandlingEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
	void BroadcastErrorCategoryChanged(const UECErrorCategory& ErrorCategory,
		const TArray<TPair<FName, int64>>& OldNames,
		bool bResolveData
	);
	
	TSharedPtr<class FECGraphPinFactory_Result> ResultPinFactory;
	TSharedPtr<class FECGraphNodeFactory_SwitchResult> SwitchResultNodeFactory;

	TSharedPtr<class IAssetTypeActions> ErrorCategoryAssetActions;
};
