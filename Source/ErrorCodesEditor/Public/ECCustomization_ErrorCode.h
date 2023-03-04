// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"

class SECErrorCodeWidget;
struct FMGErrorCode;

class FECCustomization_ErrorCode : public IPropertyTypeCustomization
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
	void UpdateErrorCodeProperties(FMGErrorCode NewErrorCode);
	void SyncErrorCodeWidgetToProperty();

	TSharedPtr<IPropertyHandle> StructProperty;
	TSharedPtr<IPropertyHandle> CategoryProperty;
	TSharedPtr<IPropertyHandle> CodeProperty;

	TSharedPtr<SECErrorCodeWidget> ErrorCodeWidget;
};
