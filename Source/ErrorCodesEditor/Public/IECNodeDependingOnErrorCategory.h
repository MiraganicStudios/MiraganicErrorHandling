// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IECNodeDependingOnErrorCategory.generated.h"

class UECErrorCategory;

UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class ERRORCODESEDITOR_API UECNodeDependingOnErrorCategory : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implementing K2Nodes depend on ErrorCategory(s). Modeled after INodeDependingOnEnum.
 */
class ERRORCODESEDITOR_API IECNodeDependingOnErrorCategory
{
	GENERATED_BODY()

public:
	virtual bool DependsOnErrorCategory(const UECErrorCategory& Category) const = 0;
	
	virtual void ReloadErrorCategory(UECErrorCategory* Category) = 0;

	virtual bool ShouldBeReconstructedAfterChanges() const { return false; }
};
