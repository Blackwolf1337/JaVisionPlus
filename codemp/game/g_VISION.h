#pragma once //Only once of g_VISION.h

// JaVision+ Definitions in General
//
// functions beginning with v_* are the main code of this gamemod!
// functions beginning with AM_* are additional functions which can be modified any time.
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * You may modify v_* functions but proceed with caution, since they'll  *
// * glue this gamemod together											   *
// *																	   *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//
// ____INCLUDED IN____
// ..game/g_cmds.c
// ..game/g_svcmds.c
// ..game/g_VISION_acchandler.c
// ..game/g_VISION_account.c
// ..game/g_VISION_cmds.c
// ___________________
//
// Keep it tracked, preventing useless includes.
// ___________________
//
// Admin data is saved in plain custom format (vision data) in <fs_homepath>/<fs_game>/admins.vd
// Possible key encryption (AES 256)
// --- Definitions --- 

#include "qcommon/q_shared.h"

#define NUL 0x00
#define SOH 0x01
#define STX 0x02
#define ETX 0x03
#define SUB 0x1A
#define FS	0x1C
#define GS	0x1D
#define LF	0x0A

#define VISION_DATA "VisionData.vbin"
#define ACCOUNTSIZE	sizeof(account_t)

//Copied from Ja++ old fork
//Don't be mad ples, will be cleaned up.
typedef struct VisionPersistent_s {
	qboolean silenced; // have they been silenced?
	qboolean isGhost; // are they a ghost?
	qboolean isSlept; // are they frozen?
	qboolean isFrozen; // are they frozen?
	qboolean isGod; // are they invincible? (Caution: This prevents also Bot AI)
	qboolean isListening; //Spying on TELL
	qboolean isListeningC; //Spying on TELL with Chat output.
	qboolean isDev; // see execution of commands
	qboolean isTag; // is with Admin tag
	qboolean isMonked;
	qboolean hasExtraStyles; // Extra Saber Styles
	qboolean empowered; // are they empowered?
	qboolean merc; // are they merced?
	qboolean icon; // icon enabled?
	int renamedTime; // level.time they were renamed
	
					 //saving for amsaber
	int forcePowerLevelSaber[NUM_FORCE_POWERS];

	// saving these for amempower
	int forcePowersKnown;
	int forcePowerBaseLevel[NUM_FORCE_POWERS];
	int forcePowerLevel[NUM_FORCE_POWERS];

	// saving these for ammerc
	int forcePowerMax;

	// saving Gravity for g_active.cpp
	int gravity;
	qboolean qgravity;
} VisionPersistent_t;


//Account data, shouldn't be touched unless needed to.
//I am probably doing something wrong with the byte datatype, but we'll see...
typedef struct account_s {
	char		v_User[32];		//
	char		v_Password[32];	//memory allocation on the fly
	uint64_t	v_privileges;	//hope i'll find another way around this.
	char		v_rank[3];
	char		v_loginEffect[3];
	char		v_loginMsg[128];
	char		v_additionalInfo[MAX_STRING_CHARS];	//notes, reminders etc.
	char		v_banned[2];		//if the account is unavailable

	struct account_s *next;		//Pointer to a new struct of a User
} account_t;

typedef struct accountBin_s {
	char		bin_User[32];
	char		bin_Password[32];
	uint64_t	bin_privileges;
	char		bin_rank[3];
	char		bin_loginEffect[3];
	char		bin_loginMsg[128];
	char		bin_additionalInfo[MAX_STRING_CHARS];
	char		bin_banned[2];
} accountBin_t;

//Commands unified with admin commands.
typedef struct VisionCommand_s {
	const char	*cmd;
	uint64_t	v_priv_id;
	void( *func )( gentity_t *ent );
	qboolean	v_admin_authorization;
} VisionCommand_t;

//Main Mod Functions
void v_Account_Create( char *user, char *password, uint64_t privileges, char *rank, char *loginEffect, char *loginMsg );
void v_memfree_Accounts( void );
qboolean v_HandleCommands( gentity_t *ent, const char *cmd );
void v_Write_Binary( qboolean silent );
void v_Read_Binary( qboolean silent );
//Side Functions
void AM_Login( gentity_t *ent );
void AM_Logout( gentity_t *ent );
//Header access 
static account_t *accounts = NULL;	//This shouldn't be modified.