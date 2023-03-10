// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "ECErrorCategoryEnum.generated.h"

class UECErrorCategoryEnum;

using FECErrorCodePair = TPair<FName, int64>;
DECLARE_MULTICAST_DELEGATE_ThreeParams(FECErrorCategoryChanged, const UECErrorCategoryEnum&, const TArray<FECErrorCodePair>&, bool);

/**
 * A collection of error codes defined in the editor.
 */
UCLASS()
class ERRORCODES_API UECErrorCategoryEnum : public UUserDefinedEnum
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
#endif

#if WITH_EDITORONLY_DATA
	static FECErrorCategoryChanged& PostChangedInEditor();
#endif
};
