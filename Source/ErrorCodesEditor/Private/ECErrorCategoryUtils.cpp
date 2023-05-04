// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECErrorCategoryUtils.h"
#include "ECErrorCategory.h"
#include "ECResult.h"
#include "IECNodeDependingOnErrorCategory.h"
#include "K2Node_Variable.h"
#include "NodeDependingOnEnumInterface.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/UserDefinedStruct.h"
#include "Internationalization/TextPackageNamespaceUtil.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/EnumEditorUtils.h"
#include "Kismet2/StructureEditorUtils.h"
#include "UObject/PropertyIterator.h"

#define LOCTEXT_NAMESPACE "ErrorCodesEditor_ErrorCategoryUtils"

class FArchiveEnumeratorResolver : public FArchiveUObject
{
public:
	const UEnum* Enum;
	const TArray<TPair<FName, int64>>& OldNames;

	FArchiveEnumeratorResolver(const UEnum* InEnum, const TArray<TPair<FName, int64>>& InOldNames)
		: FArchiveUObject(), Enum(InEnum), OldNames(InOldNames)
	{
	}

	virtual bool UseToResolveEnumerators() const override
	{
		return true;
	}
};

void ErrorCodes::BroadcastPreChange(UECErrorCategory& ErrorCategory)
{
	FEnumEditorUtils::FEnumEditorManager::Get().PreChange(&ErrorCategory, FEnumEditorUtils::Changed);
	ErrorCategory.Modify();
}

// Copied from FEnumEditorUtils::BroadcastChanges
void ErrorCodes::BroadcastPostChange(const UECErrorCategory& ErrorCategory,
	const TArray<TPair<FName, int64>>& OldNames,
	bool bResolveData
)
{
	if (bResolveData)
	{
		FArchiveEnumeratorResolver EnumeratorResolver(&ErrorCategory, OldNames);

		// Track any user defined structs that may have been modified during this enum update
		TSet<UUserDefinedStruct*> EffectedUserStructs;
		
		// Helper lambda to get the class of a UStruct off of a property
		auto GetStructClass = [&EffectedUserStructs](const FProperty* Prop) -> UClass*
		{
			if(Prop)
			{
				// Attempt to use this property's default owner class
				if(UClass* OwnerClass = Prop->GetOwnerClass())
				{
					return OwnerClass;	
				}
				// Otherwise check for UserDefinedStructs that may have this property
				else if (UUserDefinedStruct* UserStruct = Cast<UUserDefinedStruct>(Prop->GetOwnerStruct()))
				{
					// Exclude UDS compiler artifacts; they don't require regeneration.
					if (!UserStruct->PrimaryStruct.IsValid())
					{
						EffectedUserStructs.Add(UserStruct);
					}
				}
			}
			return nullptr;
		};

		TSet<UClass*> ClassesToCheck;

		for (TPropertyIterator<FByteProperty> It; It; ++It)
		{
			const FByteProperty* ByteProperty = *It;
			if (ByteProperty && (&ErrorCategory == ByteProperty->GetIntPropertyEnum()))
			{
				if(UClass* StructClass = GetStructClass(ByteProperty))
				{					
					ClassesToCheck.Add(StructClass);
				}
			}
		}

		for (TPropertyIterator<FEnumProperty> It; It; ++It)
		{
			const FEnumProperty* EnumProperty = *It;
			if (EnumProperty && (&ErrorCategory == EnumProperty->GetEnum()))
			{
				// Check for user defined structs that may have this property
				if (UClass* StructClass = GetStructClass(EnumProperty))
				{
					ClassesToCheck.Add(StructClass);
				}
			}
		}

		for (FThreadSafeObjectIterator ObjIter; ObjIter; ++ObjIter)
		{
			for (UClass* Class : ClassesToCheck)
			{
				if (ObjIter->IsA(Class))
				{
					ObjIter->Serialize(EnumeratorResolver);
					break;
				}
			}
		}

		// User defined structs have to be notified that their structure has been changed after 
		// serialization in order for the structure editor to get updated properly
		for(UUserDefinedStruct* Struct : EffectedUserStructs)
		{
			Struct->Serialize(EnumeratorResolver);

			FStructureEditorUtils::ModifyStructData(Struct);
			FStructureEditorUtils::OnStructureChanged(Struct);
		}
	}

	struct FNodeValidatorHelper
	{
		static bool IsValid(UK2Node* Node)
		{
			return ::IsValid(Node)
				&& (NULL != Cast<UEdGraph>(Node->GetOuter()))
				&& !Node->HasAnyFlags(RF_Transient);
		}
	};

	TSet<UBlueprint*> BlueprintsToRefresh;

	{
		//CUSTOM NODES DEPENTENT ON ENUM

		for (TObjectIterator<UK2Node> It(RF_Transient); It; ++It)
		{
			UK2Node* Node = *It;
			INodeDependingOnEnumInterface* NodeDependingOnEnum = Cast<INodeDependingOnEnumInterface>(Node);
			if (FNodeValidatorHelper::IsValid(Node) && NodeDependingOnEnum && (&ErrorCategory == NodeDependingOnEnum->GetEnum()))
			{
				if (Node->HasValidBlueprint())
				{
					if (NodeDependingOnEnum->ShouldBeReconstructedAfterEnumChanged())
					{
						Node->ReconstructNode();
					}
					BlueprintsToRefresh.Add(Node->GetBlueprint());
				}
			}
		}

		for (TObjectIterator<UK2Node> It(RF_Transient); It; ++It)
		{
			UK2Node* Node = *It;
			IECNodeDependingOnErrorCategory* NodeDependingOnEnum = Cast<IECNodeDependingOnErrorCategory>(Node);
			if (FNodeValidatorHelper::IsValid(Node) && NodeDependingOnEnum && (NodeDependingOnEnum->DependsOnErrorCategory(ErrorCategory)))
			{
				if (Node->HasValidBlueprint())
				{
					if (NodeDependingOnEnum->ShouldBeReconstructedAfterChanges())
					{
						Node->ReconstructNode();
					}
					BlueprintsToRefresh.Add(Node->GetBlueprint());
				}
			}
		}
	}

	for (TObjectIterator<UEdGraphNode> It(RF_Transient); It; ++It)
	{
		for (UEdGraphPin* Pin : It->Pins)
		{
			if (Pin && (Pin->PinType.PinSubCategory != UEdGraphSchema_K2::PSC_Bitmask) && (&ErrorCategory == Pin->PinType.PinSubCategoryObject.Get()) && (EEdGraphPinDirection::EGPD_Input == Pin->Direction))
			{
				UK2Node* Node = Cast<UK2Node>(Pin->GetOuter());
				if (FNodeValidatorHelper::IsValid(Node))
				{
					if (Node->HasValidBlueprint())
					{
						UBlueprint* Blueprint = Node->GetBlueprint();
						if (INDEX_NONE == ErrorCategory.GetIndexByNameString(Pin->DefaultValue))
						{
							Pin->Modify();
							if (Blueprint->BlueprintType == BPTYPE_Interface)
							{
								Pin->DefaultValue = ErrorCategory.GetNameStringByIndex(0);
							}
							else
							{
								Pin->DefaultValue = TEXT("(INVALID)");
							}
							Node->PinDefaultValueChanged(Pin);
							BlueprintsToRefresh.Add(Blueprint);
						}
					}
				}
			}
		}
	}

	// Modify any properties that are using the enum as a bitflags type for bitmask values inside a Blueprint class.
	for (TPropertyIterator<FIntProperty> PropertyIter; PropertyIter; ++PropertyIter)
	{
		const FIntProperty* IntProperty = *PropertyIter;
		if (IntProperty && IntProperty->HasMetaData(*FBlueprintMetadata::MD_Bitmask.ToString()))
		{
			UClass* OwnerClass = IntProperty->GetOwnerClass();
			if (OwnerClass)
			{
				// Note: We only need to consider the skeleton class here.
				UBlueprint* Blueprint = Cast<UBlueprint>(OwnerClass->ClassGeneratedBy);
				if (Blueprint && OwnerClass == Blueprint->SkeletonGeneratedClass)
				{
					const FString& BitmaskEnumName = IntProperty->GetMetaData(FBlueprintMetadata::MD_BitmaskEnum);
					if (BitmaskEnumName == ErrorCategory.GetName() && !ErrorCategory.HasMetaData(*FBlueprintMetadata::MD_Bitflags.ToString()))
					{
						FName VarName = IntProperty->GetFName();

						// This will remove the metadata key from both the skeleton & full class.
						FBlueprintEditorUtils::RemoveBlueprintVariableMetaData(Blueprint, VarName, nullptr, FBlueprintMetadata::MD_BitmaskEnum);

						// Need to reassign the property since the skeleton class will have been regenerated at this point.
						IntProperty = FindFieldChecked<FIntProperty>(Blueprint->SkeletonGeneratedClass, VarName);

						// Reconstruct any nodes that reference the variable that was just modified.
						for (TObjectIterator<UK2Node_Variable> VarNodeIt; VarNodeIt; ++VarNodeIt)
						{
							UK2Node_Variable* VarNode = *VarNodeIt;
							if (VarNode && VarNode->GetPropertyForVariable() == IntProperty)
							{
								VarNode->ReconstructNode();

								if (VarNode->HasValidBlueprint())
								{
									BlueprintsToRefresh.Add(VarNode->GetBlueprint());
								}
							}
						}

						BlueprintsToRefresh.Add(Blueprint);
					}
				}
			}
		}
	}

	for (auto It = BlueprintsToRefresh.CreateIterator(); It; ++It)
	{
		FBlueprintEditorUtils::MarkBlueprintAsModified(*It);
		(*It)->BroadcastChanged();
	}

	FEnumEditorUtils::FEnumEditorManager::Get().PostChange(&ErrorCategory, FEnumEditorUtils::Changed);
}

void ErrorCodes::SetErrorValueDisplayName(UECErrorCategory& Category,
	int32 Idx,
	const FText& NewDisplayName
	)
{
	if (Idx < 0 || Idx >= Category.NumEnums() ||
		!Category.IsDisplayNameValidAndUnique(Idx, NewDisplayName))
	{
		return;
	}
	
	const FScopedTransaction Transaction(NSLOCTEXT("EnumEditor", "SetEnumeratorDisplayName", "Set Display Name"));

	ErrorCodes::BroadcastPreChange(Category);
	
	const FName EnumEntryName = *Category.GetNameStringByIndex(Idx);

	FText DisplayNameToSet = NewDisplayName;

#if USE_STABLE_LOCALIZATION_KEYS
	// Make sure the package namespace for this display name is up-to-date
	{
		const FString PackageNamespace = TextNamespaceUtil::EnsurePackageNamespace(&Category);
		if (!PackageNamespace.IsEmpty())
		{
			const FString DisplayNameNamespace = FTextInspector::GetNamespace(DisplayNameToSet).Get(FString());
			const FString FullNamespace = TextNamespaceUtil::BuildFullNamespace(DisplayNameNamespace, PackageNamespace);
			if (!DisplayNameNamespace.Equals(FullNamespace, ESearchCase::CaseSensitive))
			{
				// We may assign a new key when if we don't have the correct package namespace in order to avoid identity conflicts when instancing
				DisplayNameToSet = FText::ChangeKey(FullNamespace, FGuid::NewGuid().ToString(), DisplayNameToSet);
			}
		}
	}
#endif // USE_STABLE_LOCALIZATION_KEYS

	Category.DisplayNameMap.Add(EnumEntryName, DisplayNameToSet);

	ErrorCodes::BroadcastPostChange(Category, TArray<TPair<FName, int64>>(), false);
}

void ErrorCodes::SetErrorValueMessage(UECErrorCategory& Category, int32 Idx, const FText& NewMessage)
{
	//TODO: Metadata is not transactional right now, so we cannot transact a tooltip edit
	// const FScopedTransaction Transaction(NSLOCTEXT("EnumEditor", "SetEnumeratorTooltip", "Set Description"));
	Category.Modify();
	Category.SetMetaData(TEXT("ToolTip"), *NewMessage.ToString(), Idx);
}

void ErrorCodes::AddErrorValueToCategory(UECErrorCategory& Category, int64 NewCode)
{
	const FScopedTransaction Transaction(LOCTEXT("Transaction_AddEntry", "Add Error Code"));

	ErrorCodes::BroadcastPreChange(Category);
	
	TArray<TPair<FName, int64>> OldNames;
	CopyErrorValuesWithoutMax(OldNames, Category);
	TArray<TPair<FName, int64>> Names = OldNames;

	FString EnumNameStr = Category.GenerateNewErrorCodeName();
	const FString FullNameStr = Category.GenerateFullEnumName(*EnumNameStr);
	Names.Emplace(*FullNameStr, NewCode);

	// Note that we do NOT clean up enum values as it will invalidate error code references

	const UEnum::ECppForm EnumType = Category.GetCppForm();
	Category.SetEnums(Names, EnumType);
	Category.EnsureAllDisplayNamesExist();

	ErrorCodes::BroadcastPostChange(Category, OldNames);

	Category.MarkPackageDirty();
}

void ErrorCodes::AddSuccessValueToCategory(UECErrorCategory& Category)
{
	TArray<TPair<FName, int64>> OldNames;
	CopyErrorValuesWithoutMax(OldNames, Category);
	TArray<TPair<FName, int64>> Names = OldNames;

	FString EnumNameStr = TEXT("Success");
	const FString FullNameStr = Category.GenerateFullEnumName(*EnumNameStr);
	Names.Emplace(*FullNameStr, FECResult::GetSuccessValue());

	// Note that we do NOT clean up enum values as it will invalidate error code references

	const UEnum::ECppForm EnumType = Category.GetCppForm();
	Category.SetEnums(Names, EnumType);
	Category.EnsureAllDisplayNamesExist();
	
	ErrorCodes::SetErrorValueMessage(Category, 0, LOCTEXT("Message_Success", "Successful result."));

	Category.MarkPackageDirty();
}

void ErrorCodes::RemoveErrorValueFromCategory(UECErrorCategory& Category, int32 Idx)
{
	if (Idx <= 0 || Idx >= Category.NumEnums() - 1)
	{
		return;
	}
	
	const FScopedTransaction Transaction(LOCTEXT("Transaction_RemoveErrorCode", "Remove Error Code"));

	ErrorCodes::BroadcastPreChange(Category);

	TArray<TPair<FName, int64>> OldNames;
	CopyErrorValuesWithoutMax(OldNames, Category);
	TArray<TPair<FName, int64>> Names = OldNames;

	Names.RemoveAt(Idx);
	
	// Note that we do NOT clean up enum values as it will invalidate error code references

	const UEnum::ECppForm EnumType = Category.GetCppForm();
	Category.SetEnums(Names, EnumType);
	Category.EnsureAllDisplayNamesExist();
	
	ErrorCodes::BroadcastPostChange(Category, OldNames);
	Category.MarkPackageDirty();
}

void ErrorCodes::CopyErrorValuesWithoutMax(TArray<TPair<FName, int64>>& OutEnumPairs, const UECErrorCategory& Category)
{
	const int32 NumEnums = Category.NumEnums() - 1;
	for (int32 Idx = 0; Idx < NumEnums; ++Idx)
	{
		OutEnumPairs.Emplace(Category.GetNameByIndex(Idx), Category.GetValueByIndex(Idx));
	}
}

void ErrorCodes::FindAllErrorCategories(TArray<const UEnum*>& OutErrorCategories)
{
	// Find C++ defined error categories
	for (TObjectIterator<UEnum> EnumIt; EnumIt; ++EnumIt)
	{
		UEnum* Category = *EnumIt;
		// BP error categories are found by the asset search in the loop below
		if (!IsValid(Category) || Category->IsA<UECErrorCategory>() || !Category->HasMetaData(TEXT("ErrorCategory")))
		{
			continue;
		}

		OutErrorCategories.Emplace(Category);
	}

	// Find asset error categories
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	TArray<FAssetData> EnumList;
	FARFilter AssetFilter;
	AssetFilter.ClassPaths.Add(UECErrorCategory::StaticClass()->GetClassPathName());
	AssetFilter.bRecursiveClasses = true;
	AssetRegistryModule.Get().GetAssets(AssetFilter, EnumList);
	for (const FAssetData& AssetData : EnumList)
	{
		if (!AssetData.IsInstanceOf(UECErrorCategory::StaticClass()))
		{
			continue;
		}
		
		UEnum* Category = Cast<UEnum>(AssetData.FastGetAsset(true));
		if (!IsValid(Category))
		{
			continue;
		}

		OutErrorCategories.Emplace(Category);
	}
}

#undef LOCTEXT_NAMESPACE
