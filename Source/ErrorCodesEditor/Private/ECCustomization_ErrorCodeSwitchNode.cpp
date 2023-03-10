﻿// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECCustomization_ErrorCodeSwitchNode.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "ECK2Node_SwitchErrorCode.h"
#include "PropertyCustomizationHelpers.h"
#include "SECWidget_ErrorCategoryList.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "ErrorCodesEditor_ErrorCodeSwitchNodeCustomization"

TSharedRef<IDetailCustomization> FECCustomization_ErrorCodeSwitchNode::MakeInstance()
{
	return MakeShareable(new FECCustomization_ErrorCodeSwitchNode());
}

void FECCustomization_ErrorCodeSwitchNode::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	const TArray<TWeakObjectPtr<UObject>>& Objects = DetailBuilder.GetSelectedObjects();
	check(Objects.Num() > 0);

	if (Objects.Num() != 1)
	{
		return;
	}

	TargetNode = CastChecked<UECK2Node_SwitchErrorCode>(Objects[0].Get());
	TSharedRef<IPropertyHandle> PinErrorCodesProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UECK2Node_SwitchErrorCode, PinErrorCodes));
	
	IDetailCategoryBuilder& PinOptionsBuilder = DetailBuilder.EditCategory(TEXT("PinOptions"));
	PinOptionsBuilder.AddProperty(PinErrorCodesProperty);
	PinOptionsBuilder.AddCustomRow(FText::GetEmpty())
		.WholeRowContent()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.Padding(0.f, 2.f)
			.AutoWidth()
			[
				SAssignNew(AddCategoryComboButton, SComboButton)
				.OnGetMenuContent(this, &FECCustomization_ErrorCodeSwitchNode::GenerateAddCategoryMenu)
				.ContentPadding(2.f)
				.ToolTipText(LOCTEXT("Tooltip_AddCategory", "Add all error codes from a category"))
				.IsEnabled(PinErrorCodesProperty->IsEditable())
				.ComboButtonStyle(FAppStyle::Get(), "BlueprintEditor.CompactVariableTypeSelector")
				.ButtonContent()
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.Padding(2.f, 0.f)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.AutoWidth()
					[
						SNew(SImage)
						.Image(FEditorStyle::GetBrush(TEXT("Icons.PlusCircle")))
						.ColorAndOpacity(FSlateColor::UseForeground())
					]
					+SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(2.f, 0.f)
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("ButtonLabel_AddCategory", "Add Category"))
						.Font(IDetailLayoutBuilder::GetDetailFont())
					]
				]
			]
		];
}

TSharedRef<SWidget> FECCustomization_ErrorCodeSwitchNode::GenerateAddCategoryMenu()
{
	return SNew(SBox)
		.WidthOverride(280.f)
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			.MaxHeight(500.f)
			[
				SNew(SECWidget_ErrorCategoryList)
				.PostErrorCategoryPicked(this, &FECCustomization_ErrorCodeSwitchNode::AddUniqueCodesFromCategory)
				.bAutoFocus(true)
			]
		];
}

void FECCustomization_ErrorCodeSwitchNode::AddUniqueCodesFromCategory(const UEnum* ErrorCategory)
{
	check(IsValid(ErrorCategory));

	const FScopedTransaction Transaction(NSLOCTEXT("ErrorCodesEditor_SwitchErrorCodeGraphNode", "AddCodesFromCategory", "Add Error Codes from Category"));
	TargetNode->Modify();

	const int32 NumNewCodes = TargetNode->AddUniqueCodesFromCategory(*ErrorCategory);
	if (NumNewCodes > 0)
	{
		FBlueprintEditorUtils::MarkBlueprintAsModified(TargetNode->GetBlueprint());
		TargetNode->GetGraph()->NotifyGraphChanged();
	}

	// Close the combo button
	AddCategoryComboButton->SetIsOpen(false);
}

#undef LOCTEXT_NAMESPACE
