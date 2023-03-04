// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_Switch.h"
#include "ECErrorCode.h"
#include "UObject/Object.h"
#include "ECK2Node_SwitchErrorCode.generated.h"

/**
 * Blueprint node for switching over error codes.
 */
UCLASS(MinimalAPI)
class UECK2Node_SwitchErrorCode : public UK2Node_Switch
{
	GENERATED_BODY()

public:
	UECK2Node_SwitchErrorCode(const FObjectInitializer& ObjectInitializer);
	
	// UObject interface
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostLoad() override;
	// End of UObject interface

	// UEdGraphNode interface
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual bool ShouldShowNodeProperties() const override { return true; }
	virtual FString GetExportTextForPin(const UEdGraphPin* Pin) const override;
	virtual void ValidateNodeDuringCompilation(FCompilerResultsLog& MessageLog) const override;
	// End of UEdGraphNode interface

	// UK2Node interface
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	// End of UK2Node interface

	// UK2Node_Switch Interface
	ERRORCODESEDITOR_API virtual void AddPinToSwitchNode() override;
	virtual FName GetUniquePinName() override;
	virtual FEdGraphPinType GetPinType() const override;
	// End of UK2Node_Switch Interface

	virtual FName GetPinNameGivenIndex(int32 Index) const override;

protected:
	virtual void CreateFunctionPin() override;
	virtual void CreateSelectionPin() override;
	virtual void CreateCasePins() override;
	virtual void RemovePin(UEdGraphPin* TargetPin) override;

public:
	UPROPERTY(EditAnywhere, Category = PinOptions)
	TArray<FECErrorCode> PinErrorCodes;

	UPROPERTY()
	TArray<FName> PinNames;
};
