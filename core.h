#pragma once
#include "implementations.h"

bool bHasLoaded = false;

void UnlockConsole()
{
	auto Console = (SDK::UConsole*)ConstructObject(GEngine->ConsoleClass, GEngine->GameViewport);
	GWorld->OwningGameInstance->LocalPlayers[0]->ViewportClient->ViewportConsole = Console;
}

void SetCheatManager(APlayerController* PC)
{
	PC->CheatManager = (UCheatManager*)ConstructObject(PC->CheatClass, PC);
}

void GiveAbility(UClass* GameplayAbilityClass, AFortPlayerPawn* PlayerPawn)
{
	static auto DefaultGameplayEffect = UObject::FindObject<UGameplayEffect>("GE_Constructor_ContainmentUnit_Applied_C GE_Constructor_ContainmentUnit_Applied.Default__GE_Constructor_ContainmentUnit_Applied_C");
	static auto GameplayEffectClass = LoadObject<UClass>(L"/Game/Abilities/Player/Constructor/Perks/ContainmentUnit/GE_Constructor_ContainmentUnit_Applied.GE_Constructor_ContainmentUnit_Applied_C");

	auto GrantedAbilities = DefaultGameplayEffect->GrantedAbilities;

	GrantedAbilities[0].Ability = GameplayAbilityClass;

	DefaultGameplayEffect->DurationPolicy = EGameplayEffectDurationType::Infinite;

	PlayerPawn->AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(GameplayEffectClass, 1 ,SDK::FGameplayEffectContextHandle());
}

void GiveAbilities(AFortPlayerPawn* PlayerPawn)
{
	static auto AbilitySet = LoadObject<UFortAbilitySet>(L"/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_DefaultPlayer.GAS_DefaultPlayer");
	static auto Abilities = AbilitySet->GameplayAbilities;

	for (int i = 0; i < Abilities.Num(); i++)
	{
		auto CurrentAbility = Abilities[i];
		if (CurrentAbility)
		{
			GiveAbility(CurrentAbility, PlayerPawn);
		}
	}
}

void MarkItemDirty(FFastArraySerializer* inASC, FFastArraySerializerItem& Item)
{
	if (Item.ReplicationID == -1)
	{
		Item.ReplicationID = ++(*(int*)(__int64(inASC) + 80));
		if ((*(int*)(__int64(inASC) + 80)) == -1)
			(*(int*)(__int64(inASC) + 80))++;
	}

	Item.ReplicationKey++;
	MarkArrayDirty(inASC);
}

void AddItem(AFortPlayerController* PC, UFortItemDefinition* ItemDefinition, int Amount, int Slot, EFortQuickBars SlotType, int LoadedAmmo = 0)
{
	auto Item = (UFortWorldItem*)ItemDefinition->CreateTemporaryItemInstanceBP(Amount, 1);

	Item->ItemEntry.Count = Amount;
	
	PC->WorldInventory->Inventory.ReplicatedEntries.Add(Item->ItemEntry);
	PC->HandleWorldInventoryLocalUpdate();
	PC->WorldInventory->HandleInventoryLocalUpdate();
	PC->QuickBars->ServerAddItemInternal(Item->ItemEntry.ItemGuid, SlotType, Slot);
	MarkItemDirty(&PC->WorldInventory->Inventory, Item->ItemEntry);
}

void SetupInventory(AFortPlayerController* PC)
{
	auto Quickbars = SpawnActor<AFortQuickBars>(GQuickBarsClass, FVector{ 0, 0, 0 }, FRotator{ 0, 0,0 });
	static auto StartingItems = reinterpret_cast<AFortGameModeAthena*>(GWorld->AuthorityGameMode)->StartingItems;

	Quickbars->Owner = PC;
	PC->QuickBars = Quickbars;
	PC->OnRep_QuickBar();

	static auto AR = UObject::FindObject<UFortWeaponRangedItemDefinition>("FortWeaponRangedItemDefinition WID_Assault_AutoHigh_Athena_VR_Ore_T03.WID_Assault_AutoHigh_Athena_VR_Ore_T03");
	static auto MediumAmmo = UObject::FindObject<UFortAmmoItemDefinition>("FortAmmoItemDefinition AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium");

	AddItem(PC, GHarvestingToolDefinition, 1, 0, EFortQuickBars::Primary);
	AddItem(PC, AR, 1, 1, EFortQuickBars::Primary, 30);
	AddItem(PC, MediumAmmo, 500, 0, EFortQuickBars::Secondary);
	AddItem(PC, StartingItems[5].Item, 500, 5, EFortQuickBars::Secondary);
	AddItem(PC, StartingItems[0].Item, 1, 0, EFortQuickBars::Secondary);
	AddItem(PC, StartingItems[1].Item, 1, 1, EFortQuickBars::Secondary);
	AddItem(PC, StartingItems[2].Item, 1, 2, EFortQuickBars::Secondary);
	AddItem(PC, StartingItems[3].Item, 1, 3, EFortQuickBars::Secondary);
	AddItem(PC, StartingItems[4].Item, 1, 4, EFortQuickBars::Secondary);

	PC->QuickBars->ServerActivateSlotInternal(EFortQuickBars::Secondary, 0, 0, false);
	PC->QuickBars->ServerActivateSlotInternal(EFortQuickBars::Secondary, 1, 0, false);
	PC->QuickBars->ServerActivateSlotInternal(EFortQuickBars::Secondary, 2, 0, false);
	PC->QuickBars->ServerActivateSlotInternal(EFortQuickBars::Secondary, 3, 0, false);
}

bool Athena(APlayerController* PC)
{
	if (!(PC->Class->GetName().find("Athena_PlayerController_C") != std::string::npos))
		return false;

	Offsets::Update();

	auto KismetLib = UKismetSystemLibrary::StaticClass()->CreateDefaultObject<UKismetSystemLibrary>();

	SDK::UUserWidget* NetDebugUI = SDK::UObject::FindObject<SDK::UUserWidget>("NetDebugUI_C AthenaHUD.WidgetArchetype.WidgetTree_1.NetDebugContainer.WidgetTree_1.NetDebugUI");
	NetDebugUI->RemoveFromViewport();

	auto PCA = (AFortPlayerControllerAthena*)(PC);
	auto PSA = (AFortPlayerStateAthena*)(PCA->PlayerState);
	auto FGSA = (AFortGameStateAthena*)(GWorld->GameState);

	HostPlayerController = PCA;

	KismetLib->STATIC_ExecuteConsoleCommand(GWorld, FString(L"net.maxchannelsize 1000000"), HostPlayerController);

	SetCheatManager(PCA);

	auto Location = FVector{ 0, 0, 0 };
	auto Rotation = FRotator{ 0, 0, 0 };

	GHarvestingToolDefinition = LoadObject<UFortWeaponMeleeItemDefinition>(L"/Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");
	GPlayerPawnClass = LoadObject<UBlueprintGeneratedClass>(L"/Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C");
	GQuickBarsClass = AFortQuickBars::StaticClass();

	auto PlayerPawn = SpawnActor<APlayerPawn_Athena_C>(GPlayerPawnClass, FVector{ 0, 0, 0 }, FRotator{ 0, 0, 0 });

	PCA->Possess(PlayerPawn);

	JonesyHead = LoadObject<UCustomCharacterPart>(L"/Game/Athena/Heroes/Meshes/Heads/M_Med_Soldier_Head_01_ATH.M_Med_Soldier_Head_01_ATH");
	JonesyBody = LoadObject<UCustomCharacterPart>(L"/Game/Athena/Heroes/Meshes/Bodies/CP_005_Athena_Body.CP_005_Athena_Body");

	PSA->CharacterParts[0] = JonesyHead;
	PSA->CharacterParts[1] = JonesyBody;

	PSA->OnRep_CharacterParts();
	GiveAbilities(PlayerPawn);

	SetupInventory(PCA);

	PCA->ServerReadyToStartMatch();
	FGSA->GamePhase = EAthenaGamePhase::None;

	PSA->bHasFinishedLoading = true;
	PSA->bHasStartedPlaying = true;
	PCA->bHasInitiallySpawned = true;
	PCA->bAssignedStartSpawn = true;
	PCA->bReadyToStartMatch = true;
	PCA->bClientPawnIsLoaded = true;
	PCA->bHasServerFinishedLoading = true;
	PCA->bHasClientFinishedLoading = true;

	bHasLoaded = true;
	return true;
}