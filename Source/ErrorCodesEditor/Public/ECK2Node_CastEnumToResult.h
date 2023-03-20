// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "NodeDependingOnEnumInterface.h"
#include "UObject/Object.h"
#include "ECK2Node_CastEnumToResult.generated.h"

/**
 * K2Node which casts an arbitrary enum value into an FECResult.
 */
UCLASS(MinimalAPI)
class UECK2Node_CastEnumToResult : public UK2Node, public INodeDependingOnEnumInterface
{
	GENERATED_BODY()

public:
	UECK2Node_CastEnumToResult(const FObjectInitializer& ObjectInitializer);

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual void ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const override;
	
	//~ End UEdGraphNode Interface

	//~ Begin UK2Node Interface
	virtual FText GetCompactNodeTitle() const override;
	virtual bool ShouldDrawCompact() const override { return true; }
	virtual bool IsNodePure() const override { return true; }
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual bool IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	//~ End UK2Node Interface

	// INodeDependingOnEnumInterface
	virtual class UEnum* GetEnum() const override { return Enum; }
	virtual void ReloadEnum(class UEnum* InEnum) override;
	virtual bool ShouldBeReconstructedAfterEnumChanged() const override { return false; }
	// End of INodeDependingOnEnumInterface

	virtual FName GetFunctionName() const;

	UPROPERTY()
	TObjectPtr<UEnum> Enum;

	static FName GetEnumInputPinName();

private:
	/** Constructing FText strings can be costly, so we cache the node's tooltip */
	FNodeTextCache CachedTooltip;
};
