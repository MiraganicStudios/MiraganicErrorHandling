// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "IDetailCustomNodeBuilder.h"
#include "Kismet2/EnumEditorUtils.h"

class UECErrorCategoryEnum;
/**
 * 
 */
class FECCustomization_ErrorCategoryEnum : public IDetailCustomization, FEnumEditorUtils::INotifyOnEnumChanged, FEditorUndoClient
{
public:
	static TSharedRef<class IDetailCustomization> MakeInstance();
	
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	void RequestRefresh();
	
	/** FEnumEditorUtils::INotifyOnEnumChanged */
	virtual void PreChange(const class UUserDefinedEnum* Enum, FEnumEditorUtils::EEnumEditorChangeInfo Info) override;
	virtual void PostChange(const class UUserDefinedEnum* Enum, FEnumEditorUtils::EEnumEditorChangeInfo Info) override;

	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;

	TSharedPtr<class FECErrorCodesBuilder> ErrorCodesBuilder;
	TWeakObjectPtr<UECErrorCategoryEnum> TargetErrorCategory;
};

class FECErrorCodesBuilder : public IDetailCustomNodeBuilder, public TSharedFromThis<FECErrorCodesBuilder>
{
public:
	FECErrorCodesBuilder(UECErrorCategoryEnum& InErrorCategory)
		: TargetErrorCategory(&InErrorCategory)
		, NextId(0)
	{}

	void RequestRefresh();

	virtual void SetOnRebuildChildren( FSimpleDelegate InOnRegenerateChildren ) override { RequestRebuild = InOnRegenerateChildren; }
	virtual void GenerateHeaderRowContent( FDetailWidgetRow& NodeRow ) override;
	virtual void GenerateChildContent( IDetailChildrenBuilder& ChildrenBuilder ) override;
	virtual bool RequiresTick() const override { return false; }
	virtual FName GetName() const override { return TEXT("ErrorCodes"); }
	virtual bool InitiallyCollapsed() const override { return false; }

	void AddEntry();
	
private:
	void RefreshNextId();
	
	void RemoveEntryAtIndex(int32 Index);

	static void CopyEnumsWithoutMax(TArray<TPair<FName, int64>>& OutEnumPairs, const UEnum& Enum);
	
	FSimpleDelegate RequestRebuild;
	TWeakObjectPtr<UECErrorCategoryEnum> TargetErrorCategory;

	int64 NextId;

	using FEnumEditorManager = FEnumEditorUtils::FEnumEditorManager;
	using EEEnumEditorChangeInfo = FEnumEditorUtils::EEnumEditorChangeInfo;
};
