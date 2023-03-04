// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECCustomization_ErrorCategory.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "ECEditorLogging.h"
#include "ECErrorCategory.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyCustomizationHelpers.h"

#define LOCTEXT_NAMESPACE "ErrorCodes_ErrorCategoryCustomization"

class FECErrorMapNodeBuilder : public IDetailCustomNodeBuilder, public TSharedFromThis<FECErrorMapNodeBuilder>
{
public:
	FECErrorMapNodeBuilder(IDetailLayoutBuilder& InDetailLayoutBuilder,
		const TSharedPtr<IPropertyHandle>& InPropertyHandle
	)
		: DetailLayoutBuilder(&InDetailLayoutBuilder)
		, ErrorsPropertyHandle(InPropertyHandle)
		, MapPropertyHandle(InPropertyHandle->AsMap())
		, NextId(1)
	{}

	virtual void GenerateHeaderRowContent(FDetailWidgetRow& NodeRow) override;
	virtual void GenerateChildContent(IDetailChildrenBuilder& ChildrenBuilder) override;
	virtual FName GetName() const override;
	virtual TSharedPtr<IPropertyHandle> GetPropertyHandle() const override;
	virtual void SetOnRebuildChildren(FSimpleDelegate InOnRegenerateChildren) override;

private:
	void AddErrorEntry();
	void ClearErrorEntries();
	void RemoveErrorEntryAtIndex(uint32 Index);
	
	IDetailLayoutBuilder* DetailLayoutBuilder;
	TSharedPtr<IPropertyHandle> ErrorsPropertyHandle;
	TSharedPtr<IPropertyHandleMap> MapPropertyHandle;

	FSimpleDelegate OnRebuildChildren;

	int64 NextId;
};

void FECErrorMapNodeBuilder::GenerateHeaderRowContent(FDetailWidgetRow& NodeRow)
{
	TSharedRef<SWidget> AddButton = PropertyCustomizationHelpers::MakeAddButton(FSimpleDelegate::CreateSP(this, &FECErrorMapNodeBuilder::AddErrorEntry),
		LOCTEXT("AddErrorCodeTooltip", "Add Error Code"));
	TSharedRef<SWidget> ClearButton = PropertyCustomizationHelpers::MakeEmptyButton(FSimpleDelegate::CreateSP(this, &FECErrorMapNodeBuilder::ClearErrorEntries),
		LOCTEXT("ClearErrorCodeTooltip", "Remove All Error Codes"));
	NodeRow
		.FilterString(ErrorsPropertyHandle->GetPropertyDisplayName())
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
		if (!KeyHandle.IsValid())
		{
			UE_LOG(LogErrorCodesEditor, Error, TEXT("%s: Invalid handle"), *FString(__FUNCTION__));
			continue;
		}

		int64 Id = 0;
		KeyHandle->GetValue(Id);
		NextId = FMath::Max(NextId, Id + 1);
	}
	
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
		FText IdText = FText::AsNumber(ErrorId);
		
		TSharedRef<SWidget> RemoveButton = PropertyCustomizationHelpers::MakeDeleteButton(FSimpleDelegate::CreateSP(this, &FECErrorMapNodeBuilder::RemoveErrorEntryAtIndex, ElemIdx),
			LOCTEXT("RemoveErrorCode_Tooltip", "Remove Error Code"));

		ChildrenBuilder.AddCustomRow(ElemHandle->GetDefaultCategoryText())
		.NameContent()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(50.f)
				[
					SNew(SEditableTextBox)
					.Justification(ETextJustify::Center)
					.Text(IdText)
					.IsEnabled(false)
				]
			]
			+SHorizontalBox::Slot()
			.Padding(2.f, 0.f)
			.FillWidth(1.f)
			[
				SNew(SBox)
				.MinDesiredWidth(200.f)
				[
					TitleHandle->CreatePropertyValueWidget()
				]
			]
		]
		.ValueContent()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.MaxWidth(800.f)
			.FillWidth(1.f)
			[
				SNew(SBox)
				.MinDesiredWidth(600.f)
				[
					MessageHandle->CreatePropertyValueWidget()
				]
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
	OnRebuildChildren.ExecuteIfBound();
}

void FECErrorMapNodeBuilder::RemoveErrorEntryAtIndex(uint32 Index)
{
	MapPropertyHandle->DeleteItem(Index);
	OnRebuildChildren.ExecuteIfBound();
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
