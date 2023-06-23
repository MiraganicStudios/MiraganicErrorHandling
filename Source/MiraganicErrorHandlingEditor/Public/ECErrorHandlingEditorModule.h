// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

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
