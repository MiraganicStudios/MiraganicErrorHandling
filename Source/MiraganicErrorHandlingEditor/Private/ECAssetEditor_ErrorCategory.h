// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"

class UECErrorCategory;
/**
 * 
 */
class FECAssetEditor_ErrorCategory : public FAssetEditorToolkit
{
public:
	virtual ~FECAssetEditor_ErrorCategory() override;
	
	void InitEditor(const EToolkitMode::Type Mode,
		const TSharedPtr<IToolkitHost>& InitToolkitHost,
		UECErrorCategory* ErrorCategory
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
	static FName GetResultsTabId();
	static FName GetAppIdentifierName();
	static FName GetDefaultTabLayoutName();

	TSharedRef<SDockTab> SpawnResultsTab(const FSpawnTabArgs& Args);

	void FillToolbar(FToolBarBuilder& ToolbarBuilder);
	
	TWeakObjectPtr<UECErrorCategory> EditingErrorCategory;

	TSharedPtr<IDetailsView> PropertyView;
};
