#pragma once
#include "../stdafx.h"
#include "UnrealClasses.h"
#ifdef _MSC_VER
#pragma pack ( push, 0x4 )
#endif

#define GAME_NAME		"Rocket League"
#define GAME_VERSION	"1.15.0"

#define CLASS_ALIGN		0x4 //4 bytez



//Unreal Engine signatures
#define CallFunction_Pattern	"\x55\x8D\xAC\x24\x00\x00\x00\x00\x81\xEC\x00\x00\x00\x00"
#define CallFunction_Mask		"xxxx????xx????"
#define CallFunction_Offset		0x0

#define ProcessEvent_Pattern	"\x83\xC0\x07\x83\xE0\xF8\xE8\x00\x00\x00\x00\x8B\xC4"
#define ProcessEvent_Mask		"xxxxxxx????xx"
#define ProcessEvent_Offset		0x0

#define GObjects_Pattern		"\xA1\x00\x00\x00\x00\x8B\x00\x00\x8B\x00\x00\x25\x00\x02\x00\x00"
#define GObjects_Mask			"x????x??x??xxxxx"
#define GObjects_Offset			0x1

#define GNames_Pattern			"\x8b\x0d\x00\x00\x00\x00\x83\x3c\x81\x00\x74"
#define GNames_Mask				"xx????xxxxx"
#define GNames_Offset			0x2

//Rest should be untouched?



// Function flags
#define FUNC_Final				0x001
#define FUNC_Latent				0x008				// ???	
#define FUNC_Simulated			0x100				// ???
#define FUNC_Exec				0x200
#define FUNC_Native				0x400
#define FUNC_Event				0x800

//
// /UE3/Development/Src/Core/Inc/UnObjBas.h
// Flags associated with each property in a class, overriding the
// property's default behavior.
//

// For compilers that don't support 64 bit enums.
#define	CPF_Edit				0x0000000000000001		// Property is user-settable in the editor.
#define	CPF_Const				0x0000000000000002		// Actor's property always matches class's default actor property.
#define CPF_Input				0x0000000000000004		// Variable is writable by the input system.
#define CPF_ExportObject		0x0000000000000008		// Object can be exported with actor.
#define CPF_OptionalParm		0x0000000000000010		// Optional parameter (if CPF_Param is set).
#define CPF_Net					0x0000000000000020		// Property is relevant to network replication.
#define CPF_EditConstArray		0x0000000000000040		// Prevent adding/removing of items from dynamic a array in the editor.
#define CPF_Parm				0x0000000000000080		// Function/When call parameter.
#define CPF_OutParm				0x0000000000000100		// Value is copied out after function call.
#define CPF_SkipParm			0x0000000000000200		// Property is a short-circuitable evaluation function parm.
#define CPF_ReturnParm			0x0000000000000400		// Return value.
#define CPF_CoerceParm			0x0000000000000800		// Coerce args into this function parameter.
#define CPF_Native      		0x0000000000001000		// Property is native: C++ code is responsible for serializing it.
#define CPF_Transient   		0x0000000000002000		// Property is transient: shouldn't be saved, zero-filled at load time.
#define CPF_Config      		0x0000000000004000		// Property should be loaded/saved as permanent profile.
#define CPF_Localized   		0x0000000000008000		// Property should be loaded as localizable text.
#define CPF_Travel      		0x0000000000010000		// Property travels across levels/servers.
#define CPF_EditConst   		0x0000000000020000		// Property is uneditable in the editor.
#define CPF_GlobalConfig		0x0000000000040000		// Load config from base class, not subclass.
#define CPF_Component			0x0000000000080000		// Property containts component references.
#define CPF_NeedCtorLink		0x0000000000400000		// Fields need construction/destruction.
#define CPF_NoExport    		0x0000000000800000		// Property should not be exported to the native class header file.
#define CPF_NoClear				0x0000000002000000		// Hide clear (and browse) button.
#define CPF_EditInline			0x0000000004000000		// Edit this object reference inline.
#define CPF_EdFindable			0x0000000008000000		// References are set by clicking on actors in the editor viewports.
#define CPF_EditInlineUse		0x0000000010000000		// EditInline with Use button.
#define CPF_Deprecated  		0x0000000020000000		// Property is deprecated.  Read it from an archive, but don't save it.
#define CPF_EditInlineNotify	0x0000000040000000		// EditInline, notify outer object on editor change.
#define CPF_RepNotify			0x0000000100000000		// Notify actors when a property is replicated
#define CPF_Interp				0x0000000200000000		// interpolatable property for use with matinee
#define CPF_NonTransactional	0x0000000400000000		// Property isn't transacted

// Combinations of flags.
#define	CPF_ParmFlags				(CPF_OptionalParm | CPF_Parm | CPF_OutParm | CPF_SkipParm | CPF_ReturnParm | CPF_CoerceParm)
#define	CPF_PropagateFromStruct		(CPF_Const | CPF_Native | CPF_Transient)
#define	CPF_PropagateToArrayInner	(CPF_ExportObject | CPF_EditInline | CPF_EditInlineUse | CPF_EditInlineNotify | CPF_Localized | CPF_Component)




#ifdef _MSC_VER
#pragma pack ( pop )
#endif