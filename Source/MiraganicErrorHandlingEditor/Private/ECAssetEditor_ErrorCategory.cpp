// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.


#include "ECAssetEditor_ErrorCategory.h"

#include "ECCustomization_ErrorCategory.h"
#include "ECErrorCategory.h"
#include "Styling/ToolBarStyle.h"
#include "SPositiveActionButton.h"
#include "Kismet2/EnumEditorUtils.h"

#define LOCTEXT_NAMESPACE "ErrorHandlingEditor_ErrorCategoryAssetEditor"

FECAssetEditor_ErrorCategory::~FECAssetEditor_ErrorCategory()
{
}

void FECAssetEditor_ErrorCategory::InitEditor(const EToolkitMode::Type Mode,
	const TSharedPtr<IToolkitHost>& InitToolkitHost,
	UECErrorCategory* ErrorCategory
	)
{
	EditingErrorCategory = ErrorCategory;

	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout(GetDefaultTabLayoutName())
	->AddArea
	(
		FTabManager::NewPrimaryArea()
		->SetOrientation(Orient_Vertical)
		->Split
		(
			FTabManager::NewSplitter()
			->Split
			(
				FTabManager::NewStack()
				->AddTab( GetResultsTabId(), ETabState::OpenedTab )
				->SetHideTabWell(true)
			)
		)
	);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	FAssetEditorToolkit::InitAssetEditor( Mode, InitToolkitHost, GetAppIdentifierName(), StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, ErrorCategory );

	TSharedPtr<FExtender> Extender = MakeShared<FExtender>();
	Extender->AddToolBarExtension("Asset", EExtensionHook::After, GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateSP(this, &FECAssetEditor_ErrorCategory::FillToolbar));
	AddToolbarExtender(Extender);
	RegenerateMenusAndToolbars();
}

FName FECAssetEditor_ErrorCategory::GetToolkitFName() const
{
	return TEXT("EC_ErrorCategoryAssetEditor");
}
FText FECAssetEditor_ErrorCategory::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Error Category Editor");
}

FText FECAssetEditor_ErrorCategory::GetToolkitName() const
{
	if (GetEditingObjects().Num() == 1)
	{
		return FAssetEditorToolkit::GetToolkitName();
	}

	return GetBaseToolkitName();
}

FText FECAssetEditor_ErrorCategory::GetToolkitToolTipText() const
{
	if (GetEditingObjects().Num() == 1)
	{
		return FAssetEditorToolkit::GetToolkitToolTipText();
	}

	return GetBaseToolkitName();
}

FString FECAssetEditor_ErrorCategory::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "ErrorCategory ").ToString();	
}

FLinearColor FECAssetEditor_ErrorCategory::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.5f, 0.f, 0.f, 0.5f);
}

void FECAssetEditor_ErrorCategory::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu", "Error Category Editor"));

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner( GetResultsTabId(), FOnSpawnTab::CreateSP(this, &FECAssetEditor_ErrorCategory::SpawnResultsTab) )
		.SetDisplayName( LOCTEXT("DisplayName_ResultsTab", "Results") )
		.SetGroup(WorkspaceMenuCategory.ToSharedRef())
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.Enum_16x"));
}

void FECAssetEditor_ErrorCategory::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(GetResultsTabId());
}

FName FECAssetEditor_ErrorCategory::GetResultsTabId()
{
	static const FName ErrorsTabId = TEXT("ECResultsTab");
	return ErrorsTabId;
}

FName FECAssetEditor_ErrorCategory::GetAppIdentifierName()
{
	static const FName AppIdentifierName = TEXT("ECErrorCategoryAssetEditorApp");
	return AppIdentifierName;
}

FName FECAssetEditor_ErrorCategory::GetDefaultTabLayoutName()
{
	return TEXT("ECErrorCategoryTabLayout");
}

TSharedRef<SDockTab> FECAssetEditor_ErrorCategory::SpawnResultsTab(const FSpawnTabArgs& Args)
{
	check( Args.GetTabId() == GetResultsTabId() );

	UECErrorCategory* EditedErrorCategory = NULL;
	const TArray<UObject*>& EditingObjs = GetEditingObjects();
	if (EditingObjs.Num())
	{
		EditedErrorCategory = Cast<UECErrorCategory>(EditingObjs[ 0 ]);
	}

	// Create a property view
	FPropertyEditorModule& EditModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.bHideSelectionTip = true;
	DetailsViewArgs.bShowOptions = false;
	DetailsViewArgs.ColumnWidth = 0.85f;

	PropertyView = EditModule.CreateDetailView( DetailsViewArgs );

	FOnGetDetailCustomizationInstance LayoutEnumDetails = FOnGetDetailCustomizationInstance::CreateStatic(&FECCustomization_ErrorCategory::MakeInstance);
	PropertyView->RegisterInstancedCustomPropertyLayout(UECErrorCategory::StaticClass(), LayoutEnumDetails);

	PropertyView->SetObject(EditedErrorCategory);

	return SNew(SDockTab)
		.Label( LOCTEXT("TabLabel_Results", "Results") )
		.TabColorScale( GetTabColorScale() )
		[
			PropertyView.ToSharedRef()
		];
}

void FECAssetEditor_ErrorCategory::FillToolbar(FToolBarBuilder& ToolbarBuilder)
{
	const FToolBarStyle& ToolBarStyle = ToolbarBuilder.GetStyleSet()->GetWidgetStyle<FToolBarStyle>(ToolbarBuilder.GetStyleName());

	ToolbarBuilder.BeginSection(TEXT("Results"));
	// ToolbarBuilder.AddWidget(
	// 	SNew(SBox)
	// 	.HAlign(HAlign_Center)
	// 	.VAlign(VAlign_Fill)
	// 	.Padding(ToolBarStyle.ButtonPadding)
	// 	[
	// 		SNew(SPositiveActionButton)
	// 		.Text(LOCTEXT("ButtonLabel_AddError", "Add Error"))
	// 		.OnClicked(this, &FECAssetEditor_ErrorCategory::AddNewError)
	// 	]);
	ToolbarBuilder.EndSection();
}

#undef LOCTEXT_NAMESPACE
