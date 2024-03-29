// Copyright 2023 Miraganic Studios.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_Switch.h"
#include "ECResult.h"
#include "IECNodeDependingOnErrorCategory.h"
#include "UObject/Object.h"
#include "ECK2Node_SwitchResult.generated.h"

/**
 * Blueprint node for switching over error codes.
 */
UCLASS(MinimalAPI)
class UECK2Node_SwitchResult : public UK2Node_Switch, public IECNodeDependingOnErrorCategory
{
	GENERATED_BODY()

public:
	UECK2Node_SwitchResult(const FObjectInitializer& ObjectInitializer);
	
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
	virtual void PreloadRequiredAssets() override;
	// End of UEdGraphNode interface

	// UK2Node interface
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	// End of UK2Node interface

	// UK2Node_Switch Interface
	MIRAGANICERRORHANDLINGEDITOR_API virtual void AddPinToSwitchNode() override;
	virtual FName GetUniquePinName() override;
	virtual FEdGraphPinType GetPinType() const override;
	virtual bool CanRemoveExecutionPin(UEdGraphPin* TargetPin) const override;
	// End of UK2Node_Switch Interface

	// IECNodeDependingOnErrorCategory Interface
	virtual bool DependsOnErrorCategory(const UECErrorCategory& Category) const override;
	virtual void ReloadErrorCategory(UECErrorCategory* Category) override;
	virtual bool ShouldBeReconstructedAfterChanges() const override { return true; }
	// End IECNodeDependingOnErrorCategory Interface

	virtual FName GetPinNameGivenIndex(int32 Index) const override;

	/**
	 * Add pins for all error codes in a category that aren't already included.
	 * @return The number of new error codes that were added.
	 */
	int32 AddUniqueCodesFromCategory(const UEnum& Category);

protected:
	virtual void CreateFunctionPin() override;
	virtual void CreateSelectionPin() override;
	virtual void CreateCasePins() override;
	virtual void RemovePin(UEdGraphPin* TargetPin) override;

	FName GetNameForErrorCodePin(const FECResult& ErrorCode) const;
	FString GetTooltipForErrorCodePin(const FECResult& ErrorCode) const;

	static FName GetSuccessPinName();

public:
	UPROPERTY(EditAnywhere, Category = PinOptions)
	TArray<FECResult> PinResultCodes;

	UPROPERTY()
	TArray<FName> PinNames;
};
