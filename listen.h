#pragma once
#include "core.h"

void Listen(UWorld* World)
{
    UWorld* LastListenedWorld = World;

    AOnlineBeaconHost* OnlineBeaconHost = SpawnActor<AOnlineBeaconHost>(SDK::AOnlineBeaconHost::StaticClass(), FVector{ 0, 0, 0 }, FRotator{ 0, 0, 0 });
    OnlineBeaconHost->ListenPort = 7777;

    if (InitHost(OnlineBeaconHost))
    {
        World->NetDriver = OnlineBeaconHost->NetDriver;
        SetWorld(World->NetDriver, World);
        FLevelCollection* const SourceCollection = FindCollectionByType(World, ELevelCollectionType::DynamicSourceLevels);
        if (SourceCollection)
        {
            SourceCollection->NetDriver = World->NetDriver;
        }
        FLevelCollection* const StaticCollection = FindCollectionByType(World, ELevelCollectionType::StaticLevels);
        if (StaticCollection)
        {
            StaticCollection->NetDriver = World->NetDriver;
        }
        if ((World->NetDriver->MaxInternetClientRate < World->NetDriver->MaxClientRate) && (World->NetDriver->MaxInternetClientRate > 2500))
        {
            World->NetDriver->MaxClientRate = World->NetDriver->MaxInternetClientRate;
        }
    }
}

void Patch()
{
    auto ReservationPatchAddr = Util::FindPattern("48 8B CF E8 3B 6F FF FF 84 C0 75 34 4C 8D");
    DWORD dwProtection;
    VirtualProtect((LPVOID)ReservationPatchAddr, 14, PAGE_EXECUTE_READWRITE, &dwProtection);
    reinterpret_cast<uint8_t*>(ReservationPatchAddr)[10] = 0x74;

    auto ValidationPatchAddr = Util::FindPattern("E8 7F FB FF FF 84 C0 75 34");
    VirtualProtect((LPVOID)ValidationPatchAddr, 9, PAGE_EXECUTE_READWRITE, &dwProtection);
    reinterpret_cast<uint8_t*>(ValidationPatchAddr)[7] = 0x74;
}

void SetClientLoginState_Hook(SDK::UNetConnection* InConnection, const EClientLoginState NewState)
{
    SetClientLoginState(InConnection, NewState);
    if (NewState == EClientLoginState::ReceivedJoin && InConnection != nullptr && InConnection->PlayerController != nullptr)
    {
        SDK::TArray<class SDK::AActor*> actors;
        auto PCA = static_cast<SDK::AFortPlayerController*>(InConnection->PlayerController);
        if (PCA != HostPlayerController)
        {
            auto PSA = reinterpret_cast<AFortPlayerStateAthena*>(PCA->PlayerState);
            PCA->RemoteRole = ENetRole::ROLE_AutonomousProxy;
            PSA->RemoteRole = ENetRole::ROLE_AutonomousProxy;

            TArray<AActor*> outPlayerStarts;
            static auto PlayerStartClass = AFortPlayerStartWarmup::StaticClass();
            GetAllActorsOfClass(GWorld, PlayerStartClass, &outPlayerStarts);

            auto PlayerPawn = SpawnActor<APlayerPawn_Athena_C>(GPlayerPawnClass, outPlayerStarts[PlayerStartIndex]->K2_GetActorLocation(), FRotator{ 0, 0, 0 });
            PlayerStartIndex++;

            PSA->CharacterParts[0] = JonesyHead;
            PSA->CharacterParts[1] = JonesyBody;
            PSA->OnRep_CharacterParts();
            PCA->Possess(PlayerPawn);
            PlayerPawn->bReplicateMovement = true;
            PlayerPawn->RemoteRole = ENetRole::ROLE_AutonomousProxy;
            PlayerPawn->SetMaxHealth(100);
            PlayerPawn->SetMaxShield(100);

            GiveAbilities(PlayerPawn);
            SetupInventory(PCA);

            PSA->bHasFinishedLoading = true;
            PSA->bHasStartedPlaying = true;
            PCA->bHasClientFinishedLoading = true;
            PCA->bHasInitiallySpawned = true;
            PCA->bAssignedStartSpawn = true;
            PCA->bReadyToStartMatch = true;
            PCA->bClientPawnIsLoaded = true;
            PCA->bHasServerFinishedLoading = true;
            PCA->bHasClientFinishedLoading = true;
            PSA->OnRep_bHasStartedPlaying();
            PCA->OnRep_bHasServerFinishedLoading();
            PSA->HeroType = reinterpret_cast<AFortPlayerStateAthena*>(HostPlayerController->PlayerState)->HeroType;
            PSA->OnRep_HeroType();
        }
    }
}