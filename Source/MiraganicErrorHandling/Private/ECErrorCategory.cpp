// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.


#include "ECErrorCategory.h"

#include "ECLogging.h"
#include "Internationalization/TextPackageNamespaceUtil.h"

void UECErrorCategory::Serialize(FArchive& Ar)
{
	UEnum::Serialize(Ar);

	// UserDefinedEnum override modifies the values
}

#if WITH_EDITOR

bool UECErrorCategory::Rename(const TCHAR* NewName, UObject* NewOuter, ERenameFlags Flags)
{
	bool bSucceeded = UEnum::Rename(NewName, NewOuter, Flags);
	if (bSucceeded)
	{
		UpdateAfterPathChanged();
	}

	return bSucceeded;
}

void UECErrorCategory::PostDuplicate(bool bDuplicateForPIE)
{
	UEnum::PostDuplicate(bDuplicateForPIE);

	if (!bDuplicateForPIE)
	{
		UpdateAfterPathChanged();
	}
}

void UECErrorCategory::PostLoad()
{
	UEnum::PostLoad();

	UpdateAfterPathChanged();
	if (NumEnums() > 1 && DisplayNameMap.Num() == 0)
	{
		UpgradeDisplayNamesFromMetaData();
	}
	EnsureAllDisplayNamesExist();

	// Apply the transactional flag to error categories that were not created with it
	SetFlags(RF_Transactional);
}

void UECErrorCategory::PostEditUndo()
{
	UEnum::PostEditUndo();

	UpdateAfterPathChanged();
	PostChangedInEditor().Broadcast(*this, {}, false);
}

void UECErrorCategory::UpdateAfterPathChanged()
{
	TArray<TPair<FName, int64>> NewEnumeratorsNames;
	const int32 EnumeratorsToCopy = NumEnums() - 1; // skip _MAX
	for (int32 Idx = 0; Idx < EnumeratorsToCopy; Idx++)
	{
		const FString ShortName = GetNameStringByIndex(Idx);
		const FString NewFullName = GenerateFullEnumName(*ShortName);
		// Different from UUserDefinedEnum: Values must be preserved
		const int64 Value = GetValueByIndex(Idx);
		NewEnumeratorsNames.Emplace(*NewFullName, Value);
	}

	SetEnums(NewEnumeratorsNames, UEnum::ECppForm::Namespaced);
}

void UECErrorCategory::UpgradeDisplayNamesFromMetaData()
{
	const int32 EnumeratorsToEnsure = FMath::Max(NumEnums() - 1, 0);
	DisplayNameMap.Empty(EnumeratorsToEnsure);

	bool bDidUpgradeDisplayNames = false;
	for (int32 Index = 0; Index < EnumeratorsToEnsure; ++Index)
	{
		const FString& MetaDataEntryDisplayName = GetMetaData(TEXT("DisplayName"), Index);
		if (!MetaDataEntryDisplayName.IsEmpty())
		{
			bDidUpgradeDisplayNames = true;

			const FName EnumEntryName = *GetNameStringByIndex(Index);

			FText DisplayNameToSet;

#if USE_STABLE_LOCALIZATION_KEYS
			// Give the new text instance the full package-based namespace, and give it a brand new key; these will become its new stable identity
			{
				const FString PackageNamespace = GIsEditor ? TextNamespaceUtil::EnsurePackageNamespace(this) : TextNamespaceUtil::GetPackageNamespace(this);
				const FString TextNamespace = TextNamespaceUtil::BuildFullNamespace(TEXT(""), PackageNamespace, true);
				const FString TextKey = FGuid::NewGuid().ToString();
				DisplayNameToSet = FInternationalization::ForUseOnlyByLocMacroAndGraphNodeTextLiterals_CreateText(*MetaDataEntryDisplayName, *TextNamespace, *TextKey);
			}
#else  // USE_STABLE_LOCALIZATION_KEYS
			DisplayNameToSet = FText::FromName(EnumEntryName);
#endif // USE_STABLE_LOCALIZATION_KEYS

			DisplayNameMap.Add(EnumEntryName, DisplayNameToSet);
		}
	}

#if USE_STABLE_LOCALIZATION_KEYS
	if (bDidUpgradeDisplayNames)
	{
		UE_LOG(LogErrorHandling, Warning, TEXT("ErrorCode '%s' was upgraded to use FText to store its display name data. Please re-save this asset to avoid issues with localization and determinstic cooking."), *GetPathName());
	}
#endif // USE_STABLE_LOCALIZATION_KEYS
}

void UECErrorCategory::EnsureAllDisplayNamesExist()
{
	const int32 EnumeratorsToEnsure = FMath::Max(NumEnums() - 1, 0);

	// Remove any stale display names
	{
		TSet<FName> KnownEnumEntryNames;
		KnownEnumEntryNames.Reserve(EnumeratorsToEnsure);

		for (int32 Index = 0; Index < EnumeratorsToEnsure; ++Index)
		{
			const FName EnumEntryName = *GetNameStringByIndex(Index);
			KnownEnumEntryNames.Add(EnumEntryName);
		}

		for (auto DisplayNameIt = DisplayNameMap.CreateIterator(); DisplayNameIt; ++DisplayNameIt)
		{
			if (!KnownEnumEntryNames.Contains(DisplayNameIt->Key))
			{
				DisplayNameIt.RemoveCurrent();
			}
		}
	}

	DisplayNameMap.Reserve(EnumeratorsToEnsure);

	// Add any missing display names
	for	(int32 Index = 0; Index < EnumeratorsToEnsure; ++Index)
	{
		const FName EnumEntryName = *GetNameStringByIndex(Index);

		if (!DisplayNameMap.Contains(EnumEntryName))
		{
			FText DisplayNameToSet;

#if USE_STABLE_LOCALIZATION_KEYS
			// Give the new text instance the full package-based namespace, and give it a brand new key; these will become its new stable identity
			{
				const FString PackageNamespace = GIsEditor ? TextNamespaceUtil::EnsurePackageNamespace(this) : TextNamespaceUtil::GetPackageNamespace(this);
				const FString TextNamespace = TextNamespaceUtil::BuildFullNamespace(FString(), PackageNamespace, true);
				const FString TextKey = FGuid::NewGuid().ToString();
				const FString EnumEntryDisplayName = EnumEntryName.ToString();
				DisplayNameToSet = FInternationalization::ForUseOnlyByLocMacroAndGraphNodeTextLiterals_CreateText(*EnumEntryDisplayName, *TextNamespace, *TextKey);
			}
#else  // USE_STABLE_LOCALIZATION_KEYS
			DisplayNameToSet = FText::FromName(EnumEntryName);
#endif // USE_STABLE_LOCALIZATION_KEYS

			DisplayNameMap.Add(EnumEntryName, DisplayNameToSet);
		}
	}
}

FString UECErrorCategory::GenerateNewErrorCodeName()
{
	FString ErrorCodeString;
	do
	{
		ErrorCodeString = FString::Printf(TEXT("NewErrorCode%u"), UniqueNameIndex);
		++UniqueNameIndex;
	}
	while (!IsProperErrorCodeName(ErrorCodeString));
	return ErrorCodeString;
}

bool UECErrorCategory::IsProperErrorCodeName(const FString& NewName)
{
	if (IsFullEnumName(*NewName))
	{
		return false;
	}

	check(GetFName().IsValidXName());

	const FName ShortName(*NewName);
	const bool bValidName = ShortName.IsValidXName(INVALID_OBJECTNAME_CHARACTERS);

	const FString FullNameStr = GenerateFullEnumName(*NewName);
	const FName FullName(*FullNameStr);
	check(!bValidName || FullName.IsValidXName());

	const bool bNameNotUsed = (GetIndexByName(FullName) == INDEX_NONE);
	return bNameNotUsed && bValidName;
}

bool UECErrorCategory::IsDisplayNameValidAndUnique(int32 EnumeratorIndex, const FText& NewDisplayName) const
{
	if (NewDisplayName.IsEmptyOrWhitespace() || NewDisplayName.ToString() == TEXT("(INVALID)"))
	{
		return false;
	}

	// Names must be unique
	for (int32 Idx = 0; Idx < NumEnums(); ++Idx)
	{
		if (Idx != EnumeratorIndex && NewDisplayName.ToString() == GetDisplayNameTextByIndex(Idx).ToString())
		{
			return false;
		}
	}

	return true;
}

#endif

#if WITH_EDITORONLY_DATA
FECErrorCategoryChanged& UECErrorCategory::PostChangedInEditor()
{
	static FECErrorCategoryChanged PostChangedDelegate;
	return PostChangedDelegate;
}
#endif
