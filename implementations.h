#pragma once
#include "offsets.h"

inline void ServerEditBuildingActor_Implementation(AFortPlayerController* PC, uint8_t* Params)
{
	auto params = reinterpret_cast<AFortPlayerController_ServerEditBuildingActor_Params*>(Params);
	ReplaceBuildingActor(params->BuildingActorToEdit, 1, params->NewBuildingClass, params->BuildingActorToEdit->CurrentBuildingLevel, params->RotationIterations, params->bMirrored, PC);
}

inline void ServerCreateBuildingActor_Implementation(AFortPlayerController* PC, uint8_t* Params)
{
	auto params = reinterpret_cast<AFortPlayerController_ServerCreateBuildingActor_Params*>(Params);

	auto BuildingActor = SpawnActor<ABuildingSMActor>(params->BuildingClassData.BuildingClass, params->BuildLoc, params->BuildRot);
	BuildingActor->InitializeKismetSpawnedBuildingActor(BuildingActor, PC);
}

void ServerBeginEditingBuildingActor_Implementation(AFortPlayerController* PC, uint8_t* Params)
{
	auto params = reinterpret_cast<AFortPlayerController_ServerBeginEditingBuildingActor_Params*>(Params);
	static auto EditToolDefinition = UObject::FindObject<UFortWeaponItemDefinition>("FortEditToolItemDefinition EditTool.EditTool", 100000);

	auto EditTool = reinterpret_cast<AFortWeap_EditingTool*>(PC->MyFortPawn->EquipWeaponDefinition(EditToolDefinition, RandomGuid()));
	EditTool->EditActor = params->BuildingActorToEdit;
	EditTool->OnRep_EditActor();
}

FGameplayAbilitySpec* FindAbilitySpecFromHandle(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle)
{
	for (int i = 0; i < AbilitySystemComponent->ActivatableAbilities.Items.Num(); i++)
	{
		auto Spec = AbilitySystemComponent->ActivatableAbilities.Items[i];
		if (Spec.Handle.Handle == Handle.Handle)
		{
			return &Spec;
		}
	}

	return nullptr;
}

void ServerTryActivateAbility_Implementation(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle AbilityToActivate, FPredictionKey PredictionKey, const FGameplayEventData* TriggerEventData = 0)
{
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilitySystemComponent, AbilityToActivate);
	if (!Spec)
	{
		AbilitySystemComponent->ClientActivateAbilityFailed(AbilityToActivate, PredictionKey.Current);
		return;
	}

	ConsumeAllReplicatedData(AbilitySystemComponent, AbilityToActivate, PredictionKey);

	UGameplayAbility* InstancedAbility = nullptr;
	Spec->InputPressed = true;

	if (InternalTryActivateAbility(AbilitySystemComponent, AbilityToActivate, PredictionKey, &InstancedAbility, 0, TriggerEventData))
	{

	}
	else
	{
		AbilitySystemComponent->ClientActivateAbilityFailed(AbilityToActivate, PredictionKey.Current);
		Spec->InputPressed = false;
	}

	MarkAbilitySpecDirty(AbilitySystemComponent, *Spec);
}

void ServerAbilityRPCBatch_Implementation(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilities_FServerAbilityRPCBatch& BatchInfo)
{
	ServerTryActivateAbility_Implementation(AbilitySystemComponent, BatchInfo.AbilitySpecHandle, BatchInfo.PredictionKey);
	ServerAbilityRPCBatch_Internal(AbilitySystemComponent, BatchInfo);
}