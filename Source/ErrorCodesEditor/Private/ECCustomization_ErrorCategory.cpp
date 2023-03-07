// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECCustomization_ErrorCategory.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "ECEditorLogging.h"
#include "ECErrorCategory.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyUtilities.h"
#include "PropertyCustomizationHelpers.h"
#include "STextPropertyEditableTextBox.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "HAL/PlatformApplicationMisc.h"

#define LOCTEXT_NAMESPACE "ErrorCodes_ErrorCategoryCustomization"

namespace
{
	/**
	 * Allows STextPropertyEditableTextBox to edit a property handle
	 * Copied from TextCustomization.cpp, but changed 'RequestRefresh' to 'RefreshDelegate'.
	 */
	class FEditableTextPropertyHandle : public IEditableTextProperty
	{
	public:
		FEditableTextPropertyHandle(const TSharedRef<IPropertyHandle>& InPropertyHandle, FSimpleDelegate InRefreshDelegate)
			: PropertyHandle(InPropertyHandle)
			, RefreshDelegate(InRefreshDelegate)
		{
		}

		virtual bool IsMultiLineText() const override
		{
			return PropertyHandle->IsValidHandle() && PropertyHandle->GetMetaDataProperty()->GetBoolMetaData("MultiLine");
		}

		virtual bool IsPassword() const override
		{
			return PropertyHandle->IsValidHandle() && PropertyHandle->GetMetaDataProperty()->GetBoolMetaData("PasswordField");
		}

		virtual bool IsReadOnly() const override
		{
			return !PropertyHandle->IsValidHandle() || PropertyHandle->IsEditConst();
		}

		virtual bool IsDefaultValue() const override
		{
			return PropertyHandle->IsValidHandle() && !PropertyHandle->DiffersFromDefault();
		}

		virtual FText GetToolTipText() const override
		{
			return (PropertyHandle->IsValidHandle())
				? PropertyHandle->GetToolTipText()
				: FText::GetEmpty();
		}

		virtual int32 GetNumTexts() const override
		{
			return (PropertyHandle->IsValidHandle())
				? PropertyHandle->GetNumPerObjectValues() 
				: 0;
		}

		virtual FText GetText(const int32 InIndex) const override
		{
			if (PropertyHandle->IsValidHandle())
			{
				FString ObjectValue;
				if (PropertyHandle->GetPerObjectValue(InIndex, ObjectValue) == FPropertyAccess::Success)
				{
					FText TextValue;
					if (FTextStringHelper::ReadFromBuffer(*ObjectValue, TextValue))
					{
						return TextValue;
					}
				}
			}

			return FText::GetEmpty();
		}

		virtual void SetText(const int32 InIndex, const FText& InText) override
		{
			if (PropertyHandle->IsValidHandle())
			{
				FString ObjectValue;
				FTextStringHelper::WriteToBuffer(ObjectValue, InText);
				PropertyHandle->SetPerObjectValue(InIndex, ObjectValue);
			}
		}

		virtual bool IsValidText(const FText& InText, FText& OutErrorMsg) const override
		{
			return true;
		}

#if USE_STABLE_LOCALIZATION_KEYS
		virtual void GetStableTextId(const int32 InIndex, const ETextPropertyEditAction InEditAction, const FString& InTextSource, const FString& InProposedNamespace, const FString& InProposedKey, FString& OutStableNamespace, FString& OutStableKey) const override
		{
			if (PropertyHandle->IsValidHandle())
			{
				TArray<UPackage*> PropertyPackages;
				PropertyHandle->GetOuterPackages(PropertyPackages);

				check(PropertyPackages.IsValidIndex(InIndex));

				StaticStableTextId(PropertyPackages[InIndex], InEditAction, InTextSource, InProposedNamespace, InProposedKey, OutStableNamespace, OutStableKey);
			}
		}
#endif // USE_STABLE_LOCALIZATION_KEYS

		virtual void RequestRefresh() override
		{
			RefreshDelegate.ExecuteIfBound();
		}

	private:
		TSharedRef<IPropertyHandle> PropertyHandle;
		FSimpleDelegate RefreshDelegate;
	};
}

class FECErrorMapNodeBuilder : public IDetailCustomNodeBuilder, public TSharedFromThis<FECErrorMapNodeBuilder>, public FEditorUndoClient
{
public:
	FECErrorMapNodeBuilder(IDetailLayoutBuilder& InDetailLayoutBuilder,
		const TSharedPtr<IPropertyHandle>& InPropertyHandle
	)
		: DetailLayoutBuilder(&InDetailLayoutBuilder)
		, ErrorsPropertyHandle(InPropertyHandle)
		, MapPropertyHandle(InPropertyHandle->AsMap())
		, PropertyUtilities(InDetailLayoutBuilder.GetPropertyUtilities())
		, NextId(1LL)
		, bRefreshPending(false)
	{
		if (GEditor)
		{
			GEditor->RegisterForUndo(this);
		}
	}

	virtual void GenerateHeaderRowContent(FDetailWidgetRow& NodeRow) override;
	virtual void GenerateChildContent(IDetailChildrenBuilder& ChildrenBuilder) override;
	virtual bool RequiresTick() const override { return true; }
	virtual void Tick(float DeltaTime) override;
	virtual FName GetName() const override;
	virtual TSharedPtr<IPropertyHandle> GetPropertyHandle() const override;
	virtual void SetOnRebuildChildren(FSimpleDelegate InOnRegenerateChildren) override;

	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;

private:
	void AddErrorEntry();
	void ClearErrorEntries();
	void RemoveErrorEntryAtIndex(uint32 Index);

	void CopyEntryAtIndex(uint32 Index);
	void PasteToEntryAtIndex(uint32 Index);

	bool IsMapDifferentFromDefault(TSharedPtr<IPropertyHandle>) const;
	void ResetMapToDefault(TSharedPtr<IPropertyHandle>);
	
	IDetailLayoutBuilder* DetailLayoutBuilder;
	TSharedPtr<IPropertyHandle> ErrorsPropertyHandle;
	TSharedPtr<IPropertyHandleMap> MapPropertyHandle;
	TWeakPtr<IPropertyUtilities> PropertyUtilities;

	FSimpleDelegate OnRebuildChildren;

	int64 NextId;
	bool bRefreshPending;
};

void FECErrorMapNodeBuilder::GenerateHeaderRowContent(FDetailWidgetRow& NodeRow)
{
	TSharedRef<SWidget> AddButton = PropertyCustomizationHelpers::MakeAddButton(FSimpleDelegate::CreateSP(this, &FECErrorMapNodeBuilder::AddErrorEntry),
		LOCTEXT("AddErrorCodeTooltip", "Add Error Code"));
	TSharedRef<SWidget> ClearButton = PropertyCustomizationHelpers::MakeEmptyButton(FSimpleDelegate::CreateSP(this, &FECErrorMapNodeBuilder::ClearErrorEntries),
		LOCTEXT("ClearErrorCodeTooltip", "Remove All Error Codes"));

	FResetToDefaultOverride ResetToDefaultOverride = FResetToDefaultOverride::Create(
		FIsResetToDefaultVisible::CreateSP(this, &FECErrorMapNodeBuilder::IsMapDifferentFromDefault),
		FResetToDefaultHandler::CreateSP(this, &FECErrorMapNodeBuilder::ResetMapToDefault));
	
	NodeRow
		.FilterString(ErrorsPropertyHandle->GetPropertyDisplayName())
		.OverrideResetToDefault(ResetToDefaultOverride)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				ErrorsPropertyHandle->CreatePropertyNameWidget()
			]
			+SHorizontalBox::Slot()
			.Padding(2.f)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.AutoWidth()
			[
				AddButton
			]
			+SHorizontalBox::Slot()
			.Padding(2.f)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.AutoWidth()
			[
				ClearButton
			]
		];
}

void FECErrorMapNodeBuilder::GenerateChildContent(IDetailChildrenBuilder& ChildrenBuilder)
{
	// Map property handle will be invalidated if emptied, etc.
	MapPropertyHandle = ErrorsPropertyHandle->AsMap();
	
	uint32 NumElems = 0;
	MapPropertyHandle->GetNumElements(NumElems);
	
	for (uint32 ElemIdx = 0; ElemIdx < NumElems; ++ElemIdx)
	{
		TSharedPtr<IPropertyHandle> ElemHandle = ErrorsPropertyHandle->GetChildHandle(ElemIdx);
		if (!ElemHandle.IsValid())
		{
			UE_LOG(LogErrorCodesEditor, Error, TEXT("%s: Invalid handle"), *FString(__FUNCTION__));
			continue;
		}

		TSharedPtr<IPropertyHandle> KeyHandle = ElemHandle->GetKeyHandle();
		TSharedPtr<IPropertyHandle> TitleHandle = ElemHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FECErrorCodeData, Title));
		TSharedPtr<IPropertyHandle> MessageHandle = ElemHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FECErrorCodeData, Message));

		if (!KeyHandle.IsValid() || !TitleHandle.IsValid() || !MessageHandle.IsValid())
		{
			UE_LOG(LogErrorCodesEditor, Error, TEXT("%s: Invalid title or message handle(s)"), *FString(__FUNCTION__));
			continue;
		}

		int64 ErrorId;
		KeyHandle->GetValue(ErrorId);
		// Set next id to the maximum + 1 among existing ids
		NextId = FMath::Max(NextId, ErrorId + 1);
		FText IdText = FText::AsNumber(ErrorId);
		FText IdTooltip = LOCTEXT("ErrorIdTooltip", "This error's unique id. This is internal data which you don't need to change.");
		
		TSharedRef<SWidget> RemoveButton = PropertyCustomizationHelpers::MakeDeleteButton(FSimpleDelegate::CreateSP(this, &FECErrorMapNodeBuilder::RemoveErrorEntryAtIndex, ElemIdx),
			LOCTEXT("RemoveErrorCode_Tooltip", "Remove Error Code"));

		TSharedRef<IEditableTextProperty> TitleTextProperty = MakeShareable(new FEditableTextPropertyHandle(TitleHandle.ToSharedRef(), OnRebuildChildren));
		TSharedRef<IEditableTextProperty> MessageTextProperty = MakeShareable(new FEditableTextPropertyHandle(MessageHandle.ToSharedRef(), OnRebuildChildren));

		FText TitleText;
		TitleHandle->GetValue(TitleText);
		FText MessageText;
		MessageHandle->GetValue(MessageText);
		FText SearchString = FText::Format(LOCTEXT("SearchStringFmt", "{0}: {1}"), {TitleText, MessageText});

		FUIAction CopyAction(FExecuteAction::CreateSP(this, &FECErrorMapNodeBuilder::CopyEntryAtIndex, ElemIdx));
		FUIAction PasteAction(FExecuteAction::CreateSP(this, &FECErrorMapNodeBuilder::PasteToEntryAtIndex, ElemIdx));

		const FEditableTextBoxStyle& TextBoxStyle = FCoreStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("NormalEditableTextBox");
		
		ChildrenBuilder.AddCustomRow(SearchString)
		.CopyAction(CopyAction)
		.PasteAction(PasteAction)
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
				SNew(STextPropertyEditableTextBox, TitleTextProperty)
				.MinDesiredWidth(200.f)
				.MaxDesiredHeight(600.f)
				.Font(FEditorStyle::GetFontStyle("PropertyWindow.NormalFont"))
				.AutoWrapText(true)
			]
		]
		.ValueContent()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.MaxWidth(800.f)
			.FillWidth(1.f)
			[
				SNew(STextPropertyEditableTextBox, MessageTextProperty)
				.MinDesiredWidth(600.f)
				.MaxDesiredHeight(1000.f)
				.Font(FEditorStyle::GetFontStyle("PropertyWindow.NormalFont"))
				.AutoWrapText(true)
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

void FECErrorMapNodeBuilder::Tick(float DeltaTime)
{
	if (bRefreshPending && PropertyUtilities.IsValid())
	{
		PropertyUtilities.Pin()->ForceRefresh();
		bRefreshPending = false;
	}
}

FName FECErrorMapNodeBuilder::GetName() const
{
	return TEXT("Errors");
}

TSharedPtr<IPropertyHandle> FECErrorMapNodeBuilder::GetPropertyHandle() const
{
	return ErrorsPropertyHandle;
}

void FECErrorMapNodeBuilder::SetOnRebuildChildren(FSimpleDelegate InOnRegenerateChildren)
{
	OnRebuildChildren = InOnRegenerateChildren;
}

void FECErrorMapNodeBuilder::PostUndo(bool bSuccess)
{
	// Refresh on next tick; refreshing here causes infinite recursion
	bRefreshPending = true;
}

void FECErrorMapNodeBuilder::PostRedo(bool bSuccess)
{
	// Refresh on next tick; refreshing here causes infinite recursion
	bRefreshPending = true;
}

void FECErrorMapNodeBuilder::AddErrorEntry()
{
	TArray<UObject*> OuterObjects;
	ErrorsPropertyHandle->GetOuterObjects(OuterObjects);

	if (OuterObjects.Num() == 1)
	{
		const FScopedTransaction Transaction(LOCTEXT("AddErrorCode_Transaction", "Add Error Code"));

		uint32 LastIdx;
		MapPropertyHandle->GetNumElements(LastIdx);
		if (MapPropertyHandle->AddItem() == FPropertyAccess::Success)
		{
			TSharedPtr<IPropertyHandle> Handle = ErrorsPropertyHandle->GetChildHandle(LastIdx);
			if (Handle.IsValid())
			{
				TSharedPtr<IPropertyHandle> KeyHandle = Handle->GetKeyHandle();
				KeyHandle->SetValue(NextId);
				++NextId;
				// We assume we'll never reach the maximum
				check(NextId > 0);
				
				OnRebuildChildren.ExecuteIfBound();
			}
			else
			{
				UE_LOG(LogErrorCodesEditor, Error, TEXT("%s: Invalid handle"), *FString(__FUNCTION__));
			}
		}
	}
}

void FECErrorMapNodeBuilder::ClearErrorEntries()
{
	MapPropertyHandle->Empty();
	if (PropertyUtilities.IsValid())
	{
		PropertyUtilities.Pin()->ForceRefresh();
	}
}

void FECErrorMapNodeBuilder::RemoveErrorEntryAtIndex(uint32 Index)
{
	MapPropertyHandle->DeleteItem(Index);
	OnRebuildChildren.ExecuteIfBound();
}

void FECErrorMapNodeBuilder::CopyEntryAtIndex(uint32 Index)
{
	uint32 NumElems;
	MapPropertyHandle->GetNumElements(NumElems);
	if (Index >= NumElems)
	{
		return;
	}

	TSharedPtr<IPropertyHandle> EntryPropertyHandle = ErrorsPropertyHandle->GetChildHandle(Index);
	FECErrorCodeData DefaultData;
	void* EntryDataPtr = nullptr;
	EntryPropertyHandle->GetValueData(EntryDataPtr);
	check(EntryDataPtr != nullptr);
	FString EntryDataStr;
	FECErrorCodeData::StaticStruct()->ExportText(EntryDataStr, EntryDataPtr, &DefaultData, nullptr, PPF_Copy, nullptr);
	FPlatformApplicationMisc::ClipboardCopy(*EntryDataStr);
}

void FECErrorMapNodeBuilder::PasteToEntryAtIndex(uint32 Index)
{
	uint32 NumElems;
	MapPropertyHandle->GetNumElements(NumElems);
	if (Index >= NumElems)
	{
		return;
	}

	TSharedPtr<IPropertyHandle> EntryPropertyHandle = ErrorsPropertyHandle->GetChildHandle(Index);
	void* EntryDataPtr = nullptr;
	EntryPropertyHandle->GetValueData(EntryDataPtr);
	check(EntryDataPtr != nullptr);
	FString ClipboardStr;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardStr);
	
	EntryPropertyHandle->SetValueFromFormattedString(ClipboardStr);
}

bool FECErrorMapNodeBuilder::IsMapDifferentFromDefault(TSharedPtr<IPropertyHandle> PropertyHandle) const
{
	return ErrorsPropertyHandle->DiffersFromDefault();
}

void FECErrorMapNodeBuilder::ResetMapToDefault(TSharedPtr<IPropertyHandle> PropertyHandle)
{
	ErrorsPropertyHandle->ResetToDefault();
	if (PropertyUtilities.IsValid())
	{
		PropertyUtilities.Pin()->ForceRefresh();
	}
}

TSharedRef<IDetailCustomization> FECCustomization_ErrorCategory::MakeInstance()
{
	return MakeShareable(new FECCustomization_ErrorCategory());
}

void FECCustomization_ErrorCategory::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	ErrorsPropertyHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UECErrorCategory, Errors),
		UECErrorCategory::StaticClass());
	ErrorsPropertyHandle->MarkHiddenByCustomization();
	
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("Errors");
	const FSlateFontInfo DetailFontInfo = IDetailLayoutBuilder::GetDetailFont();
	
	// Custom builder widget for errors
	const TSharedRef<FECErrorMapNodeBuilder> ErrorMapNodeBuilder = MakeShareable(new FECErrorMapNodeBuilder(DetailBuilder, ErrorsPropertyHandle));
	Category.AddCustomBuilder(ErrorMapNodeBuilder);
}

#undef LOCTEXT_NAMESPACE
