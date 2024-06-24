#pragma once
#include "replication.h"

void PCBeginPlay_Hook(SDK::APlayerController* PC)
{
	PCBeginPlay(PC);

	if (!bHasLoaded)
	{
		if (Athena(PC))
		{
			Listen(GWorld);
		}
	}
}

void ServerExecuteInventoryItem_Hook(AFortPlayerController* Obj, FFrame& Stack)
{
	FGuid* ParamGuid = reinterpret_cast<FGuid*>(Stack.Locals);
	auto PC = reinterpret_cast<AFortPlayerController*>(Obj);

	auto ItemEntries = PC->WorldInventory->Inventory.ReplicatedEntries;
	for (int i = 0; i != ItemEntries.Num(); i++)
	{
		if (IsMatchingGuid(ItemEntries[i].ItemGuid, (*ParamGuid)))
		{
			PC->MyFortPawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)ItemEntries[i].ItemDefinition, *ParamGuid);
			break;
		}
	}
}

void ServerEditBuildingActor_Hook(AFortPlayerController* PC, FFrame& Stack)
{
	ServerEditBuildingActor_Implementation(PC, Stack.Locals);
}

void ServerCreateBuildingActor_Hook(AFortPlayerController* PC, FFrame& Stack)
{
	ServerCreateBuildingActor_Implementation(PC, Stack.Locals);
}

void ServerBeginEditingBuildingActor_Hook(AFortPlayerController* PC, FFrame& Stack)
{
	ServerBeginEditingBuildingActor_Implementation(PC, Stack.Locals);
}

void ServerTryActivateAbility_Hook(UAbilitySystemComponent* AbilitySystemComponent, FFrame& Stack)
{
	auto params = reinterpret_cast<UAbilitySystemComponent_ServerTryActivateAbility_Params*>(Stack.Locals);
	ServerTryActivateAbility_Implementation(AbilitySystemComponent, params->AbilityToActivate, params->PredictionKey, 0);
}

void ServerAbilityRPCBatch_Hook(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilities_FServerAbilityRPCBatch& BatchInfo)
{
	ServerAbilityRPCBatch_Implementation(AbilitySystemComponent, BatchInfo);
}

void ServerTryActivateAbilityWithEventData_Hook(UAbilitySystemComponent* AbilitySystemComponent, uint8_t* Params)
{
	auto params = reinterpret_cast<UAbilitySystemComponent_ServerTryActivateAbilityWithEventData_Params*>(Params);
	ServerTryActivateAbility_Implementation(AbilitySystemComponent, params->AbilityToActivate, params->PredictionKey, &params->TriggerEventData);
}

static std::vector<std::string> cache;
PVOID ProcessEventHook(SDK::UObject* Object, SDK::UFunction* Function, PVOID Params)
{
	if (Object && Function)
	{
		std::string sValue = "Object = " + Object->GetName() + ", Function = " + Function->GetName();

		if (std::find(cache.begin(), cache.end(), sValue) == cache.end())
		{
			printf("%s\n", sValue.c_str());

			cache.push_back(sValue);
		}
	}

	return ProcessEvent(Object, Function, Params);
}

void Build_Hook(UObject* A, __int64* B, __int64 C)
{
	std::cout << A->GetFullName() << std::endl;
	return;
}

void ServerRepairBuildingActor_Hook(SDK::AFortPlayerController* PlayerController, FFrame& Stack)
{
	auto params = reinterpret_cast<AFortPlayerController_ServerRepairBuildingActor_Params*>(Stack.Locals);

	static auto RepairSound = SDK::UObject::FindObject<SDK::USoundCue>("SoundCue Fort_Build_RepairStart_Cue.Fort_Build_RepairStart_Cue");
	RepairEffectAndHealth(params->BuildingActorToRepair, PlayerController, 0);
	PlayerController->ClientPlaySoundAtLocation(RepairSound, params->BuildingActorToRepair->K2_GetActorLocation(), 1, 1);
}

namespace Hooks
{
	void Init()
	{
		Hook(FindOrAdd, FindOrAdd_Hook);
		Hook(PCBeginPlay, PCBeginPlay_Hook);
		Hook(ServerExecuteInventoryItem_Native, ServerExecuteInventoryItem_Hook);
		Hook(SetClientLoginState, SetClientLoginState_Hook);
		Hook(ServerEditBuildingActor, ServerEditBuildingActor_Hook);
		Hook(ServerCreateBuildingActor, ServerCreateBuildingActor_Hook);
		Hook(ServerBeginEditingBuildingActor, ServerBeginEditingBuildingActor_Hook);
		Hook(DestroyActor, DestroyActor_Hook);
		Hook(ServerTryActivateAbility, ServerTryActivateAbility_Hook);
		Hook(ServerAbilityRPCBatch_Internal, ServerAbilityRPCBatch_Hook);
		Hook(ServerTryActivateAbilityWithEventData, ServerTryActivateAbilityWithEventData_Hook);
		Hook(ServerRepairBuildingActor_Validate, ServerRepairBuildingActor_Hook);
		//Hook(MarkActive, MarkActive_Hook);
	}
}