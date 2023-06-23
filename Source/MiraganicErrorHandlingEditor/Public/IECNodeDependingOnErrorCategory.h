// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IECNodeDependingOnErrorCategory.generated.h"

class UECErrorCategory;

UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class MIRAGANICERRORHANDLINGEDITOR_API UECNodeDependingOnErrorCategory : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implementing K2Nodes depend on ErrorCategory(s). Modeled after INodeDependingOnEnum.
 */
class MIRAGANICERRORHANDLINGEDITOR_API IECNodeDependingOnErrorCategory
{
	GENERATED_BODY()

public:
	virtual bool DependsOnErrorCategory(const UECErrorCategory& Category) const = 0;
	
	virtual void ReloadErrorCategory(UECErrorCategory* Category) = 0;

	virtual bool ShouldBeReconstructedAfterChanges() const { return false; }
};
