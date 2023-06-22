// Copyright 2022 Miraganic Studios. All rights reserved.


#include "ECActorSpawning.h"

FECResult Mgnc::Can_SpawnActor(UWorld& World,
	UClass& ActorClass,
	const FTransform& Transform,
	const FActorSpawnParameters& SpawnParams
	)
{
	// NOTE: All checks are copied from UWorld::SpawnActor
	if (ActorClass.HasAnyClassFlags(CLASS_Deprecated))
	{
		return EECSpawnActorResult::ClassDeprecated;
	}
	if (ActorClass.HasAnyClassFlags(CLASS_Abstract))
	{
		return EECSpawnActorResult::ClassAbstract;
	}
	if (!ActorClass.IsChildOf(AActor::StaticClass()))
	{
		return EECSpawnActorResult::ClassWasNotActor;
	}
	if (SpawnParams.Template && SpawnParams.Template->GetClass() != &ActorClass)
	{
		return EECSpawnActorResult::TemplateClassMismatch;
	}
	if (World.bIsRunningConstructionScript && !SpawnParams.bAllowDuringConstructionScript)
	{
		return EECSpawnActorResult::CalledFromConstructionScript;
	}
	if (World.bIsTearingDown)
	{
		return EECSpawnActorResult::WorldTearingDown;
	}
	if (Transform.ContainsNaN())
	{
		return EECSpawnActorResult::InvalidSpawnTransform;
	}
#if WITH_EDITOR
	if (SpawnParams.OverridePackage && SpawnParams.bCreateActorPackage)
	{
		return EECSpawnActorResult::CannotOverrideAndCreatePackage;
	}
#endif
	ULevel* LevelToSpawnIn = SpawnParams.OverrideLevel
								? SpawnParams.OverrideLevel
								: SpawnParams.Owner
									? SpawnParams.Owner->GetLevel()
									: World.GetCurrentLevel();
	AActor* Template = SpawnParams.Template ? SpawnParams.Template : ActorClass.GetDefaultObject<AActor>();
	check(Template);
	FName NewActorName = SpawnParams.Name;
	UPackage* ExternalPackage = nullptr;
	bool bNeedGloballyUniqueName = false;

#if WITH_EDITOR
	if (SpawnParams.OverridePackage)
	{
		ExternalPackage = SpawnParams.OverridePackage;
		bNeedGloballyUniqueName = true;
	}
	else if (LevelToSpawnIn->ShouldCreateNewExternalActors() && SpawnParams.bCreateActorPackage && !(SpawnParams.ObjectFlags & RF_Transient))
	{
		bNeedGloballyUniqueName = CastChecked<AActor>(ActorClass.GetDefaultObject())->SupportsExternalPackaging();
	}
	if (!GIsEditor)
	{
		bNeedGloballyUniqueName = false;
	}
#endif // WITH_EDITOR
	
	if (!NewActorName.IsNone() && (StaticFindObjectFast(nullptr, LevelToSpawnIn, NewActorName) || (bNeedGloballyUniqueName != FActorSpawnUtils::IsGloballyUniqueName(NewActorName) && SpawnParams.NameMode == FActorSpawnParameters::ESpawnActorNameMode::Requested)))
	{
		switch (SpawnParams.NameMode)
		{
			case FActorSpawnParameters::ESpawnActorNameMode::Required_Fatal:
			case FActorSpawnParameters::ESpawnActorNameMode::Required_ErrorAndReturnNull:
			case FActorSpawnParameters::ESpawnActorNameMode::Required_ReturnNull:
				return EECSpawnActorResult::ObjectNameNotUnique;
			case FActorSpawnParameters::ESpawnActorNameMode::Requested:
				break;
			default:
				checkNoEntry();
		}
	}

	if (!World.CanCreateInCurrentContext(Template))
	{
		return EECSpawnActorResult::NetworkContextMismatch;
	}

	ESpawnActorCollisionHandlingMethod CollisionHandlingOverride = SpawnParams.SpawnCollisionHandlingOverride;
	if (SpawnParams.bNoFail)
	{
		if (CollisionHandlingOverride == ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding)
		{
			CollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		}
		else if (CollisionHandlingOverride == ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding)
		{
			CollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		}
	}
	
	const ESpawnActorCollisionHandlingMethod CollisionHandlingMethod =
		CollisionHandlingOverride == ESpawnActorCollisionHandlingMethod::Undefined
			? Template->SpawnCollisionHandlingMethod
			: CollisionHandlingOverride;

	if (CollisionHandlingMethod == ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding)
	{
		USceneComponent* const TemplateRootComponent = Template->GetRootComponent();

		const FTransform FinalRootComponentTransform = TemplateRootComponent
															? TemplateRootComponent->GetRelativeTransform() * Transform
															: Transform;

		const FVector FinalRootLocation = FinalRootComponentTransform.GetLocation();
		const FRotator FinalRootRotation = FinalRootComponentTransform.Rotator();
		// Unclear why UWorld::EncroachingBlockingGeometry is not const; probably an oversight
		if (World.EncroachingBlockingGeometry(Template, FinalRootLocation, FinalRootRotation))
		{
			return EECSpawnActorResult::SpawnCollisionBlocked;
		}
	}

	return {};
}

FECResult Mgnc::Exec_SpawnActor(AActor*& OutSpawnedActor,
	UWorld& World,
	UClass& ActorClass,
	const FTransform& Transform,
	const FActorSpawnParameters& SpawnParams
	)
{
	OutSpawnedActor = World.SpawnActor(&ActorClass, &Transform, SpawnParams);

	// This error can't be checked during the 'Can' function because it requires the actor to be spawned.
	if (!IsValid(OutSpawnedActor))
	{
		return EECSpawnActorResult::InvalidatedBySpawnNotifications;
	}
	
	return {};
}

FECResult Mgnc::Try_SpawnActor(AActor*& OutSpawnedActor,
	UWorld& World,
	UClass& ActorClass,
	const FTransform& Transform,
	const FActorSpawnParameters& SpawnParams
	)
{
	EC_VALIDATE(Can_SpawnActor(World, ActorClass, Transform, SpawnParams));

	return Exec_SpawnActor(OutSpawnedActor, World, ActorClass, Transform, SpawnParams);
}

FActorSpawnParameters Mgnc::Detail::InitDeferredActorSpawnParams(AActor* Owner,
	APawn* Instigator,
	ESpawnActorCollisionHandlingMethod CollisionHandlingOverride
	)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = CollisionHandlingOverride;
	SpawnParams.Owner = Owner;
	SpawnParams.Instigator = Instigator;
	SpawnParams.bDeferConstruction = true;
	return SpawnParams;
}
