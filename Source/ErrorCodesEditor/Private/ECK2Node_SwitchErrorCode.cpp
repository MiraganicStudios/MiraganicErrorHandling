﻿// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECK2Node_SwitchErrorCode.h"

#include "MGErrorFunctionLibrary.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "ECEditorLogging.h"
#include "MGErrorCategory.h"
#include "MGErrorFunctionLibrary.h"

UECK2Node_SwitchErrorCode::UECK2Node_SwitchErrorCode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FunctionName = GET_FUNCTION_NAME_CHECKED(UMGErrorFunctionLibrary, NotEqual_ErrorCodeErrorCode);
	FunctionClass = UMGErrorFunctionLibrary::StaticClass();
	OrphanedPinSaveMode = ESaveOrphanPinMode::SaveNone;
}

void UECK2Node_SwitchErrorCode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	bool bIsDirty = false;
	const FName PropertyName = (PropertyChangedEvent.MemberProperty ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None);
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UECK2Node_SwitchErrorCode, PinErrorCodes))
	{
		bIsDirty = true;
	}

	if (bIsDirty)
	{
		ReconstructNode();
		GetGraph()->NotifyGraphChanged();
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UECK2Node_SwitchErrorCode::PostLoad()
{
	Super::PostLoad();
	if (UEdGraphPin* FunctionPin = FindPin(FunctionName))
	{
		FunctionPin->DefaultObject = FunctionClass->GetDefaultObject();
	}
}

FText UECK2Node_SwitchErrorCode::GetTooltipText() const
{
	return NSLOCTEXT("ErrorCodes", "K2Node_SwitchErrorCode_ToolTip", "Selects an output that matches the input value");
}

FText UECK2Node_SwitchErrorCode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return NSLOCTEXT("ErrorCodes", "K2Node_SwitchErrorCode_Title", "Switch on Error Code");
}

FString UECK2Node_SwitchErrorCode::GetExportTextForPin(const UEdGraphPin* Pin) const
{
	const int32 PinIdx = PinNames.IndexOfByKey(Pin->PinName);
	if (PinIdx == INDEX_NONE)
	{
		return FString();
	}
	
	const FMGErrorCode PinLiteralValue = PinErrorCodes[PinIdx];
	const FMGErrorCode DefaultValue;
	FString ExportText;
	FMGErrorCode::StaticStruct()->ExportText(ExportText, &PinLiteralValue, &DefaultValue, nullptr, PPF_None, nullptr);
	return ExportText;
}

// void UECK2Node_SwitchErrorCode::ValidateNodeDuringCompilation(FCompilerResultsLog& MessageLog) const
// {
// 	Super::ValidateNodeDuringCompilation(MessageLog);
//
// 	UE_LOG(LogErrorCodesEditor, Warning, TEXT("%s"), *FString(__FUNCTION__));
// }

void UECK2Node_SwitchErrorCode::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	// actions get registered under specific object-keys; the idea is that 
	// actions might have to be updated (or deleted) if their object-key is  
	// mutated (or removed)... here we use the node's class (so if the node 
	// type disappears, then the action should go with it)
	UClass* ActionKey = GetClass();
	// to keep from needlessly instantiating a UBlueprintNodeSpawner, first   
	// check to make sure that the registrar is looking for actions of this type
	// (could be regenerating actions for a specific asset, and therefore the 
	// registrar would only accept actions corresponding to that asset)
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

void UECK2Node_SwitchErrorCode::AddPinToSwitchNode()
{
	FName PinName = GetUniquePinName();
	PinNames.Add(PinName);

	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, PinName);
	if (PinErrorCodes.Num() < PinNames.Num())
	{ 
		PinErrorCodes.Add(FMGErrorCode());
	}
}

FName UECK2Node_SwitchErrorCode::GetUniquePinName()
{
	int32 Index = 0;
	FName NewPinName;
	do
	{
		NewPinName = *FString::Printf(TEXT("Case_%d"), Index++);
	}
	while (FindPin(NewPinName));
	
	return NewPinName;
}

FEdGraphPinType UECK2Node_SwitchErrorCode::GetPinType() const
{
	FEdGraphPinType PinType;
	PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
	PinType.PinSubCategoryObject = FMGErrorCode::StaticStruct();
	return PinType;
}

FName UECK2Node_SwitchErrorCode::GetPinNameGivenIndex(int32 Index) const
{
	check(Index);
	return PinNames[Index];
}

void UECK2Node_SwitchErrorCode::CreateFunctionPin()
{
	// Set properties on the function pin
	UEdGraphPin* FunctionPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, FunctionClass, FunctionName);
	FunctionPin->bDefaultValueIsReadOnly = true;
	FunctionPin->bNotConnectable = true;
	FunctionPin->bHidden = true;

	UFunction* Function = FindUField<UFunction>(FunctionClass, FunctionName);
	const bool bIsStaticFunc = Function->HasAllFunctionFlags(FUNC_Static);
	if (bIsStaticFunc)
	{
		// Wire up the self to the CDO of the class if it's not us
		if (UBlueprint* BP = GetBlueprint())
		{
			UClass* FunctionOwnerClass = Function->GetOuterUClass();
			if (!BP->SkeletonGeneratedClass->IsChildOf(FunctionOwnerClass))
			{
				FunctionPin->DefaultObject = FunctionOwnerClass->GetDefaultObject();
			}
		}
	}
}

void UECK2Node_SwitchErrorCode::CreateSelectionPin()
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	UEdGraphPin* Pin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FMGErrorCode::StaticStruct(), TEXT("Selection"));
	K2Schema->SetPinAutogeneratedDefaultValueBasedOnType(Pin);
}

void UECK2Node_SwitchErrorCode::CreateCasePins()
{
	while (PinErrorCodes.Num() > PinNames.Num())
	{
		const FName PinName = GetUniquePinName();
		PinNames.Add(PinName);
	}

	if (PinNames.Num() > PinErrorCodes.Num())
	{
		PinNames.SetNum(PinErrorCodes.Num());
	}

	for (int32 Index = 0; Index < PinErrorCodes.Num(); ++Index)
	{
		if (PinErrorCodes[Index].IsSuccess())
		{
			PinNames[Index] = FName(TEXT("Success"));
		}
		else
		{
			PinNames[Index] = FName(*PinErrorCodes[Index].GetErrorTitle().ToString());
		}

		CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, PinNames[Index]);
	}
}

void UECK2Node_SwitchErrorCode::RemovePin(UEdGraphPin* TargetPin)
{
	checkSlow(TargetPin);

	FName PinName = TargetPin->PinName;
	// Clean-up pin name array
	int32 Index = PinNames.IndexOfByKey(PinName);
	if (Index >= 0)
	{
		if (Index < PinErrorCodes.Num())
		{
			PinErrorCodes.RemoveAt(Index);
		}
		PinNames.RemoveAt(Index);
	}
}