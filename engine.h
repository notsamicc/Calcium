#pragma once
#include "globals.h"

// Client
AActor* (*SpawnActorLong)(UWorld*, UClass*, FVector*, FRotator*, const FActorSpawnParameters& SpawnParameters);
UObject* (*StaticConstructObject)(void*, void*, void*, int, unsigned int, void*, bool, void*, bool);
PVOID(*ProcessEvent)(UObject*, UFunction*, PVOID);
UObject* (*StaticLoadObject)(UObject*, UObject*, const TCHAR*, const TCHAR*, uint32_t, void*, bool);
void (*ReadyToStartMatch_Native)(AGameMode* GameMode);
void (*ServerExecuteInventoryItem_Native)(SDK::AFortPlayerController* Obj, FFrame& Stack);
void (*PCBeginPlay)(SDK::APlayerController*);
char (*GetAllActorsOfClass)(UWorld* WorldContextObject, UClass* ActorClass, TArray<AActor*>* OutActors);
ABuildingSMActor* (*ReplaceBuildingActor)(ABuildingSMActor* BuildingActorToEdit, unsigned int a2, UClass* NewBuildingClass, unsigned int CurrentBuildingLevel, int RotationIterations, bool bMirrored, AFortPlayerController* PC);
void(*ServerEditBuildingActor)(AFortPlayerController* Obj, FFrame& Stack);
void (*ServerCreateBuildingActor)(AFortPlayerController* Obj, FFrame& Stack);
void (*ServerBeginEditingBuildingActor)(SDK::AFortPlayerController* Obj, FFrame& Stack);
void (*RepairEffectAndHealth)(SDK::ABuildingActor* a1, SDK::AFortPlayerController* a2, int a3);
void (*ServerRepairBuildingActor_Validate)(SDK::AFortPlayerController* a1, FFrame& Stack);
__int64 (*RepairSound)(__int64 a1, unsigned __int8 a2);

__forceinline UObject* ConstructObject(UObject* Class, UObject* Outer)
{
    return StaticConstructObject(Class, Outer, 0, 0, 0, 0, 0, 0, 0);
}

template <typename T>
__forceinline T* SpawnActor(UClass* Class, FVector Location, FRotator Rotation)
{
    FActorSpawnParameters params{};
    auto actor = SpawnActorLong(GWorld, Class, &Location, &Rotation, params);
    return reinterpret_cast<T*>(actor);
}

template<typename T>
__forceinline T* LoadObject(const TCHAR* Name)
{
    static auto klass = UObject::FindObject<UClass>("Class CoreUObject.Object");
    return reinterpret_cast<T*>(StaticLoadObject(klass, 0, Name, nullptr, 0, nullptr, false));
}

// Listen
bool (*InitHost)(AOnlineBeaconHost*);
void (*SetWorld)(UNetDriver*, UWorld*);
FLevelCollection* (*FindCollectionByType)(UWorld*, const ELevelCollectionType);
void(*SetClientLoginState)(UNetConnection* InConnection, const EClientLoginState NewState);

// Replication
void(*MarkArrayDirty)(FFastArraySerializer* inASC);
void* (*FindOrAdd)(__int64* NetworkObjectList, AActor* const Actor, const FName NetDriverName, bool* bOutWasAdded);
UChannel* (*CreateChannel)(UNetConnection* InConnection, EChannelType Type, bool bOpenedLocally, int32_t ChannelIndex);
void (*SetChannelActor)(UActorChannel* ActorChannel, AActor* InActor);
void(*CallPreReplication)(AActor* inActor, UNetDriver* NetDriver);
__int64 (*ReplicateActor)(UActorChannel* ActorChannel);
void (*SendClientAdjustment)(APlayerController*);
double (*TickFlush)(__int64*);
void (*NotifyActorDestroyed)(SDK::UNetDriver*, SDK::AActor*, bool);
bool (*IsNetRelevantFor)(AActor* Actor, const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation);
bool (*DestroyActor)(UWorld*, AActor*, bool, bool);
void (*ServerTryActivateAbility)(UAbilitySystemComponent*, FFrame& Stack);
bool (*InternalTryActivateAbility)(UAbilitySystemComponent*, FGameplayAbilitySpecHandle, FPredictionKey, UGameplayAbility**, __int64, const FGameplayEventData*);
void (*ConsumeAllReplicatedData)(UAbilitySystemComponent*, FGameplayAbilitySpecHandle, FPredictionKey);
void (*MarkAbilitySpecDirty)(UAbilitySystemComponent*, FGameplayAbilitySpec&);
void (*ServerAbilityRPCBatch)(UAbilitySystemComponent*, FFrame& Stack);
void (*ServerSetReplicatedTargetData)(UAbilitySystemComponent*, FGameplayAbilitySpecHandle AbilityHandle, FPredictionKey AbilityOriginalPredictionKey, const FGameplayAbilityTargetDataHandle& ReplicatedTargetDataHandle, FGameplayTag ApplicationTag, FPredictionKey CurrentPredictionKey);
UWorld* (*ActorGetWorld)(AActor*);
AWorldSettings* (*GetWorldSettings)(UWorld*, bool);
float (*GetMaxTickRate)(UEngine*, float, bool);
__int64(*ServerAbilityRPCBatch_Internal)(UAbilitySystemComponent*, FGameplayAbilities_FServerAbilityRPCBatch& BatchInfo);
void (*ServerTryActivateAbilityWithEventData)(UAbilitySystemComponent*, FFrame& Stack);
bool (*MarkActive)(__int64 NetworkObjects, AActor* const Actor, UNetConnection* const Connection, const FName NetDriverName);
bool (*DemoReplicateActor)(UNetDriver* NetDriver, AActor* Actor, UNetConnection* Connection, __int64 bMustReplicate);