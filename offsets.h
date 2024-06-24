#pragma once
#include "engine.h"

namespace Offsets
{
	void Init()
	{
		uintptr_t baseAddress = Util::BaseAddress();

		GWorld = *(UWorld**)(baseAddress + 0x4C6B830);
		GEngine = *(UEngine**)(baseAddress + 0x4C69B70);
		StaticConstructObject = decltype(StaticConstructObject)(baseAddress + 0x14D0610);
		StaticLoadObject = decltype(StaticLoadObject)(baseAddress + 0x14D3080);
		SpawnActorLong = decltype(SpawnActorLong)(baseAddress + 0x2230840);
		ProcessEvent = decltype(ProcessEvent)(baseAddress + 0x14AC100);
		ReadyToStartMatch_Native = decltype(ReadyToStartMatch_Native)(baseAddress + 0x25A5DD0);
		MarkArrayDirty = decltype(MarkArrayDirty)(baseAddress + 0x5229F0);
		ServerExecuteInventoryItem_Native = decltype(ServerExecuteInventoryItem_Native)(baseAddress + 0x118AAA0);
		PCBeginPlay = decltype(PCBeginPlay)(baseAddress + 0x23960C0);
		GetAllActorsOfClass = decltype(GetAllActorsOfClass)(baseAddress + 0x216D050);
		ReplaceBuildingActor = decltype(ReplaceBuildingActor)(baseAddress + 0xAA0950);
		ServerEditBuildingActor = decltype(ServerEditBuildingActor)(baseAddress + 0x118A7A0);
		ServerCreateBuildingActor = decltype(ServerCreateBuildingActor)(baseAddress + 0x118A230);
		ServerBeginEditingBuildingActor = decltype(ServerBeginEditingBuildingActor)(baseAddress + 0x1189CD0);
		ServerRepairBuildingActor_Validate = decltype(ServerRepairBuildingActor_Validate)(baseAddress + 0x118C050);
		RepairEffectAndHealth = decltype(RepairEffectAndHealth)(baseAddress + 0xAA0560);
		RepairSound = decltype(RepairSound)(baseAddress + 0x4119D0);

		// Listen
		InitHost = decltype(InitHost)(baseAddress + 0x340790);
		FindCollectionByType = decltype(FindCollectionByType)(baseAddress + 0x253F530);
		SetWorld = decltype(SetWorld)(baseAddress + 0x229F820);
		SetClientLoginState = decltype(SetClientLoginState)(baseAddress + 0x229EBE0);

		//Replication
		FindOrAdd = decltype(FindOrAdd)(baseAddress + 0x22AE560);
		CreateChannel = decltype(CreateChannel)(baseAddress + 0x228AC00);
		SetChannelActor = decltype(SetChannelActor)(baseAddress + 0x2116CA0);
		CallPreReplication = decltype(CallPreReplication)(baseAddress + 0x1EF6560);
		ReplicateActor = decltype(ReplicateActor)(baseAddress + 0x21122B0);
		SendClientAdjustment = decltype(SendClientAdjustment)(baseAddress + 0x23AEF10);
		IsNetRelevantFor = decltype(IsNetRelevantFor)(baseAddress + 0x1F0A450);
		TickFlush = decltype(TickFlush)(baseAddress + 0x22A0BE0);
		NotifyActorDestroyed = decltype(NotifyActorDestroyed)(baseAddress + 0x2295820);
		DestroyActor = decltype(DestroyActor)(baseAddress + 0x2220640);
		ServerTryActivateAbility = decltype(ServerTryActivateAbility)(baseAddress + 0x586930);
		InternalTryActivateAbility = decltype(InternalTryActivateAbility)(baseAddress + 0x51FBD0);
		ConsumeAllReplicatedData = decltype(ConsumeAllReplicatedData)(baseAddress + 0x50B5D0);
		MarkAbilitySpecDirty = decltype(MarkAbilitySpecDirty)(baseAddress + 0x522970);
		ServerAbilityRPCBatch = decltype(ServerAbilityRPCBatch)(baseAddress + 0x5856A0);
		ServerSetReplicatedTargetData = decltype(ServerSetReplicatedTargetData)(baseAddress + 0x52DB70);
		ActorGetWorld = decltype(ActorGetWorld)(baseAddress + 0x1F08850);
		GetWorldSettings = decltype(GetWorldSettings)(baseAddress + 0x2542300);
		GetMaxTickRate = decltype(GetMaxTickRate)(baseAddress + 0x24F3590);
		ServerAbilityRPCBatch_Internal = decltype(ServerAbilityRPCBatch_Internal)(baseAddress + 0x52CF30);
		ServerTryActivateAbilityWithEventData = decltype(ServerTryActivateAbilityWithEventData)(baseAddress + 0x586A90);
		MarkActive = decltype(MarkActive)(baseAddress + 0x22B7280);
		DemoReplicateActor = decltype(DemoReplicateActor)(baseAddress + 0x2126760);
	}

	void Update()
	{
		uintptr_t baseAddress = Util::BaseAddress();

		GWorld = *(UWorld**)(baseAddress + 0x4C6B830);
		GEngine = *(UEngine**)(baseAddress + 0x4C69B70);
	}
}