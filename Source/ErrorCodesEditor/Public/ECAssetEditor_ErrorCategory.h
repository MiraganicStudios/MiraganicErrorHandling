// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"

class UECErrorCategoryEnum;
/**
 * 
 */
class ERRORCODESEDITOR_API FECAssetEditor_ErrorCategory : public FAssetEditorToolkit
{
public:
	virtual ~FECAssetEditor_ErrorCategory() override;
	
	void InitEditor(const EToolkitMode::Type Mode,
		const TSharedPtr<IToolkitHost>& InitToolkitHost,
		UECErrorCategoryEnum* ErrorCategory
	);

	//~ Begin IToolkit Interface
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;

	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	//~ End IToolkit Interface

protected:
	static FName GetErrorCodesTabId();
	static FName GetAppIdentifierName();
	static FName GetDefaultTabLayoutName();

	TSharedRef<SDockTab> SpawnErrorCodesTab(const FSpawnTabArgs& Args);

	void FillToolbar(FToolBarBuilder& ToolbarBuilder);
	
	TWeakObjectPtr<UECErrorCategoryEnum> EditingErrorCategory;

	TSharedPtr<IDetailsView> PropertyView;
};
