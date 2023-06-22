// Copyright 2022 Miraganic Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ECErrorMacros.h"
#include "ECResult.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "ECActorSpawning.generated.h"

/**
 * Errors related to spawning an actor. These errors mirror the errors from UWorld::SpawnActor.
 */
UENUM(meta = (ErrorCategory))
enum class EECSpawnActorResult : int64
{
	Success = 0,
	// Class cannot be deprecated.
	ClassDeprecated,
	// Class cannot be abstract.
	ClassAbstract,
	// Class must be a child of AActor.
	ClassWasNotActor,
	// Actor class template did not match passed in actor class.
	TemplateClassMismatch,
	// Cannot spawn actor from within a construction script while bAllowDuringConstructionScript is true.
	CalledFromConstructionScript,
	// Cannot spawn an actor while the world is tearing down.
	WorldTearingDown,
	// Spawn transform contained NaN.
	InvalidSpawnTransform,
	// Only one of 'OverridePackage' and 'bCreateActorPackage' can be set. Editor only.
	CannotOverrideAndCreatePackage,
	// Cannot generate unique name for actor in level.
	ObjectNameNotUnique,
	// Cannot spawn actor due to client/server context (is your actor declared client/server only?).
	NetworkContextMismatch,
	// Actor spawn is blocked by another actor's collision.
	SpawnCollisionBlocked,
	// Actor was destroyed by spawn notifications (BeginPlay, etc.).
	InvalidatedBySpawnNotifications,
};

namespace Mgnc
{
	
//----------------------------------------------------------------------------------------------------------------------
// ~ UWorld::SpawnActor

/**
 * Check if a 'UWorld::SpawnActor' call would succeed.
 *
 * NOTE: This does not check whether the actor would be invalidated by spawn notifications, as that requires the actor
 * to actually be spawned.
 */
MIRAGANICERRORHANDLING_API UE_NODISCARD FECResult Can_SpawnActor(
	UWorld& World,
	UClass& ActorClass,
	const FTransform& Transform,
	const FActorSpawnParameters& SpawnParams
);

/**
 * Note: This simply calls UWorld::SpawnActor. It doesn't include performance improvements like other Exec functions.
 */
MIRAGANICERRORHANDLING_API FECResult Exec_SpawnActor(
	AActor*& OutSpawnedActor,
	UWorld& World,
	UClass& ActorClass,
	const FTransform& Transform,
	const FActorSpawnParameters& SpawnParams
);

/**
 * Try to spawn an actor or return the error which occurred.
 */
MIRAGANICERRORHANDLING_API UE_NODISCARD FECResult Try_SpawnActor(AActor*& OutSpawnedActor,
	UWorld& World,
	UClass& ActorClass,
	const FTransform& Transform,
	const FActorSpawnParameters& SpawnParams
	);

//----------------------------------------------------------------------------------------------------------------------
// ~ UWorld::SpawnActor (Templated)

template <typename T>
UE_NODISCARD FECResult Try_SpawnActor(
	T*& OutSpawnedActor,
	UWorld& World,
	UClass& ActorClass,
	const FTransform& Transform,
	const FActorSpawnParameters& SpawnParams
)
{
	static_assert(TIsDerivedFrom<T, AActor>::Value, "T must be derived from AActor.");
	AActor* SpawnedActor = nullptr;
	EC_VALIDATE(Try_SpawnActor(SpawnedActor, World, ActorClass, Transform, SpawnParams));
	OutSpawnedActor = Cast<T>(SpawnedActor);
	return {};
}

template <typename T>
UE_NODISCARD FECResult Can_SpawnActor(
	UWorld& World,
	const FTransform& Transform,
	const FActorSpawnParameters& SpawnParams
)
{
	static_assert(TIsDerivedFrom<T, AActor>::Value, "T must be derived from AActor.");
	return Can_SpawnActor(World, *T::StaticClass(), Transform, SpawnParams);
}

template <typename T>
UE_NODISCARD FECResult Try_SpawnActor(
	T*& OutSpawnedActor,
	UWorld& World,
	const FTransform& Transform,
	const FActorSpawnParameters& SpawnParams
)
{
	static_assert(TIsDerivedFrom<T, AActor>::Value, "T must be derived from AActor.");
	AActor* SpawnedActor = nullptr;
	EC_VALIDATE(Try_SpawnActor(SpawnedActor, World, Transform, SpawnParams));
	OutSpawnedActor = Cast<T>(SpawnedActor);
	return {};
}

//----------------------------------------------------------------------------------------------------------------------
// ~ UWorld::SpawnActorDeferred

namespace Detail
{
MIRAGANICERRORHANDLING_API UE_NODISCARD FActorSpawnParameters InitDeferredActorSpawnParams(
	AActor* Owner,
	APawn* Instigator,
	ESpawnActorCollisionHandlingMethod CollisionHandlingOverride
);
} // namespace Detail

template<typename T>
UE_NODISCARD FECResult Can_SpawnActorDeferred(
	UWorld& World,
	UClass& ActorClass,
	const FTransform& Transform,
	AActor* Owner = nullptr,
	APawn* Instigator = nullptr,
	ESpawnActorCollisionHandlingMethod CollisionHandlingOverride =
	ESpawnActorCollisionHandlingMethod::Undefined
)
{
	static_assert(TIsDerivedFrom<T, AActor>::Value, "T must be derived from AActor.");
	check(!Owner || &World == Owner->GetWorld());
	const FActorSpawnParameters SpawnParams = Detail::InitDeferredActorSpawnParams(Owner, Instigator,
		CollisionHandlingOverride);
	return Can_SpawnActor(World, ActorClass, Transform, SpawnParams);
}

template<typename T>
UE_NODISCARD FECResult Try_SpawnActorDeferred(
	T*& OutSpawnedActor,
	UWorld& World,
	UClass& ActorClass,
	const FTransform& Transform,
	AActor* Owner = nullptr,
	APawn* Instigator = nullptr,
	ESpawnActorCollisionHandlingMethod CollisionHandlingOverride =
	ESpawnActorCollisionHandlingMethod::Undefined
)
{
	static_assert(TIsDerivedFrom<T, AActor>::Value, "T must be derived from AActor.");
	check(!Owner || &World == Owner->GetWorld());
	const FActorSpawnParameters SpawnParams = Detail::InitDeferredActorSpawnParams(Owner, Instigator,
		CollisionHandlingOverride);
	AActor* SpawnedActor = nullptr;
	EC_VALIDATE(Try_SpawnActor(SpawnedActor, World, ActorClass, Transform, SpawnParams));
	OutSpawnedActor = Cast<T>(SpawnedActor);
	return {};
}

template <typename T>
UE_NODISCARD FECResult Exec_SpawnActorDeferred(
	T*& OutSpawnedActor,
	UWorld& World,
	UClass& ActorClass,
	const FTransform& Transform,
	AActor* Owner = nullptr,
	APawn* Instigator = nullptr,
	ESpawnActorCollisionHandlingMethod CollisionHandlingOverride =
	ESpawnActorCollisionHandlingMethod::Undefined
)
{
	static_assert(TIsDerivedFrom<T, AActor>::Value, "T must be derived from AActor.");
	check(!Owner || &World == Owner->GetWorld());
	const FActorSpawnParameters SpawnParams = Detail::InitDeferredActorSpawnParams(Owner, Instigator,
		CollisionHandlingOverride);
	AActor* SpawnedActor = nullptr;
	EC_VALIDATE(Exec_SpawnActor(SpawnedActor, World, ActorClass, Transform, SpawnParams));
	OutSpawnedActor = Cast<T>(SpawnedActor);
	return {};
}
} // namespace Mgnc
