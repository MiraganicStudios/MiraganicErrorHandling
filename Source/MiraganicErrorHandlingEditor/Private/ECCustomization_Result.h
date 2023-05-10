// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"

class SECWidget_Result;
struct FECResult;

class FECCustomization_Result : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
		FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& CustomizationUtils
		) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
		IDetailChildrenBuilder& ChildBuilder,
		IPropertyTypeCustomizationUtils& CustomizationUtils
		) override;

private:
	void UpdateResultProperties(FECResult NewResult);
	void SyncResultWidgetToProperty();

	TSharedPtr<IPropertyHandle> StructProperty;
	TSharedPtr<IPropertyHandle> CategoryProperty;
	TSharedPtr<IPropertyHandle> CodeProperty;

	TSharedPtr<SECWidget_Result> ResultWidget;
};
