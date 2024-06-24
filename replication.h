#pragma once
#include "listen.h"

int32 ServerReplicateActors_PrepConnections(UNetDriver* NetDriver, const float DeltaSeconds)
{
	int32 NumClientsToTick = NetDriver->ClientConnections.Num();

	bool bFoundReadyConnection = false;

	for (int32 ConnIdx = 0; ConnIdx < NetDriver->ClientConnections.Num(); ConnIdx++)
	{
		UNetConnection* Connection = NetDriver->ClientConnections[ConnIdx];

		AActor* OwningActor = Connection->OwningActor;
		auto ConnectionState = *(DWORD*)(__int64(Connection) + 0x12C);
		if (OwningActor != NULL && ConnectionState == EConnectionState::USOCK_Open && (Connection->Driver->Time - Connection->LastReceiveTime < 1.5f))
		{
			bFoundReadyConnection = true;

			AActor* DesiredViewTarget = OwningActor;
			if (Connection->PlayerController)
			{
				if (AActor* ViewTarget = Connection->PlayerController->GetViewTarget())
				{
					if (ActorGetWorld(ViewTarget))
					{
						DesiredViewTarget = ViewTarget;
					}
					else
					{
						std::cout << "Player controller %s's view target (%s) no longer has a valid world! Was it unloaded as part a level instance?" << std::endl;
					}
				}
			}
			Connection->ViewTarget = DesiredViewTarget;

			for (int32 ChildIdx = 0; ChildIdx < Connection->Children.Num(); ChildIdx++)
			{
				UNetConnection* Child = Connection->Children[ChildIdx];
				APlayerController* ChildPlayerController = Child->PlayerController;
				if (ChildPlayerController != NULL)
				{
					Child->ViewTarget = ChildPlayerController->GetViewTarget();
				}
				else
				{
					Child->ViewTarget = NULL;
				}
			}
		}
		else
		{
			Connection->ViewTarget = NULL;
			for (int32 ChildIdx = 0; ChildIdx < Connection->Children.Num(); ChildIdx++)
			{
				Connection->Children[ChildIdx]->ViewTarget = NULL;
			}
		}
	}

	return bFoundReadyConnection ? NumClientsToTick : 0;
}

void ServerReplicateActors_BuildConsiderList(UNetDriver* NetDriver, TArray<FNetworkObjectInfo*>& OutConsiderList, const float ServerTickTime)
{
	int32 NumInitiallyDormant = 0;

	TArray<AActor*> ActorsToRemove;

	//for ()
	//{
	//
	//}
}

int32 ServerReplicateActors(UNetDriver* NetDriver, float DeltaSeconds)
{
	if (NetDriver->ClientConnections.Num() == 0)
	{
		return 0;
	}

	auto ReplicationFrame = *(DWORD*)(__int64(NetDriver) + 0x330);
	ReplicationFrame++;

	int32 Updated = 0;
	
	const int32 NumClientsToTick = ServerReplicateActors_PrepConnections(NetDriver, DeltaSeconds);

	if (NumClientsToTick == 0)
	{
		return 0;
	}

	AWorldSettings* WorldSettings = GetWorldSettings(NetDriver->World, false);	// Double check

	bool bCPUSaturated = false;
	float ServerTickTime = GetMaxTickRate(GEngine, DeltaSeconds, true);
	if (ServerTickTime == 0.f)
	{
		ServerTickTime = DeltaSeconds;
	}
	else
	{
		ServerTickTime = 1.f / ServerTickTime;
		bCPUSaturated = DeltaSeconds > 1.2f * ServerTickTime;
	}

	TArray<FNetworkObjectInfo*> ConsiderList;
	auto NetworkObjects = *(__int64*)(__int64(NetDriver) + 1288);
	auto ActiveNetworkObjects = *(TSet*)(__int64(NetworkObjects) + 80);
	auto ActiveObjectsNum = *(DWORD*)(__int64(&ActiveNetworkObjects) + 8) - *(DWORD*)(__int64(NetworkObjects) + 132);
	std::cout << ActiveObjectsNum << std::endl;
	//ConsiderList.Reserve(ActiveObjectsNum);

	ServerReplicateActors_BuildConsiderList(NetDriver, ConsiderList, ServerTickTime);
}

UActorChannel* FindActorChannel(UNetConnection* Connection, AActor* Actor)
{
	for (int i = 0; i < ActorChannels.size(); i++)
	{
		auto ActorChannel = ActorChannels.operator[](i);
		if (ActorChannel->Actor == Actor && ActorChannel->Connection == Connection)
		{
			return ActorChannel;
		}
	}

	return nullptr;
}

void* FindOrAdd_Hook(__int64* NetworkObjectList, SDK::AActor* const Actor, const SDK::FName NetDriverName, bool* bOutWasAdded)
{
	for (int i = 0; i < ActorsToReplicate.size(); i++)
	{
		if (ActorsToReplicate.operator[](i) == Actor)
		{
			return FindOrAdd(NetworkObjectList, Actor, NetDriverName, bOutWasAdded);
		}
	}

	ActorsToReplicate.push_back(Actor);
	return FindOrAdd(NetworkObjectList, Actor, NetDriverName, bOutWasAdded);
}

void ReplicateActors(UNetDriver* NetDriver)
{
	auto ReplicationFrame = *(DWORD*)(__int64(NetDriver) + 0x330);
	ReplicationFrame++;

	for (int i = 0; i < NetDriver->ClientConnections.Num(); i++)
	{
		auto Connection = NetDriver->ClientConnections[i];
		if (Connection)
		{
			for (int i = 0; i < ActorsToReplicate.size(); i++)
			{
				auto Actor = ActorsToReplicate.operator[](i);
				if (Actor)
				{
					if (Actor->bReplicates == true && Actor->NetDormancy != ENetDormancy::DORM_Initial)
					{
						SDK::UActorChannel* ActorChannel = FindActorChannel(Connection, ActorsToReplicate.operator[](i));
						if (!ActorChannel)
						{
							ActorChannel = (UActorChannel*)CreateChannel(Connection, EChannelType::CHTYPE_Actor, 1, -1);
						
							SetChannelActor(ActorChannel, ActorsToReplicate.operator[](i));
							ActorChannels.push_back(ActorChannel);
						}
			
						if (IsNetRelevantFor(ActorsToReplicate.operator[](i), GWorld->AuthorityGameMode, GWorld->AuthorityGameMode, GWorld->AuthorityGameMode->K2_GetActorLocation()))
						{
							CallPreReplication(ActorsToReplicate.operator[](i), NetDriver);
							DemoReplicateActor(NetDriver, ActorsToReplicate.operator[](i), Connection, 0);
						}
					}
				}
		
				if (Connection->PlayerController)
				{
					SendClientAdjustment(Connection->PlayerController);
				}
			}

			for (int i = 0; i < ActorChannels.size(); i++)
			{
				if (ActorChannels.operator[](i) && ActorChannels.operator[](i)->Actor)
				{
					SetChannelActor(ActorChannels.operator[](i), ActorChannels.operator[](i)->Actor);
					if (ActorChannels.operator[](i)->Actor->bReplicates == true && ActorChannels.operator[](i)->Actor->RemoteRole != SDK::ENetRole::ROLE_None)
					{
						if (IsNetRelevantFor(ActorChannels.operator[](i)->Actor, GWorld->AuthorityGameMode, GWorld->AuthorityGameMode, GWorld->AuthorityGameMode->K2_GetActorLocation()))
						{
							std::cout << ActorChannels.operator[](i)->Actor->GetName() << std::endl;
							CallPreReplication(ActorChannels.operator[](i)->Actor, NetDriver);
							ReplicateActor(ActorChannels.operator[](i));
						}
					}
				}
			
				if (Connection->PlayerController)
				{
					SendClientAdjustment(Connection->PlayerController);
				}
			}

		}
	}
}

bool DestroyActor_Hook(UWorld* World, AActor* Actor, bool bNetForce, bool bShouldModifyLevel)
{
	if (bHasLoaded == true && World->NetDriver->ClientConnections.Num() > 0)
	{
		for (int i = 0; i < ActorsToReplicate.size(); i++)
		{
			if (ActorsToReplicate.operator[](i) == Actor)
			{
				ActorsToReplicate.erase(ActorsToReplicate.begin() + i);
				break;
			}
		}

		NotifyActorDestroyed(World->NetDriver, Actor, false);
	}

	return DestroyActor(World, Actor, bNetForce, bShouldModifyLevel);
}

double TickFlush_Hook(__int64* NetDriver)
{
	ReplicateActors((UNetDriver*)NetDriver);
	return TickFlush(NetDriver);
}

bool MarkActive_Hook(__int64 NetworkObjects, AActor* const Actor, UNetConnection* const Connection, const FName NetDriverName)
{
	auto ActorChannel = (UActorChannel*)CreateChannel(Connection, EChannelType::CHTYPE_Actor, 1, -1);
	
	if (Actor)
	{
		ActorChannel->Actor = Actor;
		ActorChannels.push_back(ActorChannel);
	}
	return MarkActive(NetworkObjects, Actor, Connection, NetDriverName);
}