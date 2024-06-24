#pragma once
#include "enums.h"

using namespace SDK;

UWorld* GWorld;
UEngine* GEngine;
UGameplayStatics* GameplayStatics;
UBlueprintGeneratedClass* GPlayerPawnClass;
APlayerController* HostPlayerController;
UClass* GQuickBarsClass;
UFortWeaponMeleeItemDefinition* GHarvestingToolDefinition;
UCustomCharacterPart* JonesyHead;
UCustomCharacterPart* JonesyBody;
int PlayerStartIndex = 0;

std::vector<UActorChannel*> ActorChannels;
std::vector<AActor*> ActorsToReplicate;

bool bIsTickHooked = false;

typedef int32_t int32;

#define Hook(a,b) DetourTransactionBegin(); DetourUpdateThread(GetCurrentThread()); DetourAttach(&(PVOID&)a, b); DetourTransactionCommit();

__forceinline bool GuidMatches(FGuid A, FGuid B)
{
	return A.A == B.A && A.B == B.B && A.C == B.C && A.D == B.D;
}

struct FActorSpawnParameters
{
	FActorSpawnParameters() : Name(), Template(nullptr), Owner(nullptr), Instigator(nullptr), OverrideLevel(nullptr),
		SpawnCollisionHandlingOverride(), bRemoteOwned(0), bNoFail(0),
		bDeferConstruction(0),
		bAllowDuringConstructionScript(0),
		NameMode(),
		ObjectFlags()
	{
	}
	;

	FName Name;
	UObject* Template;
	UObject* Owner;
	UObject* Instigator;
	UObject* OverrideLevel;
	ESpawnActorCollisionHandlingMethod SpawnCollisionHandlingOverride;

private:
	uint8_t bRemoteOwned : 1;

public:
	bool IsRemoteOwned() const { return bRemoteOwned; }

	uint8_t bNoFail : 1;
	uint8_t bDeferConstruction : 1;
	uint8_t bAllowDuringConstructionScript : 1;
	ESpawnActorNameMode NameMode;
	EObjectFlags ObjectFlags;
};

__forceinline bool IsMatchingGuid(SDK::FGuid A, SDK::FGuid B)
{
	return A.A == B.A && A.B == B.B && A.C == B.C && A.D == B.D;
}

__forceinline FGuid RandomGuid()
{
	FGuid Guid;
	Guid.A = rand() % 1000;
	Guid.B = rand() % 1000;
	Guid.C = rand() % 1000;
	Guid.D = rand() % 1000;

	return Guid;
}

struct TSet
{
	unsigned char Unknown[0x50];
};

struct FNetworkObjectList
{
	unsigned char AllNetworkObjects[0x50];
	unsigned char ActiveNetworkObjects[0x50];
	unsigned char ObjectsDormantOnAllConnections[0x50];

	TMap<TWeakObjectPtr<UNetConnection>, int32> NumDormantObjectsPerConnection;
};

__forceinline bool CompareVector(FVector a, FVector b)
{
	return a.X == b.X && a.Y == b.Y && a.Z == b.Z;
}