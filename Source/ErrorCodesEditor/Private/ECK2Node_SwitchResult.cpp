// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECK2Node_SwitchResult.h"

#include "ECErrorFunctionLibrary.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "ECEditorLogging.h"
#include "ECErrorFunctionLibrary.h"
#include "ECErrorCategory.h"
#include "Kismet2/CompilerResultsLog.h"

UECK2Node_SwitchResult::UECK2Node_SwitchResult(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FunctionName = GET_FUNCTION_NAME_CHECKED(UECErrorFunctionLibrary, NotEqual_ResultResult);
	FunctionClass = UECErrorFunctionLibrary::StaticClass();
	OrphanedPinSaveMode = ESaveOrphanPinMode::SaveNone;
}

void UECK2Node_SwitchResult::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	bool bIsDirty = false;
	const FName PropertyName = (PropertyChangedEvent.MemberProperty ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None);
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UECK2Node_SwitchResult, PinResultCodes))
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

void UECK2Node_SwitchResult::PostLoad()
{
	Super::PostLoad();
	if (UEdGraphPin* FunctionPin = FindPin(FunctionName))
	{
		FunctionPin->DefaultObject = FunctionClass->GetDefaultObject();
	}
}

FText UECK2Node_SwitchResult::GetTooltipText() const
{
	return NSLOCTEXT("ErrorCodes", "K2Node_SwitchErrorCode_ToolTip", "Selects an output that matches the input value");
}

FText UECK2Node_SwitchResult::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return NSLOCTEXT("ErrorCodes", "K2Node_SwitchErrorCode_Title", "Switch on Error Code");
}

FString UECK2Node_SwitchResult::GetExportTextForPin(const UEdGraphPin* Pin) const
{
	const int32 PinIdx = PinNames.IndexOfByKey(Pin->PinName);
	if (PinIdx == INDEX_NONE)
	{
		return FString();
	}
	
	const FECResult PinLiteralValue = PinResultCodes[PinIdx];
	const FECResult DefaultValue;
	FString ExportText;
	FECResult::StaticStruct()->ExportText(ExportText, &PinLiteralValue, &DefaultValue, nullptr, PPF_None, nullptr);
	return ExportText;
}

void UECK2Node_SwitchResult::ValidateNodeDuringCompilation(FCompilerResultsLog& MessageLog) const
{
	Super::ValidateNodeDuringCompilation(MessageLog);

	for (const FECResult& PinResultCode : PinResultCodes)
	{
		if (!PinResultCode.HasValidError())
		{
			MessageLog.Error(*NSLOCTEXT("ErrorCodes", "SwitchErrorCode_SuccessCase", "@@ contains invalid case(s).").ToString(), this);
			return;
		}
	}
}

void UECK2Node_SwitchResult::PreloadRequiredAssets()
{
	for (const FECResult& ResultCode : PinResultCodes)
	{
		UEnum* ErrorCategoryMut = const_cast<UEnum*>(ResultCode.GetCategory());
		PreloadObject(ErrorCategoryMut);
	}
	Super::PreloadRequiredAssets();
}

void UECK2Node_SwitchResult::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
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

void UECK2Node_SwitchResult::AddPinToSwitchNode()
{
	FName PinName = GetUniquePinName();
	PinNames.Add(PinName);

	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, PinName);
	if (PinResultCodes.Num() < PinNames.Num())
	{ 
		PinResultCodes.Add(FECResult());
	}
}

FName UECK2Node_SwitchResult::GetUniquePinName()
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

FEdGraphPinType UECK2Node_SwitchResult::GetPinType() const
{
	FEdGraphPinType PinType;
	PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
	PinType.PinSubCategoryObject = FECResult::StaticStruct();
	return PinType;
}

bool UECK2Node_SwitchResult::CanRemoveExecutionPin(UEdGraphPin* TargetPin) const
{
	if (TargetPin && TargetPin->PinName == GetSuccessPinName())
	{
		// Can't remove the 'Success' pin.
		return false;
	}
	
	return Super::CanRemoveExecutionPin(TargetPin);
}

bool UECK2Node_SwitchResult::DependsOnErrorCategory(const UECErrorCategory& Category) const
{
	for (const FECResult& PinResultCode : PinResultCodes)
	{
		if (PinResultCode.GetCategory() == &Category)
		{
			return true;
		}
	}

	return false;
}

void UECK2Node_SwitchResult::ReloadErrorCategory(UECErrorCategory* Category)
{
	// TODO: For each pin that depends on that category, reload its data
	// TODO: What is this for? Hot reload?
}

FName UECK2Node_SwitchResult::GetPinNameGivenIndex(int32 Index) const
{
	check(Index);
	return PinNames[Index];
}

int32 UECK2Node_SwitchResult::AddUniqueCodesFromCategory(const UEnum& Category)
{
	int32 NumAdded = 0;
	const int32 NumEnums = Category.NumEnums() - 1;
	for (int32 Idx = 0; Idx < NumEnums; ++Idx)
	{
		const int64 Value = Category.GetValueByIndex(Idx);
		// 0 is reserved for 'Success' result codes
		if (Value == FECResult::Success().GetValue())
		{
			continue;
		}
		const FECResult ResultCode(Category, Value);
		const int32 NewPinIdx = PinResultCodes.Num();
		const int32 AddedIdx = PinResultCodes.AddUnique(ResultCode);
		// Already exists
		if (AddedIdx != NewPinIdx)
		{
			continue;
		}

		const FName PinName = GetNameForErrorCodePin(ResultCode);
		PinNames.Emplace(PinName);
		UEdGraphPin* Pin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, PinName);
		Pin->PinToolTip = GetTooltipForErrorCodePin(ResultCode);
		++NumAdded;
	}

	return NumAdded;
}

void UECK2Node_SwitchResult::CreateFunctionPin()
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

void UECK2Node_SwitchResult::CreateSelectionPin()
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	UEdGraphPin* Pin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FECResult::StaticStruct(), TEXT("Selection"));
	K2Schema->SetPinAutogeneratedDefaultValueBasedOnType(Pin);
}

void UECK2Node_SwitchResult::CreateCasePins()
{
	while (PinResultCodes.Num() > PinNames.Num())
	{
		const FName PinName = GetUniquePinName();
		PinNames.Add(PinName);
	}

	if (PinNames.Num() > PinResultCodes.Num())
	{
		PinNames.SetNum(PinResultCodes.Num());
	}

	// Always have a success pin
	UEdGraphPin* SuccessPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, GetSuccessPinName());
	SuccessPin->PinFriendlyName = NSLOCTEXT("ErrorCodesEditor_K2NodeSwitchErrorCode", "FriendlyName_SuccessPin", "Success");
	SuccessPin->PinToolTip = TEXT("No error.");

	for (int32 Index = 0; Index < PinResultCodes.Num(); ++Index)
	{
		PinNames[Index] = GetNameForErrorCodePin(PinResultCodes[Index]);

		UEdGraphPin* CasePin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, PinNames[Index]);
		
		CasePin->PinToolTip = GetTooltipForErrorCodePin(PinResultCodes[Index]);
	}
}

void UECK2Node_SwitchResult::RemovePin(UEdGraphPin* TargetPin)
{
	checkSlow(TargetPin);

	FName PinName = TargetPin->PinName;
	// Clean-up pin name array
	int32 Index = PinNames.IndexOfByKey(PinName);
	if (Index >= 0)
	{
		if (Index < PinResultCodes.Num())
		{
			PinResultCodes.RemoveAt(Index);
		}
		PinNames.RemoveAt(Index);
	}
}

FName UECK2Node_SwitchResult::GetNameForErrorCodePin(const FECResult& ResultCode) const
{
	if (ResultCode.HasValidError())
	{
		return FName(*ResultCode.GetTitle().ToString());
	}
	else
	{
		// Success is invalid for switch nodes (the success node already exists)
		return TEXT("[INVALID]");
	}
}

FString UECK2Node_SwitchResult::GetTooltipForErrorCodePin(const FECResult& ResultCode) const
{
	if (ResultCode.HasValidError())
	{
		return FString::Format(TEXT("{0}:{1}{2}{3}"), {*ResultCode.GetCategoryName().ToString(),
				*ResultCode.GetTitle().ToString(), LINE_TERMINATOR, *ResultCode.GetMessage().ToString()});
	}
	else if (ResultCode.IsSuccess())
	{
		return TEXT("Invalid result code: Success (Cannot add extra 'Success' pins).");
	}
	else
	{
		return ResultCode.ToString();
	}
}

FName UECK2Node_SwitchResult::GetSuccessPinName()
{
	return TEXT("EC_RESERVED_Success");
}
