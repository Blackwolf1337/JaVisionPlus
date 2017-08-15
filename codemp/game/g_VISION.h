#pragma once //Only once of g_VISION.h

// JaVision+ Definitions in General
//
// functions beginning with v_* are the main code of this gamemod!
// functions beginning with AM_* are additional functions which can be modified any time.
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// *																	   *
// * You may modify v_* functions but proceed with caution, since they'll  *
// * glue this gamemod together											   *
// *																	   *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//
// ____g_VISION.h INCLUDED IN____
//
// ..game/g_local.h
// ______________________________
//
// You can find all the functions which i added in these files:
//
// ..game/g_cmds.c
// ..game/g_svcmds.c
// ..game/g_VISION_account.c
// ..game/g_VISION_cmds.c
// ..game/g_VISION_utils.c
//
// Also keep track of g_VISIOH.h, preventing useless includes.
// Currently it's included only into ..game/g_local.h (No need to spread it around)
// if you need access to most of the structures just include ..game/g_local.h
// and you should be fine.
// _______________________________
//
// Admin data is saved in plain custom format (vision data) in <fs_homepath>/<fs_game>/admins.vd
// Possible key encryption (AES 256)
// --- Definitions --- 

#include "g_local.h"

#define NUL 0x00
#define SOH 0x01
#define STX 0x02
#define ETX 0x03
#define SUB 0x1A
#define FS	0x1C
#define GS	0x1D
#define LF	0x0A

#define VISION_DATA "VisionData.vbin"
#define ACCOUNTSIZE	sizeof( account_t )

// toggle ON, OFF for switch statements and classes
#define ON 1
#define OFF 0
#define FORCE 0
#define MERC 1
#define GHOST 2
#define SLEEP 3
#define GOD 4


//Bitfield
#define PRIV_EMPOWER	(0x0000000000000001u)
#define PRIV_GHOST		(0x0000000000000002u)
#define PRIV_MERC 		(0x0000000000000004u)
#define PRIV_NOCLIP		(0x0000000000000008u)
#define PRIV_SLEEP		(0x0000000000000010u)
#define PRIV_FORCETEAM	(0x0000000000000020u)
#define PRIV_GIVE		(0x0000000000000040u)
#define PRIV_GOD		(0x0000000000000080u)
#define PRIV_GRAVITY	(0x0000000000000100u)
#define PRIV_SPEED		(0x0000000000000200u)
#define PRIV_KICK		(0x0000000000000400u)
#define PRIV_NOTARGET	(0x0000000000000800u)
#define PRIV_KILLVOTE	(0x0000000000001000u)
#define PRIV_NPCSP		(0x0000000000002000u)
#define PRIV_POLL		(0x0000000000004000u)
#define PRIV_PROTECT	(0x0000000000008000u)
#define PRIV_PSAY		(0x0000000000010000u)
#define PRIV_REMAP		(0x0000000000020000u)
#define PRIV_RENAME		(0x0000000000040000u)
#define PRIV_MUTE		(0x0000000000080000u)
#define PRIV_SLAP		(0x0000000000100000u)
#define PRIV_SLAY		(0x0000000000200000u)
#define PRIV_TELEPORT	(0x0000000000400000u)
#define PRIV_WEATHER	(0x0000000000800000u)
#define PRIV_VSTR		(0x0000000001000000u)
#define PRIV_WHOIS		(0x0000000002000000u)
#define PRIV_27			(0x0000000004000000u)
#define PRIV_28			(0x0000000008000000u)
#define PRIV_29			(0x0000000010000000u)
#define PRIV_30			(0x0000000020000000u)
#define PRIV_31			(0x0000000040000000u)
#define PRIV_32			(0x0000000080000000u)
#define PRIV_33			(0x0000000100000000u)
#define PRIV_34			(0x0000000200000000u)
#define PRIV_35			(0x0000000400000000u)
#define PRIV_36			(0x0000000800000000u)
#define PRIV_37			(0x0000001000000000u)
#define PRIV_38			(0x0000002000000000u)
#define PRIV_39			(0x0000004000000000u)
#define PRIV_40			(0x0000008000000000u)
#define PRIV_41			(0x0000010000000000u)
#define PRIV_42			(0x0000020000000000u)
#define PRIV_43			(0x0000040000000000u)
#define PRIV_44			(0x0000080000000000u)
#define PRIV_45			(0x0000100000000000u)
#define PRIV_46			(0x0000200000000000u)
#define PRIV_47			(0x0000400000000000u)
#define PRIV_48			(0x0000800000000000u)
#define PRIV_49			(0x0001000000000000u)
#define PRIV_50			(0x0002000000000000u)
#define PRIV_51			(0x0004000000000000u)
#define PRIV_52			(0x0008000000000000u)
#define PRIV_53			(0x0010000000000000u)
#define PRIV_54			(0x0020000000000000u)
#define PRIV_55			(0x0040000000000000u)
#define PRIV_56			(0x0080000000000000u)
#define PRIV_57			(0x0100000000000000u)
#define PRIV_58			(0x0200000000000000u)
#define PRIV_59			(0x0400000000000000u)
#define PRIV_60			(0x0800000000000000u)
#define PRIV_61			(0x1000000000000000u)
#define PRIV_62			(0x2000000000000000u)
#define PRIV_63			(0x4000000000000000u)
#define PRIV_ACTMGR		(0x8000000000000000u) // Sollte nicht an Spieler vergeben werden!
#define PRIV_NONE		(0xBADC0DED)

//g_VISION_utils.c
#define FINDCL_SUBSTR					(0x0001u)
#define FINDCL_FIRSTMATCH				(0x0002u)
#define FINDCL_CASE						(0x0004u)
#define FINDCL_PRINT					(0x0008u)

#define STRIP_COLOUR	(0x00000001u)
#define STRIP_EXTASCII	(0x00000002u)


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
	int v_loginEffect;

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
	int speed;

	//pokemon bubble
	qboolean invulnerableSpecial;
} VisionPersistent_t;
// ^- in gentity_s


//Account data, shouldn't be touched unless needed to.
//I am probably doing something wrong with the byte datatype, but we'll see...
typedef struct account_s {
	char		v_User[32];		//
	char		v_Password[32];	//memory allocation on the fly
	uint64_t	v_privileges;	//hope i'll find another way around this.
	char		v_rank[3];
	int			v_loginEffect;
	char		v_loginMsg[128];
	char		v_additionalInfo[MAX_STRING_CHARS];	//notes, reminders etc.
	char		v_banned[2];		//if the account is unavailable

	struct account_s *next;		//Pointer to a new struct of a User
} account_t;
// ^- in gentity_s

//This is temporary will be removed in future
typedef struct accountBin_s {
	char		bin_User[32];
	char		bin_Password[32];
	uint64_t	bin_privileges;
	char		bin_rank[3];
	int			bin_loginEffect;
	char		bin_loginMsg[128];
	char		bin_additionalInfo[MAX_STRING_CHARS];
	char		bin_banned[2];
} accountBin_t;

//Commands unified with admin commands.
typedef struct VisionCommand_s {
	const char	*cmd; // command itself (input grabbed from arguments)
	const char	*identifier; // identifier
	uint64_t	v_privileges; // privileges
	void ( *func )( gentity_t *ent ); // function pointer
	qboolean	v_admin_authorization; // admin command y/n?
} VisionCommand_t;


/* * * * * * * * * * * *
	Main Mod Functions
 * * * * * * * * * * * */

#define EXTINFO_CMDS	(0x0001u)
#define EXTINFO_CLIENT	(0x0002u)
#define EXTINFO_ADMIN   (0x0004u)
#define EXTINFO_ALL		(0x0007u)

static struct amInfoSetting_s {
	const char *str;
	uint32_t bit;
} aminfoSettings[] = {
	{ "all", EXTINFO_ALL },
	//{ "saber", EXTINFO_SABER },
	{ "cmds", EXTINFO_CMDS },
	{ "client", EXTINFO_CLIENT },
	{ "admin", EXTINFO_ADMIN },
};
static const size_t numAminfoSettings = ARRAY_LEN(aminfoSettings);

void v_Account_Create( char *user, char *password, uint64_t privileges, char *rank, char *loginEffect, char *loginMsg );	// Need> g_svcmds.c
int  v_Account_Delete( char *user );	// Need> g_svcmds.c
qboolean v_HandleCommands( gentity_t *ent, const char *cmd );	// Need> g_cmds.c
void v_Write_Binary( qboolean silent );	// Need> g_svcmds.c
void v_Read_Binary( qboolean silent );	// Need> g_svcmds.c

/* * * * * * * * * * * 
	   Utilities
 * * * * * * * * * * */

typedef struct printBufferSession_s {
	size_t length, maxLength;
	char *buffer;
	void( *callback )( const char *buffer, int clientNum );
	int clientNum;
} printBufferSession_t;

char *Q_strrep( const char *subject, const char *search, const char *replace );	// Need> g_VISION_cmds.c
void Q_ConvertLinefeeds( char *string ); // Need> g_VISION_cmds.c
qboolean Q_StringIsInteger( const char * s );
int G_ClientFromString( const gentity_t * ent, const char * match, uint32_t flags ); // Need> g_VISION_cmds.c --- replace with ClientNumberFromString ?
void Q_NewPrintBuffer( printBufferSession_t * session, size_t length, void( *callback )( const char *buffer, int clientNum ), int clientNum );
void Q_PrintBuffer( printBufferSession_t * session, const char * append );
void Q_DeletePrintBuffer( printBufferSession_t * session );
void PB_Callback( const char * buffer, int clientNum );
void ClientCleanName( const char *in, char *out, int outSize );
void G_Give( gentity_t *ent, const char *name, const char *args, int argc );
//void G_Knockdown( gentity_t *victim ); // linux no like????
void G_GetDismemberBolt( gentity_t *self, vec3_t boltPoint, int limbType );
void G_Dismember( gentity_t *ent, gentity_t *enemy, vec3_t point, int limbType, float limbRollBase, float limbPitchBase, int deathAnim, qboolean postDeath );
char *ConcatArgs( int start );

/* * * * * * * * * * * *
 General Admin Functions
 * * * * * * * * * * * */

void AM_Logout( gentity_t *ent ); // for the future (force logout)
void Cmd_Noclip_f( gentity_t *ent ); // function from the game itself g_cmds.c
void Cmd_Notarget_f( gentity_t *ent ); // "" "" "" "" "" g_cmds.c
void Cmd_NPC_f( gentity_t *ent );
void Toggle_Func( gentity_t *ent, int power, int toggle );

/* * * * * * * * * * * *
		Struct
 * * * * * * * * * * * */

account_t *accounts;	//This shouldn't be modified.
VisionCommand_t *command;