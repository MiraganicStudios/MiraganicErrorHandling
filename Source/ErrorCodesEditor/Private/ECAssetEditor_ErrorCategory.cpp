// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECAssetEditor_ErrorCategory.h"

#include "ECCustomization_ErrorCategoryEnum.h"
#include "ECErrorCategoryEnum.h"
#include "Styling/ToolBarStyle.h"
#include "SPositiveActionButton.h"
#include "Kismet2/EnumEditorUtils.h"

#define LOCTEXT_NAMESPACE "ErrorCodesEditor_ErrorCategoryAssetEditor"

FECAssetEditor_ErrorCategory::~FECAssetEditor_ErrorCategory()
{
}

void FECAssetEditor_ErrorCategory::InitEditor(const EToolkitMode::Type Mode,
	const TSharedPtr<IToolkitHost>& InitToolkitHost,
	UECErrorCategoryEnum* ErrorCategory
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
				->AddTab( GetErrorCodesTabId(), ETabState::OpenedTab )
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

	InTabManager->RegisterTabSpawner( GetErrorCodesTabId(), FOnSpawnTab::CreateSP(this, &FECAssetEditor_ErrorCategory::SpawnErrorCodesTab) )
		.SetDisplayName( LOCTEXT("DisplayName_ErrorCodesTab", "Error Codes") )
		.SetGroup(WorkspaceMenuCategory.ToSharedRef())
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "GraphEditor.Enum_16x"));
}

void FECAssetEditor_ErrorCategory::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(GetErrorCodesTabId());
}

FName FECAssetEditor_ErrorCategory::GetErrorCodesTabId()
{
	static const FName ErrorsTabId = TEXT("ECErrorCodesTab");
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

TSharedRef<SDockTab> FECAssetEditor_ErrorCategory::SpawnErrorCodesTab(const FSpawnTabArgs& Args)
{
	check( Args.GetTabId() == GetErrorCodesTabId() );

	UECErrorCategoryEnum* EditedErrorCategory = NULL;
	const TArray<UObject*>& EditingObjs = GetEditingObjects();
	if (EditingObjs.Num())
	{
		EditedErrorCategory = Cast<UECErrorCategoryEnum>(EditingObjs[ 0 ]);
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

	FOnGetDetailCustomizationInstance LayoutEnumDetails = FOnGetDetailCustomizationInstance::CreateStatic(&FECCustomization_ErrorCategoryEnum::MakeInstance);
	PropertyView->RegisterInstancedCustomPropertyLayout(UECErrorCategoryEnum::StaticClass(), LayoutEnumDetails);

	PropertyView->SetObject(EditedErrorCategory);

	return SNew(SDockTab)
		.Label( LOCTEXT("TabLabel_ErrorCodes", "Error Codes") )
		.TabColorScale( GetTabColorScale() )
		[
			PropertyView.ToSharedRef()
		];
}

void FECAssetEditor_ErrorCategory::FillToolbar(FToolBarBuilder& ToolbarBuilder)
{
	const FToolBarStyle& ToolBarStyle = ToolbarBuilder.GetStyleSet()->GetWidgetStyle<FToolBarStyle>(ToolbarBuilder.GetStyleName());

	ToolbarBuilder.BeginSection(TEXT("ErrorCodes"));
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
