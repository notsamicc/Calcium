#pragma once
#include "util.h"

enum class ESpawnActorNameMode : uint8_t
{
	Required_Fatal,
	Required_ErrorAndReturnNull,
	Required_ReturnNull,
	Requested
};

enum EObjectFlags
{
	RF_NoFlags = 0x00000000,
	RF_Public = 0x00000001,
	RF_Standalone = 0x00000002,
	RF_MarkAsNative = 0x00000004,
	RF_Transactional = 0x00000008,
	RF_ClassDefaultObject = 0x00000010,
	RF_ArchetypeObject = 0x00000020,
	RF_Transient = 0x00000040,
	RF_MarkAsRootSet = 0x00000080,
	RF_TagGarbageTemp = 0x00000100,
	RF_NeedInitialization = 0x00000200,
	RF_NeedLoad = 0x00000400,
	RF_KeepForCooker = 0x00000800,
	RF_NeedPostLoad = 0x00001000,
	RF_NeedPostLoadSubobjects = 0x00002000,
	RF_NewerVersionExists = 0x00004000,
	RF_BeginDestroyed = 0x00008000,
	RF_FinishDestroyed = 0x00010000,
	RF_BeingRegenerated = 0x00020000,
	RF_DefaultSubObject = 0x00040000,
	RF_WasLoaded = 0x00080000,
	RF_TextExportTransient = 0x00100000,
	RF_LoadCompleted = 0x00200000,
	RF_InheritableComponentTemplate = 0x00400000,
	RF_DuplicateTransient = 0x00800000,
	RF_StrongRefOnFrame = 0x01000000,
	RF_NonPIEDuplicateTransient = 0x02000000,
	RF_Dynamic = 0x04000000,
	RF_WillBeLoaded = 0x08000000,
};

enum EClientLoginState
{
	Invalid = 0,
	LoggingIn = 1,
	Welcomed = 2,
	ReceivedJoin = 3,
	CleanedUp = 4
};

enum ELevelCollectionType
{
	DynamicSourceLevels,
	DynamicDuplicatedLevels,
	StaticLevels,
	MAX,
};

enum class EChannelType
{
	CHTYPE_None = 0,
	CHTYPE_Control = 1,
	CHTYPE_Actor = 2,
	CHTYPE_File = 3,
	CHTYPE_Voice = 4,
	CHTYPE_MAX = 8,
};

enum EConnectionState
{
	USOCK_Invalid = 0,
	USOCK_Closed = 1,
	USOCK_Pending = 2,
	USOCK_Open = 3,
};