// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECCustomization_ErrorCategoryEnum.h"

#include "DetailCategoryBuilder.h"
#include "ECErrorCategoryEnum.h"
#include "DetailLayoutBuilder.h"
#include "ECErrorCategory.h"
#include "ECErrorCategoryUtils.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "STextPropertyEditableTextBox.h"
#include "Kismet2/StructureEditorUtils.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Internationalization/TextPackageNamespaceUtil.h"

#define LOCTEXT_NAMESPACE "ErrorCodesEditor_ErrorCategoryEnumDetails"

/** Allows STextPropertyEditableTextBox to edit a user defined enum entry */
class FECErrorTitleEditableText : public IEditableTextProperty
{
public:
	FECErrorTitleEditableText(UUserDefinedEnum* InTargetEnum, const int32 InEnumeratorIndex)
		: TargetEnum(InTargetEnum)
		, EnumeratorIndex(InEnumeratorIndex)
		, bCausedChange(false)
	{
	}

	virtual bool IsMultiLineText() const override
	{
		return false;
	}

	virtual bool IsPassword() const override
	{
		return false;
	}

	virtual bool IsReadOnly() const override
	{
		return false;
	}

	virtual bool IsDefaultValue() const override
	{
		return false;
	}

	virtual FText GetToolTipText() const override
	{
		return LOCTEXT("Tooltip_ErrorTitle", "Error Title");
	}

	virtual int32 GetNumTexts() const override
	{
		return 1;
	}

	virtual FText GetText(const int32 InIndex) const override
	{
		check(InIndex == 0);
		return TargetEnum->GetDisplayNameTextByIndex(EnumeratorIndex);
	}

	virtual void SetText(const int32 InIndex, const FText& InText) override
	{
		check(InIndex == 0);
		TGuardValue<bool> CausingChange(bCausedChange, true);
		FEnumEditorUtils::SetEnumeratorDisplayName(TargetEnum, EnumeratorIndex, InText);
	}

	virtual bool IsValidText(const FText& InText, FText& OutErrorMsg) const override
	{
		bool bValidName = true;

		bool bUnchangedName = (InText.ToString().Equals(TargetEnum->GetDisplayNameTextByIndex(EnumeratorIndex).ToString()));
		if (InText.IsEmpty())
		{
			OutErrorMsg = LOCTEXT("NameMissingError", "You must provide a name.");
			bValidName = false;
		}
		else if (!FEnumEditorUtils::IsEnumeratorDisplayNameValid(TargetEnum, EnumeratorIndex, InText))
		{
			OutErrorMsg = FText::Format(LOCTEXT("NameInUseError", "'{0}' is already in use."), InText);
			bValidName = false;
		}

		return bValidName && !bUnchangedName;
	}

#if USE_STABLE_LOCALIZATION_KEYS
	virtual void GetStableTextId(const int32 InIndex, const ETextPropertyEditAction InEditAction, const FString& InTextSource, const FString& InProposedNamespace, const FString& InProposedKey, FString& OutStableNamespace, FString& OutStableKey) const override
	{
		check(InIndex == 0);
		return StaticStableTextId(TargetEnum, InEditAction, InTextSource, InProposedNamespace, InProposedKey, OutStableNamespace, OutStableKey);
	}
#endif // USE_STABLE_LOCALIZATION_KEYS

	virtual void RequestRefresh() override
	{
	}

	bool CausedChange() const
	{
		return bCausedChange;
	}

private:
	/** The user defined enum being edited */
	UUserDefinedEnum* TargetEnum;

	/** Index of enumerator entry */
	int32 EnumeratorIndex;

	/** Set while we are invoking a change to the user defined enum */
	bool bCausedChange;
};

/** Allows STextPropertyEditableTextBox to edit the tooltip metadata for a user defined enum entry */
class FECErrorMessageEditableText : public IEditableTextProperty
{
public:
	FECErrorMessageEditableText(UUserDefinedEnum* InTargetEnum, const int32 InEnumeratorIndex)
		: TargetEnum(InTargetEnum)
		, EnumeratorIndex(InEnumeratorIndex)
		, bCausedChange(false)
	{
	}

	virtual bool IsMultiLineText() const override
	{
		return true;
	}

	virtual bool IsPassword() const override
	{
		return false;
	}

	virtual bool IsReadOnly() const override
	{
		return false;
	}

	virtual bool IsDefaultValue() const override
	{
		return false;
	}

	virtual FText GetToolTipText() const override
	{
		return FText::Format(
			LOCTEXT("TooltipFmt_ErrorMessage",
				"Error Message{0}Message displayed when logging this error code.{0}WARNING: Not available in shipping builds."),
			{FText::FromString(LINE_TERMINATOR)});
	}

	virtual int32 GetNumTexts() const override
	{
		return 1;
	}

	virtual FText GetText(const int32 InIndex) const override
	{
		check(InIndex == 0);
		return TargetEnum->GetToolTipTextByIndex(EnumeratorIndex);
	}

	virtual void SetText(const int32 InIndex, const FText& InText) override
	{
		check(InIndex == 0);
		TGuardValue<bool> CausingChange(bCausedChange, true);
		//@TODO: Metadata is not transactional right now, so we cannot transact a tooltip edit
		// const FScopedTransaction Transaction(NSLOCTEXT("EnumEditor", "SetEnumeratorTooltip", "Set Description"));
		TargetEnum->Modify();
		TargetEnum->SetMetaData(TEXT("ToolTip"), *InText.ToString(), EnumeratorIndex);
	}

	virtual bool IsValidText(const FText& InText, FText& OutErrorMsg) const override
	{
		return true;
	}

#if USE_STABLE_LOCALIZATION_KEYS
	virtual void GetStableTextId(const int32 InIndex, const ETextPropertyEditAction InEditAction, const FString& InTextSource, const FString& InProposedNamespace, const FString& InProposedKey, FString& OutStableNamespace, FString& OutStableKey) const override
	{
		check(InIndex == 0);
		return StaticStableTextId(TargetEnum, InEditAction, InTextSource, InProposedNamespace, InProposedKey, OutStableNamespace, OutStableKey);
	}
#endif // USE_STABLE_LOCALIZATION_KEYS

	virtual void RequestRefresh() override
	{
	}

	bool CausedChange() const
	{
		return bCausedChange;
	}

private:
	/** The user defined enum being edited */
	UUserDefinedEnum* TargetEnum;

	/** Index of enumerator entry */
	int32 EnumeratorIndex;

	/** Set while we are invoking a change to the user defined enum */
	bool bCausedChange;
};

TSharedRef<IDetailCustomization> FECCustomization_ErrorCategoryEnum::MakeInstance()
{
	return MakeShareable(new FECCustomization_ErrorCategoryEnum());
}

void FECCustomization_ErrorCategoryEnum::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	const TArray<TWeakObjectPtr<UObject>>& Objects = DetailBuilder.GetSelectedObjects();
	check(Objects.Num() > 0);

	if (Objects.Num() != 1)
	{
		return;
	}

	TargetErrorCategory = CastChecked<UECErrorCategoryEnum>(Objects[0].Get());
	TSharedRef<IPropertyHandle> NamesProperty = DetailBuilder.GetProperty(TEXT("Names"), UEnum::StaticClass());
	TSharedRef<IPropertyHandle> DescriptionProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UUserDefinedEnum, EnumDescription));

	// Make Description category appear before enums
	DetailBuilder.EditCategory(TEXT("Description"));
	DetailBuilder.AddPropertyToCategory(DescriptionProperty)
		.DisplayName(LOCTEXT("DisplayName_Description", "Description"));

	ErrorCodesBuilder = MakeShareable(new FECErrorCodesBuilder(*TargetErrorCategory));

	IDetailCategoryBuilder& InputsCategory = DetailBuilder.EditCategory(TEXT("ErrorCodes"), LOCTEXT("CategoryLabel_ErrorCodes", "Error Codes"));
	InputsCategory.AddCustomBuilder(ErrorCodesBuilder.ToSharedRef());
}

void FECCustomization_ErrorCategoryEnum::RequestRefresh()
{
	ErrorCodesBuilder->RequestRefresh();
}

void FECCustomization_ErrorCategoryEnum::PreChange(const UUserDefinedEnum* Enum,
	FEnumEditorUtils::EEnumEditorChangeInfo Info
	)
{
	// Nothing to do
}

void FECCustomization_ErrorCategoryEnum::PostChange(const UUserDefinedEnum* Enum,
	FEnumEditorUtils::EEnumEditorChangeInfo Info
	)
{
	if (Enum && Enum == TargetErrorCategory.Get())
	{
		RequestRefresh();
	}
}

void FECCustomization_ErrorCategoryEnum::PostUndo(bool bSuccess)
{
	RequestRefresh();
}

void FECCustomization_ErrorCategoryEnum::PostRedo(bool bSuccess)
{
	RequestRefresh();
}

void FECErrorCodesBuilder::RequestRefresh()
{
	RefreshNextId();
	RequestRebuild.ExecuteIfBound();
}

void FECErrorCodesBuilder::GenerateHeaderRowContent(FDetailWidgetRow& NodeRow)
{
	TSharedRef<SWidget> AddButton = PropertyCustomizationHelpers::MakeAddButton(FSimpleDelegate::CreateSP(this, &FECErrorCodesBuilder::AddEntry),
		LOCTEXT("AddErrorCodeTooltip", "Add Error Code"));
	
	NodeRow
		.FilterString(LOCTEXT("FilterString_ErrorCodes", "Error Codes"))
		.OverrideResetToDefault(FResetToDefaultOverride::Hide())
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Label_HeaderRow", "Error Codes"))
				.Font(IDetailLayoutBuilder::GetDetailFont())
			]
			+SHorizontalBox::Slot()
			.Padding(2.f)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.AutoWidth()
			[
				AddButton
			]
		];
}

void FECErrorCodesBuilder::GenerateChildContent(IDetailChildrenBuilder& ChildrenBuilder)
{
	const int32 NumValues = FMath::Max(0, TargetErrorCategory->NumEnums() - 1);
	for (int32 EnumIdx = 0; EnumIdx < NumValues; ++EnumIdx)
	{
		TSharedRef<FECErrorTitleEditableText> ErrorTitleEditText = MakeShareable(new FECErrorTitleEditableText(TargetErrorCategory.Get(), EnumIdx));
		TSharedRef<FECErrorMessageEditableText> ErrorMessageEditText = MakeShareable(new FECErrorMessageEditableText(TargetErrorCategory.Get(), EnumIdx));
		
		const bool bIsEditable = true;
		TSharedRef<SWidget> RemoveButton = PropertyCustomizationHelpers::MakeDeleteButton(FSimpleDelegate::CreateSP(this, &FECErrorCodesBuilder::RemoveEntryAtIndex, EnumIdx),
			LOCTEXT("Tooltip_RemoveErrorCode", "Remove Error Code"));
		RemoveButton->SetEnabled(bIsEditable);

		const FText TitleText = TargetErrorCategory->GetDisplayNameTextByIndex(EnumIdx);
		const FText MessageText = TargetErrorCategory->GetToolTipTextByIndex(EnumIdx);
		int64 EnumValue = TargetErrorCategory->GetValueByIndex(EnumIdx);
		NextId = FMath::Max(NextId, EnumValue + 1);
		FText IdText = FText::AsNumber(EnumValue);
		FText IdTooltip = LOCTEXT("Tooltip_ErrorId", "This error's unique id. This is internal data which you don't need to change.");

		FText SearchString = FText::Format(LOCTEXT("SearchStringFmt", "{0}: {1}"), {TitleText, MessageText});

		const FEditableTextBoxStyle& TextBoxStyle = FCoreStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("NormalEditableTextBox");
		FSlateFontInfo DetailFont = IDetailLayoutBuilder::GetDetailFont();
		
		ChildrenBuilder.AddCustomRow(SearchString)
			.NameContent()
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.Padding(2.f, 2.f)
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(50.f)
					[
						SNew(SBorder)
						.BorderImage(&TextBoxStyle.BackgroundImageReadOnly)
						.BorderBackgroundColor(TextBoxStyle.BackgroundColor)
						.ForegroundColor(TextBoxStyle.ReadOnlyForegroundColor)
						[
							SNew(SBox)
							.VAlign(VAlign_Center)
							[
								SNew(STextBlock)
								.Justification(ETextJustify::Center)
								.Text(IdText)
								.IsEnabled(false)
								.Font(TextBoxStyle.Font)
								.ToolTipText(IdTooltip)
							]
						]
					]
				]
				+SHorizontalBox::Slot()
				.Padding(2.f, 0.f)
				.FillWidth(1.f)
				[
					SNew(STextPropertyEditableTextBox, ErrorTitleEditText)
					.MinDesiredWidth(200.f)
					.Font(DetailFont)
				]
			]
			.ValueContent()
			.HAlign(HAlign_Fill)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.MaxWidth(800.f)
				.FillWidth(1.f)
				[
					SNew(STextPropertyEditableTextBox, ErrorMessageEditText)
					.Font(DetailFont)
				]
				+SHorizontalBox::Slot()
				.Padding(2.f, 0.f, 2.f, 0.f)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.AutoWidth()
				[
					RemoveButton
				]
			];
	}
}

void FECErrorCodesBuilder::AddEntry()
{
	if (!TargetErrorCategory.IsValid())
	{
		return;
	}
	
	const FScopedTransaction Transaction(LOCTEXT("Transaction_AddEntry", "Add Error Code"));

	BroadcastPreChange();
	
	TArray<TPair<FName, int64>> OldNames;
	CopyEnumsWithoutMax(OldNames, *TargetErrorCategory);
	TArray<TPair<FName, int64>> Names = OldNames;

	FString EnumNameStr = TargetErrorCategory->GenerateNewErrorCodeName();
	const FString FullNameStr = TargetErrorCategory->GenerateFullEnumName(*EnumNameStr);
	Names.Emplace(*FullNameStr, NextId++);

	// Note that we do NOT clean up enum values as it will invalidate error code references

	const UEnum::ECppForm EnumType = TargetErrorCategory->GetCppForm();
	TargetErrorCategory->SetEnums(Names, EnumType);
	TargetErrorCategory->EnsureAllDisplayNamesExist();

	ErrorCodes::BroadcastChanges(*TargetErrorCategory, OldNames);

	TargetErrorCategory->MarkPackageDirty();
}

void FECErrorCodesBuilder::RefreshNextId()
{
	NextId = 1;
	const int32 NumEnums = TargetErrorCategory->NumEnums() - 1;
	for (int32 Idx = 0; Idx < NumEnums; ++Idx)
	{
		NextId = FMath::Max(NextId, TargetErrorCategory->GetValueByIndex(Idx) + 1);
	}
}

void FECErrorCodesBuilder::RemoveEntryAtIndex(int32 Index)
{
	if (!TargetErrorCategory.IsValid() || TargetErrorCategory->GetNameByIndex(Index).IsNone())
	{
		return;
	}

	const FScopedTransaction Transaction(LOCTEXT("RemoveErrorCode", "Remove Error Code"));

	BroadcastPreChange();

	TArray<TPair<FName, int64>> OldNames;
	CopyEnumsWithoutMax(OldNames, *TargetErrorCategory);
	TArray<TPair<FName, int64>> Names = OldNames;

	Names.RemoveAt(Index);
	
	// Note that we do NOT clean up enum values as it will invalidate error code references

	const UEnum::ECppForm EnumType = TargetErrorCategory->GetCppForm();
	TargetErrorCategory->SetEnums(Names, EnumType);
	TargetErrorCategory->EnsureAllDisplayNamesExist();
	ErrorCodes::BroadcastChanges(*TargetErrorCategory, OldNames);

	TargetErrorCategory->MarkPackageDirty();
}

void FECErrorCodesBuilder::CopyEnumsWithoutMax(TArray<TPair<FName, int64>>& OutEnumPairs, const UEnum& Enum)
{
	const int32 NumEnums = Enum.NumEnums() - 1;
	for (int32 Idx = 0; Idx < NumEnums; ++Idx)
	{
		OutEnumPairs.Emplace(Enum.GetNameByIndex(Idx), Enum.GetValueByIndex(Idx));
	}
}

void FECErrorCodesBuilder::BroadcastPreChange()
{
	FEnumEditorManager::Get().PreChange(TargetErrorCategory.Get(), FEnumEditorUtils::Changed);
	TargetErrorCategory->Modify();
}

#undef LOCTEXT_NAMESPACE
