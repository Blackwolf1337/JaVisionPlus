// JaVision+ administration system
//
// These functions are mostly copied from Ja++, credits are given.
// Functions will be optimized and will be well documented in the future.
// 
//
//
//
//
//
// Admin data is saved in plain custom format (vision data) in <fs_homepath>/<fs_game>/admins.vd
// Possible key encryption (AES 256)
// --- Functionality ---

#include "g_local.h"

void Toggle_Func( gentity_t *ent, int power, int toggle ) {
	int i;

	if (power == FORCE) 
	{
		switch (toggle) 
		{
		case ON:
			ent->client->ps.fd.forcePowerSelected = 0; // HACK: What the actual fuck
			ent->client->ps.eFlags |= EF_BODYPUSH;

			ent->client->pers.vPersistent.forcePowersKnown = ent->client->ps.fd.forcePowersKnown;

			for (i = 0; i < NUM_FORCE_POWERS; i++) {
				ent->client->pers.vPersistent.forcePowerBaseLevel[i] = ent->client->ps.fd.forcePowerBaseLevel[i];
				ent->client->ps.fd.forcePowerBaseLevel[i] = 3;
				ent->client->pers.vPersistent.forcePowerLevel[i] = ent->client->ps.fd.forcePowerLevel[i];
				ent->client->ps.fd.forcePowerLevel[i] = 3;
				ent->client->ps.fd.forcePowersKnown |= (1 << i);
			}
		break;

		case OFF:
			ent->client->ps.fd.forcePowerSelected = 0; // HACK: What the actual fuck
			ent->client->ps.eFlags &= ~EF_BODYPUSH;

			ent->client->ps.fd.forcePowersKnown = ent->client->pers.vPersistent.forcePowersKnown;

			for (i = 0; i < NUM_FORCE_POWERS; i++) {
				ent->client->ps.fd.forcePowerBaseLevel[i] = ent->client->pers.vPersistent.forcePowerBaseLevel[i];
				ent->client->ps.fd.forcePowerLevel[i] = ent->client->pers.vPersistent.forcePowerLevel[i];
			}
		break;
		}
	}
	
	else if (power == MERC) 
	{
		switch (toggle) 
		{
		case ON:
			// save forcepower data because we strip them + revert
			for (int fp = 0; fp < NUM_FORCE_POWERS; fp++) {
				ent->client->pers.vPersistent.forcePowerBaseLevel[fp] = ent->client->ps.fd.forcePowerBaseLevel[fp];
				ent->client->pers.vPersistent.forcePowerLevel[fp] = ent->client->ps.fd.forcePowerLevel[fp];
				if (ent->client->ps.fd.forcePowersActive & (1 << fp)) {
					WP_ForcePowerStop(ent, (forcePowers_t)fp);
				}
				ent->client->ps.holocronsCarried[fp] = 0;
				ent->client->ps.fd.forcePowerDebounce[fp] = 0;
				ent->client->ps.fd.forcePowerDuration[fp] = 0;
			}
			ent->client->ps.fd.forceDeactivateAll = 0;
			ent->client->pers.vPersistent.forcePowerMax = ent->client->ps.fd.forcePowerMax;
			ent->client->ps.fd.forcePowerMax = ent->client->ps.fd.forcePower = 0;
			ent->client->ps.fd.forcePowerRegenDebounceTime = level.time;
			ent->client->pers.vPersistent.forcePowersKnown = ent->client->ps.fd.forcePowersKnown;
			ent->client->ps.fd.forcePowersKnown = 0;
			ent->client->ps.fd.forceGripEntityNum = ENTITYNUM_NONE;
			ent->client->ps.fd.forceMindtrickTargetIndex  = 0u;
			ent->client->ps.fd.forceMindtrickTargetIndex2 = 0u;
			ent->client->ps.fd.forceMindtrickTargetIndex3 = 0u;
			ent->client->ps.fd.forceMindtrickTargetIndex4 = 0u;
			ent->client->ps.fd.forceJumpZStart = 0;
			ent->client->ps.fd.forceJumpCharge = 0;
			ent->client->ps.fd.forceJumpSound = 0;
			ent->client->ps.fd.forceGripDamageDebounceTime = 0;
			ent->client->ps.fd.forceGripBeingGripped = 0;
			ent->client->ps.fd.forceGripCripple = 0;
			ent->client->ps.fd.forceGripUseTime = 0;
			ent->client->ps.fd.forceGripSoundTime = 0;
			ent->client->ps.fd.forceGripStarted = 0;
			ent->client->ps.fd.forceHealTime = 0;
			ent->client->ps.fd.forceHealAmount = 0;
			ent->client->ps.fd.forceRageRecoveryTime = 0;
			ent->client->ps.fd.forceDrainEntNum = ENTITYNUM_NONE;
			ent->client->ps.fd.forceDrainTime = 0;
			ent->client->ps.holocronBits = 0;
			ent->client->ps.saberAttackChainCount = 0;

			ent->client->ps.stats[STAT_WEAPONS] = ((1 << LAST_USEABLE_WEAPON) - 1) & ~1;
			ent->client->ps.stats[STAT_WEAPONS] &= ~(1 << WP_SABER);
			for (int i = 0; i < AMMO_MAX; i++) {
				ent->client->ps.ammo[i] = ammoData[i].max;
			}
			ent->client->ps.stats[STAT_HOLDABLE_ITEMS] = ((1 << HI_NUM_HOLDABLE) - 1) & ~1;
			ent->client->ps.stats[STAT_HOLDABLE_ITEM] = HI_NONE + 1;

			if (ent->client->pers.jetpack != qtrue)
				ent->client->ps.stats[STAT_HOLDABLE_ITEMS] &= ~(1 << HI_JETPACK);
		break;

		case OFF:
			// revert forcepower state
			ent->client->ps.fd.forcePowerSelected = 0; // HACK: What the actual fuck

			for (int fp = 0; fp < NUM_FORCE_POWERS; fp++) {
				ent->client->ps.fd.forcePowerBaseLevel[fp] = ent->client->pers.vPersistent.forcePowerBaseLevel[fp];
				ent->client->ps.fd.forcePowerLevel[fp] = ent->client->pers.vPersistent.forcePowerLevel[fp];
				if (ent->client->ps.fd.forcePowersActive & (1 << fp)) {
					WP_ForcePowerStop(ent, (forcePowers_t)fp);
				}
				ent->client->ps.holocronsCarried[fp] = 0;
				ent->client->ps.fd.forcePowerDebounce[fp] = 0;
				ent->client->ps.fd.forcePowerDuration[fp] = 0;
			}
			ent->client->ps.fd.forceDeactivateAll = 0;
			ent->client->ps.fd.forcePower = ent->client->ps.fd.forcePowerMax = 100;
			ent->client->ps.fd.forcePowersKnown = ent->client->pers.vPersistent.forcePowersKnown;
			ent->client->ps.fd.forcePowerRegenDebounceTime = level.time;
			ent->client->ps.fd.forceGripEntityNum = ENTITYNUM_NONE;
			ent->client->ps.fd.forceMindtrickTargetIndex = 0u;
			ent->client->ps.fd.forceMindtrickTargetIndex2 = 0u;
			ent->client->ps.fd.forceMindtrickTargetIndex3 = 0u;
			ent->client->ps.fd.forceMindtrickTargetIndex4 = 0u;
			ent->client->ps.fd.forceJumpZStart = 0;
			ent->client->ps.fd.forceJumpCharge = 0;
			ent->client->ps.fd.forceJumpSound = 0;
			ent->client->ps.fd.forceGripDamageDebounceTime = 0;
			ent->client->ps.fd.forceGripBeingGripped = 0;
			ent->client->ps.fd.forceGripCripple = 0;
			ent->client->ps.fd.forceGripUseTime = 0;
			ent->client->ps.fd.forceGripSoundTime = 0;
			ent->client->ps.fd.forceGripStarted = 0;
			ent->client->ps.fd.forceHealTime = 0;
			ent->client->ps.fd.forceHealAmount = 0;
			ent->client->ps.fd.forceRageRecoveryTime = 0;
			ent->client->ps.fd.forceDrainEntNum = ENTITYNUM_NONE;
			ent->client->ps.fd.forceDrainTime = 0;
			ent->client->ps.holocronBits = 0;
			ent->client->ps.saberAttackChainCount = 0;

			//ent->client->ps.stats[STAT_WEAPONS] = japp_spawnWeaps.integer;
			//ent->client->ps.stats[STAT_HOLDABLE_ITEMS] = japp_spawnItems.integer;
			ent->client->ps.stats[STAT_WEAPONS] = 0;
			ent->client->ps.stats[STAT_HOLDABLE_ITEMS] = 0;
			uint32_t x = ent->client->ps.stats[STAT_HOLDABLE_ITEMS];
			// get the right-most bit
			x &= -x;
			// log2n of x is array index of bit-value
			x = (x >= 1000000000)
				? 9 : (x >= 100000000)
				? 8 : (x >= 10000000)
				? 7 : (x >= 1000000)
				? 6 : (x >= 100000)
				? 5 : (x >= 10000)
				? 4 : (x >= 1000)
				? 3 : (x >= 100)
				? 2 : (x >= 10)
				? 1 : 0;
			ent->client->ps.stats[STAT_HOLDABLE_ITEM] = x;

			// select the first available weapon
			int newWeap = -1;
			for (int i = WP_SABER; i < WP_NUM_WEAPONS; i++) {
				if ((ent->client->ps.stats[STAT_WEAPONS] & (1 << i))) {
					newWeap = i;
					break;
				}
			}

			if (newWeap == WP_NUM_WEAPONS) {
				for (int i = WP_STUN_BATON; i < WP_SABER; i++) {
					if ((ent->client->ps.stats[STAT_WEAPONS] & (1 << i))) {
						newWeap = i;
						break;
					}
				}
				if (newWeap == WP_SABER) {
					newWeap = WP_NONE;
				}
			}

			weapon_t wp = (weapon_t)ent->client->ps.weapon;
			if (newWeap != -1) {
				ent->client->ps.weapon = newWeap;
			}
			else {
				ent->client->ps.weapon = WP_NONE;
			}

			if (ent->client->ps.weapon != WP_SABER) {
				G_AddEvent(ent, EV_NOAMMO, wp);
			}
		break;
		}
	}

	else if (power == GHOST)
	{
		switch (toggle)
		{
		case ON:
			ent->client->pers.vPersistent.isGhost = qtrue;
			ent->r.contents = CONTENTS_BODY;
			ent->clipmask = 267009/*CONTENTS_SOLID*/;
			ent->client->ps.fd.forcePowersKnown |= (1 << NUM_FORCE_POWERS); // JA++ client prediction
		break;

		case OFF:
			ent->client->pers.vPersistent.isGhost = qfalse;
			ent->r.contents = CONTENTS_SOLID;
			ent->clipmask = CONTENTS_SOLID | CONTENTS_BODY;
			ent->client->ps.fd.forcePowersKnown &= ~(1 << NUM_FORCE_POWERS); // JA++ client prediction
		break;
		}
	}
}

static void AM_Empower_f(gentity_t *ent) {
	char arg1[64] = { 0 };
	int targetClient;
	gentity_t *targ;

	// can empower: self, partial name, clientNum
	trap->Argv(1, arg1, sizeof(arg1));
	if (ent) {
		targetClient = (trap->Argc() > 1) ? G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT) : ent - g_entities;
	}
	else {
		targetClient = (trap->Argc() > 1) ? G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT) : -1;
	}

	if (targetClient == -1) {
		return;
	}

	targ = &g_entities[targetClient];

	/*if (!AM_CanInflict(ent, targ)) {
	return;
	}*/

	targ->client->pers.vPersistent.empowered = !targ->client->pers.vPersistent.empowered;
	if (targ->client->pers.vPersistent.empowered) {
		Toggle_Func(targ, FORCE, ON);
		//Draw string bleh
	}
	else {
		Toggle_Func(targ, FORCE, OFF);
		//Draw string blehbleh
	}
}

// ghost specified client (or self)
static void AM_Ghost_f(gentity_t *ent) {
	char arg1[64] = { 0 };
	gentity_t *targ = NULL;
	int targetClient = -1;


	if (trap->Argc() < 1) {
		trap->SendServerCommand(ent->s.number, "print \"Usage: ^1amghost ID/name\n\"");
		return;
	}

	//can ghost, partial name or clientNum
	trap->Argv(1, arg1, sizeof(arg1));

	if (ent)
	{
		targetClient = (trap->Argc() > 1) ? G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT) : ent - g_entities;
	}

	if (targetClient == -1) {
		return;
	}

	targ = &g_entities[targetClient];

	/*if (!AM_CanInflict(ent, targ)) {
		return;
	}*/
	targ->client->pers.vPersistent.isGhost = !targ->client->pers.vPersistent.isGhost;
	if (targ->client->pers.vPersistent.isGhost) {
		Toggle_Func(targ, GHOST, ON);
		//Draw String
	}
	else {
		Toggle_Func(targ, GHOST, OFF);
		//Draw String
	}
	trap->LinkEntity((sharedEntity_t *)targ);
}

// log in using user + pass
void AM_Login( gentity_t *ent ) {
	char argUser[32] = { 0 }, argPass[32] = { 0 };
	account_t *account = NULL, *current = NULL;

	if ( trap->Argc() < 3 ) {
		trap->SendServerCommand( ent->s.number, "print \"Please enter a name and password to login\n\"" );
		return;
	}

	//Grab user + pass
	trap->Argv( 1, argUser, sizeof( argUser ) );
	trap->Argv( 2, argPass, sizeof( argPass ) );

	// find valid user
	for ( current = accounts; current; current = current->next ) {
		if ( !strcmp( current->v_User, argUser ) ) {
			// case sensitive password
			if ( !strcmp(current->v_Password, argPass) ) {
				ent->client->pers.account = account = current;
				break;
			}
		}
	}

	if ( account ) 
	{
		const char *loginMsg = ent->client->pers.account->v_loginMsg;
		char *sendMsg = NULL;

		//AM_SetLoginEffect( ent );
		
		if ( !VALIDSTRING( loginMsg ) ) {
			trap->SendServerCommand( ent->s.number, "print \"You have logged in\n\"" );
			return;
		}
		
		sendMsg = Q_strrep( ent->client->pers.account->v_loginMsg, "*name*", ent->client->pers.netname );
		Q_ConvertLinefeeds( sendMsg );
		trap->SendServerCommand( -1, va( "print \"%s\n\"", sendMsg ) );
		free( sendMsg );
	}
	else 
	{
		trap->SendServerCommand( ent->s.number, "print \"Invalid login\n\"" );
	}
}


void AM_Logout( gentity_t *ent ) {
	trap->SendServerCommand( ent->s.number, "print \"Logout recognized.\n\"" );
}

void G_SleepClient(gclient_t *cl) {
	cl->pers.vPersistent.isSlept = qtrue;
	/*if (cl->hook) {
		Weapon_HookFree(cl->hook);
	}*/
	BG_ClearRocketLock(&cl->ps);
	VectorClear( &cl->ps.velocity );
	cl->ps.forceHandExtend = HANDEXTEND_KNOCKDOWN;
	cl->ps.forceHandExtendTime = INT32_MAX;
	cl->ps.forceDodgeAnim = 0;
}

static void AM_Merc_f(gentity_t *ent) {
	char arg1[64] = { 0 };
	int targetClient;
	gentity_t *targ = NULL;

	// can merc self, partial name, clientNum
	trap->Argv(1, arg1, sizeof(arg1));

	if (ent) {
		targetClient = (trap->Argc() > 1) ? G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT) : ent - g_entities;
	}
	else {
		targetClient = (trap->Argc() > 1) ? G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT) : -1;
	}


	if (targetClient == -1) {
		return;
	}

	targ = &g_entities[targetClient];

	/*if (!AM_CanInflict(ent, targ)) {
		return;
	}*/

	targ->client->pers.vPersistent.merc = !targ->client->pers.vPersistent.merc;
	// give everything between WP_NONE and LAST_USEABLE_WEAPON
	if (targ->client->pers.vPersistent.merc) {
		Toggle_Func(targ, MERC, ON);
		//Draw string bleh
	}
	// back to spawn weapons, select first usable weapon
	else {
		Toggle_Func(targ, MERC, OFF);
		//Draw string blehbleh
	}
}

static void AM_Noclip_f( gentity_t *ent ) {
	char arg1[64] = { 0 };
	int targetClient;
	gentity_t *targ = NULL;

	// can merc self, partial name, clientNum
	trap->Argv(1, arg1, sizeof(arg1));

	if (ent) {
		targetClient = (trap->Argc() > 1) ? G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT) : ent - g_entities;
	}
	else {
		targetClient = (trap->Argc() > 1) ? G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT) : -1;
	}


	if (targetClient == -1) {
		return;
	}

	targ = &g_entities[targetClient];

	/*if (!AM_CanInflict(ent, targ)) {
	return;
	}*/
	Cmd_Noclip_f( targ );
}

static void AM_Sleep_f( gentity_t *ent ) {
	char arg1[MAX_NETNAME] = { 0 };
	int clientNum;

	if (trap->Argc() < 2) {
		trap->SendServerCommand( ent->s.number, "print \"Usage: ^1amsleep ID/name ^7or ^1amsleep -1 ^7(to sleep all)\n\"" );
		return;
	}

	// grab the clientNum
	trap->Argv(1, arg1, sizeof(arg1));
	clientNum = G_ClientFromString(ent, arg1, FINDCL_SUBSTR);

	// check for purposely sleeping all. HACKHACKHACK
	if ( arg1[0] == '-' && arg1[1] == '1' ) {
		clientNum = -2;
	}

	// sleep everyone
	if ( clientNum == -2 ) {
		qboolean allSlept = qtrue;
		int i;
		gentity_t *e;
		for ( i = 0, e = g_entities; i < level.maxclients; i++, e++ ) {
			if ( !e->inuse || e->client->pers.connected == CON_DISCONNECTED ) {
				continue;
			}

			if ( !e->client->pers.vPersistent.isSlept ) {
				allSlept = qfalse;
				break;
			}
		}

		if ( allSlept ) {
			return;
		}

		for ( i = 0, e = g_entities; i < level.maxclients; i++, e++ ) {
			if ( !e->inuse || e->client->pers.connected == CON_DISCONNECTED ) {
				continue;
			}

			/*
			Can Inflict check
			*/

			G_SleepClient( e->client );
		}
		
		//Draw string
	}
	// sleep specified clientNum
	else if ( clientNum != -1 ) {
		gentity_t *e = g_entities + clientNum;

		/*if ( !AM_CanInflict( ent, e ) ) {
			return;
		}*/

		if ( e->client->pers.vPersistent.isSlept ) {
			return;
		}

		G_SleepClient( e->client );

		//Draw string
	}
}

void G_WakeClient( gclient_t *cl ) {
	const animNumber_t anim = BOTH_GETUP1;
	cl->pers.vPersistent.isSlept = qfalse;
	cl->ps.forceHandExtendTime = level.time + BG_AnimLength(g_entities[cl->ps.clientNum].localAnimIndex, anim);
	cl->ps.forceDodgeAnim = anim;
}

static void AM_Wake_f( gentity_t *ent ) {
	char arg1[MAX_NETNAME] = { 0 };
	int clientNum;

	if (trap->Argc() < 2) {
		trap->SendServerCommand( ent->s.number, "print \"Usage: ^1amwake ID/name ^7or ^1amwake -1 ^7(to wake all)\n\"" );
		return;
	}

	// grab the clientNum
	trap->Argv(1, arg1, sizeof(arg1));
	clientNum = G_ClientFromString(ent, arg1, FINDCL_SUBSTR);

	// check for purposely waking all. HACKHACKHACK
	if (arg1[0] == '-' && arg1[1] == '1') {
		clientNum = -2;
	}

	// wake everyone
	if (clientNum == -2) {
		qboolean allWoken = qtrue;
		int i;
		gentity_t *e;
		for (i = 0, e = g_entities; i < level.maxclients; i++, e++) {
			if (!e->inuse || e->client->pers.connected == CON_DISCONNECTED) {
				continue;
			}

			if (e->client->pers.vPersistent.isSlept) {
				allWoken = qfalse;
				break;
			}
		}

		if (allWoken) {
			return;
		}

		for (i = 0, e = g_entities; i < level.maxclients; i++, e++) {
			if (!e->inuse || e->client->pers.connected == CON_DISCONNECTED) {
				continue;
			}

			/*if (!AM_CanInflict(ent, e)) {
				continue;
			}*/

			G_WakeClient( e->client );
		}
		// Draw centerprint or chat output.
	}
	// sleep specified clientNum
	else if (clientNum != -1) {
		gentity_t *e = g_entities + clientNum;

		/*if (!AM_CanInflict(ent, e)) {
			return;
		}*/

		if (!e->client->pers.vPersistent.isSlept) {
			return;
		}

		G_WakeClient( e->client );
		e->s.heldByClient = 12;

		//Draw centerprint or chat output.
	}
}

void v_AM_Accountmanage( gentity_t *ent, account_t *account ) {
	//place holder
}

//
// a b c d e f g
// h i j k l m n
// o p q r s t u
// v w x y z - sort after alphabet
static const VisionCommand_t VisionCommands[] = {
	{ "amempower",	"empower",		PRIV_EMPWR,		AM_Empower_f,		qtrue },
	{ "amghost",	"ghost",		PRIV_GHOST,		AM_Ghost_f,			qtrue },
	{ "amlogin",	"x",			0xBADC0DED,		AM_Login,			qfalse },
	{ "amlogout",	"x",			0xBADC0DED,		AM_Logout,			qfalse },
	{ "ammerc",		"merc",			PRIV_MERC,		AM_Merc_f,			qtrue },
	{ "amnoclip",	"v_noclip",		PRIV_NOCLP,		AM_Noclip_f,		qtrue },
	{ "amsleep",	"sleep",		PRIV_SLEEP,		AM_Sleep_f,			qtrue },
	{ "amwake",		"wake",			PRIV_SLEEP,		AM_Wake_f,			qtrue },
	{ "v_account",	"accountmgr",	PRIV_ACTMGR,	v_AM_Accountmanage, qtrue },
};
static const size_t numVisionCommands = ARRAY_LEN( VisionCommands );

// Ja++ CONTENT (HAS TO BE RECODED):

//Author: Ja++ ?
static int v_cmdcmp( const void *a, const void *b) {
	return Q_stricmp( (const char *)a, ((VisionCommand_t*)b)->cmd );
}

//Author: Ja++ ?
// handle admin related commands.
// return true if the command exists and/or everything was handled fine.
// return false if command didn't exist, so we can tell them.
qboolean v_HandleCommands( gentity_t *ent, const char *cmd ) {
	VisionCommand_t *command = NULL;
	
	if (ent == NULL) { // call from console or anything else (which shouldn't happen)
		command = (VisionCommand_t *)bsearch(cmd, VisionCommands, numVisionCommands, sizeof(VisionCommands[0]), v_cmdcmp);
		if (command) {
			command->func(NULL);
			return qtrue;
		}
	}
	
	command = (VisionCommand_t *)bsearch( cmd, VisionCommands, numVisionCommands, sizeof( VisionCommands[0] ), v_cmdcmp );
	if ( !command ) {
		return qfalse;
	}

	//Authorization Check
	// * * * Place Holder * * *

	command->func( ent );
	return qtrue;
}