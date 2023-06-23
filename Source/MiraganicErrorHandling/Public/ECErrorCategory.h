// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "ECErrorCategory.generated.h"

class UECErrorCategory;

using FECEnumNameValuePair = TPair<FName, int64>;
DECLARE_MULTICAST_DELEGATE_ThreeParams(FECErrorCategoryChanged, const UECErrorCategory&, const TArray<FECEnumNameValuePair>&, bool);

/**
 * A collection of error codes defined in the editor.
 */
UCLASS()
class MIRAGANICERRORHANDLING_API UECErrorCategory : public UUserDefinedEnum
{
	GENERATED_BODY()

public:
	virtual void Serialize(FArchive& Ar) override;

#if WITH_EDITOR
	virtual bool Rename(const TCHAR* NewName, UObject* NewOuter, ERenameFlags Flags) override;
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	virtual void PostLoad() override;
	virtual void PostEditUndo() override;

	void UpdateAfterPathChanged();
	void UpgradeDisplayNamesFromMetaData();
	void EnsureAllDisplayNamesExist();

	FString GenerateNewErrorCodeName();
	bool IsProperErrorCodeName(const FString& NewName);

	bool IsDisplayNameValidAndUnique(int32 EnumeratorIndex, const FText& NewDisplayName) const;
#endif

#if WITH_EDITORONLY_DATA
	static FECErrorCategoryChanged& PostChangedInEditor();
#endif
};
