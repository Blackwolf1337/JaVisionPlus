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

	if ( power == FORCE ) 
	{
		switch ( toggle ) 
		{
		case ON:
			ent->client->ps.fd.forcePowerSelected = 0; // HACK: What the actual fuck
			ent->client->ps.eFlags |= EF_BODYPUSH;

			ent->client->pers.vPersistent.forcePowersKnown = ent->client->ps.fd.forcePowersKnown;

			for ( i = 0; i < NUM_FORCE_POWERS; i++ ) {
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
	
	else if ( power == MERC ) 
	{
		switch ( toggle ) 
		{
		case ON:
			// save forcepower data because we strip them + revert
			for ( int fp = 0; fp < NUM_FORCE_POWERS; fp++ ) {
				ent->client->pers.vPersistent.forcePowerBaseLevel[fp] = ent->client->ps.fd.forcePowerBaseLevel[fp];
				ent->client->pers.vPersistent.forcePowerLevel[fp] = ent->client->ps.fd.forcePowerLevel[fp];
				if (ent->client->ps.fd.forcePowersActive & (1 << fp)) {
					WP_ForcePowerStop( ent, (forcePowers_t)fp );
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

			if ( ent->client->pers.jetpack != qtrue )
				ent->client->ps.stats[STAT_HOLDABLE_ITEMS] &= ~(1 << HI_JETPACK);
		break;

		case OFF:
			// revert forcepower state
			ent->client->ps.fd.forcePowerSelected = 0; // HACK: What the actual fuck

			for (int fp = 0; fp < NUM_FORCE_POWERS; fp++) {
				ent->client->ps.fd.forcePowerBaseLevel[fp] = ent->client->pers.vPersistent.forcePowerBaseLevel[fp];
				ent->client->ps.fd.forcePowerLevel[fp] = ent->client->pers.vPersistent.forcePowerLevel[fp];
				if ( ent->client->ps.fd.forcePowersActive & (1 << fp) ) {
					WP_ForcePowerStop( ent, (forcePowers_t)fp );
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
				if ( ( ent->client->ps.stats[STAT_WEAPONS] & (1 << i) ) ) {
					newWeap = i;
					break;
				}
			}

			if (newWeap == WP_NUM_WEAPONS) {
				for ( int i = WP_STUN_BATON; i < WP_SABER; i++ ) {
					if ( ( ent->client->ps.stats[STAT_WEAPONS] & (1 << i) ) ) {
						newWeap = i;
						break;
					}
				}
				if ( newWeap == WP_SABER ) {
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

			if ( ent->client->ps.weapon != WP_SABER ) {
				G_AddEvent(ent, EV_NOAMMO, wp);
			}
		break;
		}
	}

	else if ( power == GHOST )
	{
		switch ( toggle )
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

	else if ( power == SLEEP )
	{
		switch ( toggle )
		{
		case ON:
			ent->client->pers.vPersistent.isSlept = qtrue;
			if ( ent->client->hook ) {
			Weapon_HookFree( ent->client->hook );
			}
			BG_ClearRocketLock( &ent->client->ps );
			VectorClear( &ent->client->ps.velocity);
			ent->client->ps.forceHandExtend = HANDEXTEND_KNOCKDOWN;
			ent->client->ps.forceHandExtendTime = INT32_MAX;
			ent->client->ps.forceDodgeAnim = 0;
		break;

		case OFF:
			ent->client->pers.vPersistent.isSlept = qfalse;
			ent->client->ps.forceHandExtendTime = level.time + BG_AnimLength(g_entities[ent->client->ps.clientNum].localAnimIndex, (animNumber_t)BOTH_GETUP1);
			ent->client->ps.forceDodgeAnim = (animNumber_t)BOTH_GETUP1;
		break;
		}
	}

	else if ( power == GOD )
	{
		switch ( toggle )
		{
		case ON:
			ent->client->pers.vPersistent.isGod = qtrue;
			ent->client->ps.powerups[7] = level.time + Q3_INFINITE;
			ent->takedamage = qfalse;
		break;

		case OFF:
			ent->client->pers.vPersistent.isGod = qfalse;
			ent->client->ps.powerups[7] = 0;
			ent->takedamage = qtrue;
		break;
		}
	}
}

static void AM_Empower_f( gentity_t *ent ) {
	char arg1[64] = { 0 };
	int targetClient;
	gentity_t *targ;

	// can empower: self, partial name, clientNum
	trap->Argv( 1, arg1, sizeof( arg1 ) );
	if ( ent ) {
		targetClient = ( trap->Argc() > 1 ) ? G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT ) : ent - g_entities;
	}
	else {
		targetClient = ( trap->Argc() > 1 ) ? G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT ) : -1;
	}

	if ( targetClient == -1 ) {
		return;
	}

	targ = &g_entities[targetClient];

	/*if (!AM_CanInflict(ent, targ)) {
	return;
	}*/

	targ->client->pers.vPersistent.empowered = !targ->client->pers.vPersistent.empowered;
	if ( targ->client->pers.vPersistent.empowered ) {
		Toggle_Func( targ, FORCE, ON );
		//Draw string bleh
	}
	else {
		Toggle_Func( targ, FORCE, OFF );
		//Draw string blehbleh
	}
}

// force the specified client to a specific team
static void AM_ForceTeam_f(gentity_t *ent) {
	char arg1[64] = { 0 }, arg2[64] = { 0 };
	int targetClient;
	gentity_t *targ;

	if ( trap->Argc() != 3 ) {
		trap->SendServerCommand( ent->s.number, "print \"Usage: ^1amforceteam ID/name <team> ^7or ^1amforceteam -1 <team> ^7(to force all)\n\"" );
		return;
	}

	//amforceteam <partial name|clientNum> <team>
	trap->Argv( 1, arg1, sizeof( arg1 ) );
	trap->Argv( 2, arg2, sizeof( arg2 ) );

	if ( ent ) {
		targetClient = ( trap->Argc() > 1 ) ? G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT ) : ent - g_entities;
	}
	else {
		targetClient = ( trap->Argc() > 1 ) ? G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT ) : -1;
	}

	// check for purposely sleeping all. HACKHACKHACK
	if ( arg1[0] == '-' && arg1[1] == '1' ) {
		targetClient = -2;
	}

	// sleep everyone
	if ( targetClient == -2 ) {
		qboolean allForced = qtrue;
		int i;
		gentity_t *e;
		for ( i = 0, e = g_entities; i < level.maxclients; i++, e++ ) {
			if ( !e->inuse || e->client->pers.connected == CON_DISCONNECTED ) {
				continue;
			}

			if ( !e->client->pers.vPersistent.isSlept ) {
				allForced = qfalse;
				break;
			}
		}

		if ( allForced ) {
			return;
		}

		for ( i = 0, e = g_entities; i < level.maxclients; i++, e++ ) {
			if ( !e->inuse || e->client->pers.connected == CON_DISCONNECTED ) {
				continue;
			}

			/*
			Can Inflict check
			*/

			SetTeam(e, arg2);
		}

		//Draw string
	}

	targ = &g_entities[targetClient];

	/*if (!AM_CanInflict(ent, targ)) {
		return;
	}*/

	if ( targ->inuse && targ->client && targ->client->pers.connected ) {
		SetTeam( targ, arg2 );
	}
}

// ghost specified client (or self)
static void AM_Ghost_f( gentity_t *ent ) {
	char arg1[64] = { 0 };
	gentity_t *targ = NULL;
	int targetClient = -1;


	if ( trap->Argc() < 1 ) {
		trap->SendServerCommand( ent->s.number, "print \"Usage: ^1amghost ID/name\n\"" );
		return;
	}

	//can ghost, partial name or clientNum
	trap->Argv( 1, arg1, sizeof( arg1 ) );

	if ( ent )
	{
		targetClient = ( trap->Argc() > 1 ) ? G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT ) : ent - g_entities;
	}

	if ( targetClient == -1 ) {
		return;
	}

	targ = &g_entities[targetClient];

	/*if (!AM_CanInflict(ent, targ)) {
		return;
	}*/
	targ->client->pers.vPersistent.isGhost = !targ->client->pers.vPersistent.isGhost;
	if ( targ->client->pers.vPersistent.isGhost ) {
		Toggle_Func( targ, GHOST, ON );
		//Draw String
	}
	else {
		Toggle_Func( targ, GHOST, OFF );
		//Draw String
	}
	trap->LinkEntity( (sharedEntity_t *)targ );
}

static void AM_Give_f( gentity_t *ent ) {
	int client;
	gentity_t *target = NULL;
	char arg1[64] = { 0 };
	char arg2[64] = { 0 };
	qboolean shift = 0;

	if ( !ent ) {
		trap->Print("This command is not available for server console use yet\n");
		return;
	}

	if ( trap->Argc() < 1 ) {
		trap->SendServerCommand( ent->s.number, "print \"Usage: ^1amgive <client> ammo/weapon/force/health/armor id <amount> \n        \\amgive <client> health/armor <amount>\n\"" );
		return;
	}

	/*if (!AM_CanInflict(ent, target)) {
		return;
	}*/

	trap->Argv( 1, arg1, sizeof( arg1 ) );
	if (!Q_stricmp(arg1, "weapon") || !Q_stricmp(arg1, "force") || !Q_stricmp(arg1, "ammo")
		|| !Q_stricmp(arg1, "health") || !Q_stricmp(arg1, "armor") || !Q_stricmp(arg1, "all") 
		|| !Q_stricmp(arg1, "shield") || !Q_stricmp(arg1, "weapons") || !Q_stricmp(arg1, "weaponnum")
		|| !Q_stricmp(arg1, "excellent") || !Q_stricmp(arg1, "impressive") || !Q_stricmp(arg1, "gauntletaward")
		|| !Q_stricmp(arg1, "defend") || !Q_stricmp(arg1, "assist"))
		shift = qtrue;

	trap->Argv( 2 - shift, arg2, sizeof( arg2 ) );

	if ( !shift ) {
		client = G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT );
		if (client == -1) {
			return;
		}
		target = &g_entities[client];
		G_Give( target, arg2, ConcatArgs( 3 - shift ), trap->Argc() - 1 );

	}
	else {
		target = ent;
		G_Give( target, arg2, ConcatArgs( 3 - shift ), trap->Argc() );
	}
}

static void AM_God_f( gentity_t *ent ) {
	char arg1[64] = { 0 };
	int targetClient = -1;
	gentity_t *targ = NULL;

	if (trap->Argc() < 1) {
		trap->SendServerCommand( ent->s.number, "print \"Usage: ^1amgod <client/ID>\n\"" );
		return;
	}

	// can god, partial name, clientNum
	trap->Argv( 1, arg1, sizeof( arg1 ) );

	if ( ent ) {
		targetClient = ( trap->Argc() > 1 ) ? G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT ) : ent - g_entities;
	}

	if ( targetClient == -1 ) {
		return;
	}

	targ = &g_entities[targetClient];

	/*if (!AM_CanInflict(ent, targ)) {
		return;
	}*/

	targ->client->pers.vPersistent.isGod = !targ->client->pers.vPersistent.isGod;

	if ( targ->client->pers.vPersistent.isGod ) {
		Toggle_Func( targ, GOD, ON );
		//Draw string
	}
	else {
		Toggle_Func( targ, GOD, OFF );
		//Draw string
	}
}

static void AM_Gravity_f( gentity_t *ent ) {
	char arg1[64] = { 0 };
	char arg2[64] = { 0 };
	int targetClient;
	gentity_t *targ = NULL;

	if ( trap->Argc() < 2 ) {
		trap->SendServerCommand( ent->s.number, "print \"Usage: ^1amgravity <client/id> <gravity (^2800^7)>\n\"" );
		return;
	}

	//Arg1 -> ID
	//Arg2 -> Size (Float not yet implented // Integer Only)
	trap->Argv( 1, arg1, sizeof(arg1) );
	trap->Argv( 2, arg2, sizeof(arg2) );
	targetClient = ( trap->Argc() > 1 ) ? G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT ) : ent - g_entities;

	if ( targetClient == -1 ) {
		return;
	}

	targ = &g_entities[targetClient];

	/*if (!AM_CanInflict(ent, targ)) {
		return;
	}*/

	targ->client->pers.vPersistent.gravity = atoi( arg2 );
}

static void AM_Kick_f( gentity_t *ent )  {
	char arg1[64] = { 0 };
	const char *reason = "Not specified";
	int clientNum;

	if ( trap->Argc() == 1 ) {
		trap->SendServerCommand( ent->s.number, "print \"Usage: ^1amkick <client> <reason>\n\"" );
		return;
	}

	trap->Argv( 1, arg1, sizeof( arg1 ) );
	if ( trap->Argc() > 2 ) {
		reason = ConcatArgs( 2 );
	}

	clientNum = G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT );

	if ( clientNum == -1 ) {
		return;
	}

	/*if (!AM_CanInflict(ent, &g_entities[clientNum])) {
		return;
	}*/
	trap->DropClient( clientNum, va( "^3kicked ^7for '%s'", reason ) );
}

void AM_PrintCommands(gentity_t *ent, printBufferSession_t *pb);
static void AM_Info_f( gentity_t *ent ) {
	int extendedInfo = 0;
	printBufferSession_t pb;

	Q_NewPrintBuffer(&pb, MAX_STRING_CHARS /*/ 2*/, PB_Callback, ent - g_entities);

	Q_PrintBuffer(&pb, S_COLOR_YELLOW "================================================================"
		S_COLOR_WHITE "\n\n");

	if (trap->Argc() < 2) {
		unsigned int i = 0;
		Q_PrintBuffer(&pb, "Try 'aminfo <category>' for more detailed information\nCategories: ^1");

		// print all categories
		for (i = 0; i < numAminfoSettings; i++) {
			if (i) {
				Q_PrintBuffer(&pb, "^7, ^1");
			}
			Q_PrintBuffer(&pb, aminfoSettings[i].str);
		}

		//From the old VisionPlugin
		//PrintFile(&pb);

		Q_PrintBuffer(&pb, "\n\n");
	}
	else {
		char arg[8] = { 0 };
		unsigned int i = 0;

		trap->Argv(1, arg, sizeof(arg));

		// find out which category they want
		for (i = 0; i < numAminfoSettings; i++) {
			if (!Q_stricmp(arg, aminfoSettings[i].str)) {
				extendedInfo = aminfoSettings[i].bit;
				break;
			}
		}
	}

	// mod version + compile date
	if (!extendedInfo || extendedInfo == EXTINFO_ALL) {
		char version[256] = { 0 };
		trap->Cvar_VariableStringBuffer("version", version, sizeof(version));
		Q_PrintBuffer(&pb, "Version:\n    Gamecode: JaVision+ (" ARCH_STRING ") -  " __DATE__ ", " __TIME__ "\n");
		Q_PrintBuffer(&pb, va("    Engine: %s\n\n", version));
	}

	if (extendedInfo & EXTINFO_ADMIN) {
		// admin commands
		AM_PrintCommands( ent, &pb );
	}

	if (extendedInfo & EXTINFO_CMDS) {
		// regular commands
		//G_PrintCommands(ent, &pb);
	}

	/*if (extendedInfo & EXTINFO_CLIENT) {
		// client support flags
		int i = 0;

		Q_PrintBuffer(&pb, va("Client support flags: 0x%X\n", ent->client->pers.CSF));
		for (i = 0; i < CSF_NUM; i++) {
			if (ent->client->pers.CSF & (1 << i)) {
				Q_PrintBuffer(&pb, " [X] ");
			}
			else {
				Q_PrintBuffer(&pb, " [ ] ");
			}
			Q_PrintBuffer(&pb, va("%s\n", supportFlagNames[i]));
		}

		//RAZTODO: cp_pluginDisable?
		Q_PrintBuffer(&pb, va("Client plugin disabled: %i\n", ent->client->pers.CPD));
		for (i = 0; i < CPD_NUM; i++) {
			if (ent->client->pers.CPD & (1 << i)) {
				Q_PrintBuffer(&pb, " [X] ");
			}
			else {
				Q_PrintBuffer(&pb, " [ ] ");
			}
			Q_PrintBuffer(&pb, va("%s\n", clientPluginDisableNames[i]));
		}
	}*/

	Q_PrintBuffer(&pb, S_COLOR_YELLOW "================================================================\n");

	Q_DeletePrintBuffer(&pb);
}

static void AM_KillVote_f( gentity_t *ent ) {
	//Overkill, but it's a surefire way to kill the vote =]
	level.voteExecuteTime = 0;
	level.votingGametype = qfalse;
	level.votingGametypeTo = level.gametype;
	level.voteTime = 0;

	level.voteDisplayString[0] = '\0';
	level.voteString[0] = '\0';

	trap->SetConfigstring( CS_VOTE_TIME, "" );
	trap->SetConfigstring( CS_VOTE_STRING,  "" );
	trap->SetConfigstring( CS_VOTE_YES, "" );
	trap->SetConfigstring( CS_VOTE_NO, "" );

	trap->SendServerCommand( -1, "print \"" S_COLOR_RED "Vote has been killed!\n\"" );
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
	gentity_t *e;
	int i;

	ent->client->pers.vPersistent.isTag = qfalse;
	ent->client->pers.account = NULL;
	trap->SendServerCommand( ent->s.number, "print \"You have logged out\n\"");

}

static void AM_Merc_f( gentity_t *ent ) {
	char arg1[64] = { 0 };
	int targetClient;
	gentity_t *targ = NULL;

	// can merc self, partial name, clientNum
	trap->Argv( 1, arg1, sizeof( arg1 ) );

	if ( ent ) {
		targetClient = ( trap->Argc() > 1) ? G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT ) : ent - g_entities;
	}
	else {
		targetClient = ( trap->Argc() > 1) ? G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT ) : -1;
	}


	if ( targetClient == -1 ) {
		return;
	}

	targ = &g_entities[targetClient];

	/*if (!AM_CanInflict(ent, targ)) {
		return;
	}*/

	targ->client->pers.vPersistent.merc = !targ->client->pers.vPersistent.merc;
	// give everything between WP_NONE and LAST_USEABLE_WEAPON
	if ( targ->client->pers.vPersistent.merc ) {
		Toggle_Func( targ, MERC, ON );
		//Draw string bleh
	}
	// back to spawn weapons, select first usable weapon
	else {
		Toggle_Func( targ, MERC, OFF );
		//Draw string blehbleh
	}
}

static void AM_Notarget_f( gentity_t *ent ) {
	char arg1[64] = { 0 };
	gentity_t *targ = NULL;
	int targetClient = -1;


	if ( trap->Argc() < 2 ) {
		trap->SendServerCommand( ent->s.number, "print \"Usage: ^1amnotarget <client/ID>\n\"" );
		return;
	}

	// can toggle noclip for partial name or clientNum
	trap->Argv( 1, arg1, sizeof( arg1 ) );

	targetClient = ( trap->Argc() > 1 ) ? G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT ) : ent ? ent - g_entities : -1;

	if ( targetClient == -1 ) {
		return;
	}

	targ = &g_entities[targetClient];

	/*if ( !AM_CanInflict( ent, targ ) ) {
		return;
	}*/

	Cmd_Notarget_f( targ );
	
	trap->LinkEntity( (sharedEntity_t *)targ );
}

static void AM_NPCSpawn_f( gentity_t *ent ) {
	
	if (!ent) {
		trap->Print( "This command is not available for server console use yet\n" );
		return;
	}

	Cmd_NPC_f( ent );
}

static void AM_Poll_f( gentity_t *ent ) {
	int i = 0;
	char arg1[MAX_TOKEN_CHARS] = { 0 }, arg2[MAX_TOKEN_CHARS] = { 0 };

	if (level.voteExecuteTime) {
		trap->SendServerCommand( ent->s.number, "print \"Vote already in progress.\n\"" );
		return;
	}

	if ( trap->Argc() < 2 ) {
		trap->SendServerCommand( ent->s.number, "print \"Please specify a poll.\n\"" );
		return;
	}

	trap->Argv( 0, arg1, sizeof( arg1 ) );
	Q_strncpyz( level.voteStringPoll, ConcatArgs( 1 ), sizeof( level.voteStringPoll ) );
	Q_strncpyz( level.voteStringPollCreator, ent ? ent->client->pers.netname_nocolor : "Server", sizeof( level.voteStringPollCreator ) );
	Q_ConvertLinefeeds(level.voteStringPoll);
	
	Q_strncpyz( arg2, ent ? ent->client->pers.netname : "Server", sizeof( arg2 ) );
	Q_CleanStr( arg2 );
	Q_strstrip( arg2, "\n\r;\"", NULL );

	Com_sprintf( level.voteString, sizeof( level.voteString ), "%s \"%s\"", arg1, arg2 );
	Com_sprintf( level.voteDisplayString, sizeof(level.voteDisplayString), "%s", level.voteString );
	Q_strncpyz( level.voteStringClean, level.voteString, sizeof( level.voteStringClean ) );
	Q_strstrip( level.voteStringClean, "\"\n\r", NULL );

	trap->SendServerCommand( -1, va( "print \"%s" S_COLOR_WHITE " %s\n\"", ent ? ent->client->pers.netname : "Server", G_GetStringEdString( "MP_SVGAME", "PLCALLEDVOTE" ) ) );

	// still a vote waiting to be executed
	if ( level.voteExecuteTime ) {
		level.voteExecuteTime = 0;
		if ( !level.votePoll ) {
			trap->SendConsoleCommand( EXEC_APPEND, va( "%s\n", level.voteString ) );
		}
	}

	level.voteExecuteDelay = v_voteDelay.integer;
	level.voteTime = level.time;
	level.voteYes = 0;
	level.voteNo = 0;
	level.votePoll = qtrue;
	level.votingGametype = qfalse;

	for ( i = 0; i < level.maxclients; i++ ) {
		level.clients[i].mGameFlags &= ~PSG_VOTED;
		level.clients[i].pers.vote = 0;
	}

	trap->SetConfigstring( CS_VOTE_TIME, va( "%i", level.voteTime ) );
	trap->SetConfigstring( CS_VOTE_STRING, level.voteDisplayString );
	trap->SetConfigstring( CS_VOTE_YES, va( "%i", level.voteYes ) );
	trap->SetConfigstring( CS_VOTE_NO, va( "%i", level.voteNo ) );
}

static void AM_Protect_f( gentity_t *ent ) {
	char arg1[64] = { 0 };
	int targetClient;
	gentity_t *targ;

	// can protect: self, partial name, clientNum
	trap->Argv( 1, arg1, sizeof( arg1 ) );
	targetClient = ( trap->Argc() > 1 ) ? G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT ) : ent - g_entities;

	if ( targetClient == -1 ) {
		return;
	}

	targ = &g_entities[targetClient];

	/*if (!AM_CanInflict(ent, targ)) {
		return;
	}*/

	targ->client->ps.eFlags ^= EF_INVULNERABLE;
	targ->client->invulnerableTimer = level.time + 5000;

	if (!!(targ->client->ps.eFlags&EF_INVULNERABLE))
	{
		targ->client->pers.vPersistent.invulnerableSpecial = qtrue;
	}
	else
	{
		targ->client->pers.vPersistent.invulnerableSpecial = qfalse;
	}
}

static void AM_Psay_f( gentity_t *ent ) {
	char *msg, arg1[MAX_NETNAME] = { 0 };

	if ( trap->Argc() < 2 ) {
		trap->SendServerCommand( ent->s.number, "print \"Usage: ^1ampsay <client> <message>\n\"" );
		return;
	}

	msg = ConcatArgs( 2 );
	Q_ConvertLinefeeds( msg );

	trap->Argv( 1, arg1, sizeof( arg1 ) );
	if ( arg1[0] == '-' && arg1[1] == '1' ) {
		// announce to everyone
		if ( ent ) {
			trap->SendServerCommand( -1, va( "cp \"%s\"", msg ) );
		}
	}
	else {
		// announce to a certain client
		const int targetClient = G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT );

		if ( targetClient == -1 ) {
			return;
		}

		trap->SendServerCommand( targetClient, va( "cp \"%s\"", msg ) );
		trap->SendServerCommand( targetClient, va( "print \"%s\n\"", msg ) );
		if ( ent ) {
			trap->SendServerCommand( ent - g_entities, va( "cp\"Relay:\n%s\"", msg ) );
			trap->SendServerCommand( ent - g_entities, va( "print \"Relay:\n%s\n\"", msg ) );
		}
	}
}

static void AM_Remap_f( gentity_t *ent ) {
	char arg1[128] = { 0 };
	char arg2[128] = { 0 };

	if ( trap->Argc() < 2)  {
		trap->SendServerCommand( ent->s.number, "print \"Usage: ^1amremap <from> <to>\n\"" );
		return;
	}
	trap->Argv( 1, arg1, sizeof(arg1) );
	trap->Argv( 2, arg2, sizeof(arg2) );

	AddRemap( arg1, arg2, level.time );
	trap->SetConfigstring( CS_SHADERSTATE, BuildShaderStateConfig() );
}

static void AM_Rename_f( gentity_t *ent ) {
	char arg1[MAX_NETNAME] = { 0 }, arg2[MAX_NETNAME] = { 0 }, oldName[MAX_NETNAME] = { 0 };
	char info[MAX_INFO_STRING] = { 0 };
	int targetClient;
	gentity_t *e = NULL;

	if ( trap->Argc() != 3 ) {
		trap->SendServerCommand( ent->s.number, "print \"Usage: ^1amrename <client> <name>\n\"" );
		return;
	}

	trap->Argv( 1, arg1, sizeof( arg1 ) );
	trap->Argv( 2, arg2, sizeof( arg2 ) );

	targetClient = G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT );
	if ( targetClient == -1 ) {
		return;
	}

	e = g_entities + targetClient;
	
	/*if (!AM_CanInflict(ent, e)) {
		return;
	}*/
	Q_strncpyz( oldName, e->client->pers.netname, sizeof( oldName ) );
	ClientCleanName( arg2, e->client->pers.netname, sizeof( e->client->pers.netname ) );

	if ( !strcmp( oldName, e->client->pers.netname ) ) {
		return;
	}

	Q_strncpyz( e->client->pers.netname_nocolor, e->client->pers.netname, sizeof( e->client->pers.netname_nocolor ) );
	Q_StripColor( e->client->pers.netname_nocolor );

	// update clientinfo
	trap->GetConfigstring( CS_PLAYERS + targetClient, info, sizeof( info ) );
	Info_SetValueForKey( info, "n", e->client->pers.netname );
	trap->SetConfigstring( CS_PLAYERS + targetClient, info );

	// update userinfo (in engine)
	trap->GetUserinfo( targetClient, info, sizeof( info ) );
	Info_SetValueForKey( info, "name", e->client->pers.netname );
	trap->SetUserinfo( targetClient, info );

	trap->SendServerCommand( -1, va( "print \"%s" S_COLOR_WHITE " has been renamed to %s^7\n\"", oldName, arg2 ) );

	e->client->pers.vPersistent.renamedTime = level.time;
}

static void AM_Silence_f( gentity_t *ent ) {
	char arg1[64] = { 0 };
	int targetClient;

	if ( trap->Argc() < 2 ) {
		trap->SendServerCommand( ent->s.number, "print \"Usage: ^1amsilence <client>\n\"" );
		return;
	}

	trap->Argv( 1, arg1, sizeof( arg1 ) );

	// silence everyone
	if ( atoi( arg1 ) == -1 ) {
		int i;
		gentity_t *e = NULL;
		gclient_t *cl = NULL;
		for (i = 0, e = g_entities, cl = level.clients;
			i < level.maxclients;
			i++, e++, cl++)
		{
			if ( !e->inuse || cl->pers.connected == CON_DISCONNECTED ) {
				continue;
			}

			/*if (!AM_CanInflict(ent, e)) {
				continue;
			}*/

			cl->pers.vPersistent.silenced = qtrue;
		}
		//Draw string
		return;
	}

	targetClient = G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT );
	if (targetClient == -1) {
		return;
	}

	/*if (!AM_CanInflict(ent, &g_entities[targetClient])) {
		return;
	}*/

	level.clients[targetClient].pers.vPersistent.silenced = qtrue;
	//Draw string
}

void G_Knockdown(gentity_t *victim);
void Slap( gentity_t *executor, gentity_t *targ ) {
	vec3_t newDir = { 0.0f, 0.0f, 0.0f };
	int i;
	
	for ( i = 0; i<2; i++ ) {
		newDir[i] = ( 2.0f * ( ( rand() & 0x7fff ) / ( (float)0x7fff ) - 0.5f ) );
		if ( newDir[i] > 0.0f ) {
			newDir[i] = ceilf( newDir[i] );
		}
		else {
			newDir[i] = floorf( newDir[i] );
		}
	}
	newDir[2] = 1.0f;

	if ( targ->client->hook ) {
		Weapon_HookFree( targ->client->hook );
	}

	G_Knockdown( targ );
	G_Throw( targ, &newDir, v_slapDistance.value );

	//Draw String
}

static void AM_Slap_f( gentity_t *ent ) {
	char arg1[64] = { 0 };
	int targetClient;

	if ( trap->Argc() != 2 ) {
		trap->SendServerCommand( ent->s.number, "print \"Usage: ^1amslap <client>\n\"" );
		return;
	}

	//Can slap: partial name, clientNum
	trap->Argv( 1, arg1, sizeof( arg1 ) );
	targetClient = G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT );

	if ( targetClient == -1 ) {
		return;
	}

	/*if (!AM_CanInflict(ent, &g_entities[targetClient])) {
		return;
	}*/

	Slap( ent, &g_entities[targetClient] );
}


static void AM_Dismember( gentity_t *ent ) {
	vec3_t boltPoint;

	int i;
	for ( i = G2_MODELPART_HEAD; i <= G2_MODELPART_RLEG; i++ ) 
	{
		if ( i == G2_MODELPART_WAIST )
			continue;
		G_GetDismemberBolt( ent, &boltPoint, i );
		G_Dismember( ent, NULL, &boltPoint, i, 90, 0, ent->client->ps.legsAnim, qfalse );
	}
}

static void AM_Slay_f(gentity_t *ent) {
	char arg1[64] = { 0 };
	int targetClient;
	gentity_t *targetEnt = NULL;

	if (trap->Argc() < 2) {
		trap->SendServerCommand( ent->s.number, "print \"Usage: ^1amslay <client>\n\"" );
		return;
	}

	trap->Argv( 1, arg1, sizeof( arg1 ) );

	if ( atoi( arg1 ) == -1 ) {
		int i;
		gentity_t *e;
		for ( i = 0, e = g_entities; i < level.maxclients; i++, e++ ) {
			if ( !e->inuse || ent->client->pers.connected == CON_DISCONNECTED || ent->client->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR || ent->client->tempSpectate >= level.time )
				continue;

			/*if (!AM_CanInflict(ent, e)) {
				continue;
			}*/

			Cmd_Kill_f( e );

			if (v_slaydismember.integer)
				AM_Dismember(targetEnt);
		}
		//Draw string
		return;
	}

	targetClient = G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT );
	targetEnt = g_entities + targetClient;

	if ( targetEnt->client->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR || targetEnt->client->tempSpectate >= level.time )
		return;

	/*if (!AM_CanInflict(ent, targetEnt)) {
		return;
	}*/

	Cmd_Kill_f(targetEnt);

	if ( v_slaydismember.integer )
		AM_Dismember( targetEnt );

	//Draw String
}

static void AM_Noclip_f( gentity_t *ent ) {
	char arg1[64] = { 0 };
	int targetClient;
	gentity_t *targ = NULL;

	// can merc self, partial name, clientNum
	trap->Argv( 1, arg1, sizeof( arg1 ) );

	if ( ent ) {
		targetClient = ( trap->Argc() > 1 ) ? G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT ) : ent - g_entities;
	}
	else {
		targetClient = ( trap->Argc() > 1 ) ? G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT ) : -1;
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

	if ( trap->Argc() < 2 ) {
		trap->SendServerCommand( ent->s.number, "print \"Usage: ^1amsleep ID/name ^7or ^1amsleep -1 ^7(to sleep all)\n\"" );
		return;
	}

	// grab the clientNum
	trap->Argv( 1, arg1, sizeof( arg1 ) );
	clientNum = G_ClientFromString( ent, arg1, FINDCL_SUBSTR );

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

			Toggle_Func( e, SLEEP, ON );
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

		Toggle_Func( e, SLEEP, ON );

		//Draw string
	}
}

static void AM_Speed_f( gentity_t *ent ) {
	char arg1[64] = { 0 };
	char arg2[64] = { 0 };
	int targetClient;
	gentity_t *targ = NULL;

	if (trap->Argc() < 2) {
		trap->SendServerCommand( ent->s.number, va( "print \"Usage: ^1amspeed <client/id> <speed (Default: ^2%s7)>\n\"", g_speed.value ) );
		return;
	}

	//Arg1 -> ID
	//Arg2 -> Size (Float not yet implented // Integer Only)
	trap->Argv( 1, arg1, sizeof( arg1 ) );
	trap->Argv( 2, arg2, sizeof( arg2 ) );
	targetClient = ( trap->Argc() > 1) ? G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT ) : ent - g_entities;

	if (targetClient == -1) {
		return;
	}

	targ = &g_entities[targetClient];

	/*if (!AM_CanInflict(ent, targ)) {
		return;
	}*/


	targ->client->pers.vPersistent.speed = atoi( arg2 );
}

static void AM_Teleport_f( gentity_t *ent ) {
	vec3_t		origin, angles;
	char		buffer[MAX_TOKEN_CHARS];
	char		arg1[64] = { 0 };
	int			i;
	int			targetClient;
	gentity_t	*targ;

	if ( trap->Argc() < 6 ) {
		trap->SendServerCommand( ent->s.number, va( "print \"Usage: ^1amteleport <client/id> x y z yaw\n\"" ) );
		return;
	}

	trap->Argv( 1, arg1, sizeof( arg1 ) );
	targetClient = ( trap->Argc() > 1 ) ? G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT ) : ent - g_entities;


	if ( targetClient == -1 ) {
		return;
	}

	targ = &g_entities[targetClient];

	VectorClear( angles );
	for ( i = 1; i < 4; i++ ) {
		trap->Argv( i + 1, buffer, sizeof( buffer ) );
		origin[i - 1] = atof(buffer);
	}

	trap->Argv( 5, buffer, sizeof( buffer ) );
	angles[YAW] = atof( buffer );

	TeleportPlayer( targ, origin, angles );
}

static void AM_Unsilence_f(gentity_t *ent) {
	char arg1[64] = { 0 };
	int targetClient;

	if (trap->Argc() < 2) {
		trap->SendServerCommand(ent->s.number, "print \"Usage: ^1amunsilence <client>\n\"");
		return;
	}

	trap->Argv( 1, arg1, sizeof( arg1 ) );

	// unsilence everyone
	if ( atoi( arg1 ) == -1 ) {
		int i;
		gentity_t *e;
		for ( i = 0, e = g_entities; i < level.maxclients; i++, e++ ) {
			if ( !e->inuse || ent->client->pers.connected == CON_DISCONNECTED ) {
				continue;
			}

			/*if ( !AM_CanInflict( ent, e ) ) {
				continue;
			}*/

			level.clients[i].pers.vPersistent.silenced = qfalse;
		}
		//Draw string
		return;
	}

	targetClient = G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT );
	if ( targetClient == -1 ) {
		return;
	}
	
	/*if (!AM_CanInflict(ent, &g_entities[targetClient])) {
		return;
	}*/

	level.clients[targetClient].pers.vPersistent.silenced = qfalse;
	//Draw string
}

static void AM_Vstr_f( gentity_t *ent ) {
	char *args = NULL;
	const char *filter = NULL;

	if ( !ent ) {
		trap->Print( "This command is not available for server console use yet\n" );
		return;
	}

	args = ConcatArgs(1);

	if ( (filter = Q_strchrs( args, ";\n ") ) != NULL ) {
		args[filter - args] = '\0';
	}

	trap->SendConsoleCommand( EXEC_APPEND, va( "vstr %s\n", args ) );
}

static void AM_Wake_f( gentity_t *ent ) {
	char arg1[MAX_NETNAME] = { 0 };
	int clientNum;

	if ( trap->Argc() < 2 ) {
		trap->SendServerCommand( ent->s.number, "print \"Usage: ^1amwake ID/name ^7or ^1amwake -1 ^7(to wake all)\n\"" );
		return;
	}

	// grab the clientNum
	trap->Argv( 1, arg1, sizeof( arg1 ) );
	clientNum = G_ClientFromString( ent, arg1, FINDCL_SUBSTR );

	// check for purposely waking all. HACKHACKHACK
	if ( arg1[0] == '-' && arg1[1] == '1' ) {
		clientNum = -2;
	}

	// wake everyone
	if ( clientNum == -2 ) {
		qboolean allWoken = qtrue;
		int i;
		gentity_t *e;
		for ( i = 0, e = g_entities; i < level.maxclients; i++, e++ ) {
			if ( !e->inuse || e->client->pers.connected == CON_DISCONNECTED ) {
				continue;
			}

			if ( e->client->pers.vPersistent.isSlept ) {
				allWoken = qfalse;
				break;
			}
		}

		if ( allWoken ) {
			return;
		}

		for ( i = 0, e = g_entities; i < level.maxclients; i++, e++ ) {
			if ( !e->inuse || e->client->pers.connected == CON_DISCONNECTED ) {
				continue;
			}

			/*if (!AM_CanInflict(ent, e)) {
				continue;
			}*/

			Toggle_Func( e, SLEEP, OFF );
		}
		// Draw centerprint or chat output.
	}
	// sleep specified clientNum
	else if ( clientNum != -1 ) {
		gentity_t *e = g_entities + clientNum;

		/*if (!AM_CanInflict(ent, e)) {
			return;
		}*/

		if ( !e->client->pers.vPersistent.isSlept ) {
			return;
		}

		Toggle_Func( e, SLEEP, OFF );
		e->s.heldByClient = 12;

		//Draw centerprint or chat output.
	}
}

//Ja++ weather code

// weather manipulation
static const char *weatherEffects[] = {
	"acidrain",
	"clear",
	"constantwind",
	"die",
	"fog",
	"freeze",
	"gustingwind",
	"heavyrain",
	"heavyrainfog",
	"light_fog",
	"lightrain",
	"outsidepain",
	"outsideshake",
	"rain",
	"sand",
	"snow",
	"spacedust",
	"wind",
	"zone",
};
static const size_t numWeatherEffects = ARRAY_LEN(weatherEffects);

static void G_PrintWeatherOptions( gentity_t *ent ) {
	const char **opt = NULL;
	char buf[256] = { 0 };
	int toggle = 0;
	unsigned int count = 0;
	const unsigned int limit = 72;
	size_t i;

	Q_strcat( buf, sizeof( buf ), "Weather options:\n   " );

	for (i = 0, opt = weatherEffects; i < numWeatherEffects; i++, opt++) {
		const char *tmpMsg = NULL;

		tmpMsg = va( " ^%c%s", ( ++toggle & 1 ? COLOR_GREEN : COLOR_YELLOW ), *opt );

		//newline if we reach limit
		if ( count >= limit ) {
			tmpMsg = va( "\n   %s", tmpMsg );
			count = 0;
		}

		if ( strlen( buf ) + strlen( tmpMsg ) >= sizeof( buf ) ) {
			if ( ent ) {
				trap->SendServerCommand( ent - g_entities, va( "print \"%s\"", buf ) );
			}
			else {
				trap->Print( va( "%s\n", buf ) );
			}
			buf[0] = '\0';
		}
		count += strlen( tmpMsg );
		Q_strcat( buf, sizeof( buf ), tmpMsg );
	}

	if ( ent ) {
		trap->SendServerCommand( ent - g_entities, va("print \"%s\n\n\"", buf ) );
	}
	else {
		trap->Print( va( "%s\n", buf ) );
	}
}

static int weather_cmdcmp( const void *a, const void *b ) {
	return Q_stricmp( (const char *)a, *(const char **)b );
}

static void AM_Weather_f( gentity_t *ent ) {
	const char *cmd = NULL, *opt = NULL;
	static int effectid = 0;

	if ( trap->Argc() == 1 ) {
		G_PrintWeatherOptions( ent );
		return;
	}

	cmd = ConcatArgs( 1 );
	trap->Print( "%s\n", cmd );
	opt = (const char *)bsearch( cmd, weatherEffects, numWeatherEffects, sizeof( weatherEffects[0] ), weather_cmdcmp );
	if ( !opt ) {
		G_PrintWeatherOptions( ent );
		return;
	}
	if ( effectid == 0 ) {
		effectid = G_EffectIndex( va( "*%s", cmd ) );
	}
	else {
		trap->SetConfigstring( CS_EFFECTS + effectid, va( "*%s", cmd ) );
	}
	//Draw string
}

// display list of admins
static void AM_WhoIs_f( gentity_t *ent ) {
	int i;
	gentity_t *e = NULL;
	printBufferSession_t pb;

	Q_NewPrintBuffer(&pb, MAX_STRING_CHARS /*/ 1.5*/, PB_Callback, ent ? (ent - g_entities) : -1);

	//TODO: optimal spacing
	Q_PrintBuffer(&pb, "Listing admins...\n");
	Q_PrintBuffer(&pb, "Name                                Admin User                      Rank\n");
	for (i = 0, e = g_entities; i < level.maxclients; i++,	e++) {
		if (e->client->pers.account) {
			char strName[MAX_NETNAME] = { 0 },
				 strAdmin[32] = { 0 },
				 strRank[32] = { 0 };

			Q_strncpyz( strName, e->client->pers.netname, sizeof( strName ) );
			Q_CleanStr( strName );
			Q_strncpyz( strAdmin, e->client->pers.account ? e->client->pers.account->v_User : "", sizeof( strAdmin ) );
			Q_CleanStr( strAdmin );
			/*if (e->client->pers.account->isCustomRank == 1)
			{
				Com_sprintf(strRank, sizeof(strRank), "%d^7:%s^7", e->client->pers.account->rank, e->client->pers.account->customRank);
			}
			else
			{
				Com_sprintf(strRank, sizeof(strRank), "%d^7:%s^7", e->client->pers.adminUser->rank,
					e->client->pers.adminUser->rank < ADMIN_MAX ?
					adminRanks[e->client->pers.adminUser->rank].c_str() : "");
			}*/
			Com_sprintf( strRank, sizeof( strRank ), e->client->pers.account->v_rank );

			Q_PrintBuffer(&pb, va("%-36s^7%-32s^7%-12s\n", strName, strAdmin, strRank));
		}
	}

	Q_DeletePrintBuffer(&pb);
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
	{ "amempower",	"empower",		PRIV_EMPOWER,	AM_Empower_f,		qtrue },
	{ "amforceteam","forceteam",	PRIV_FORCETEAM,	AM_ForceTeam_f,		qtrue },
	{ "amghost",	"ghost",		PRIV_GHOST,		AM_Ghost_f,			qtrue },
	{ "amgive",		"give",			PRIV_GIVE,		AM_Give_f,			qtrue },
	{ "amgod",		"god",			PRIV_GOD,		AM_God_f,			qtrue },
	{ "amgravity",  "gravity",		PRIV_GRAVITY,	AM_Gravity_f,		qtrue },
	{ "aminfo",		"info",			PRIV_NONE,		AM_Info_f,			qfalse },
	{ "amkick",		"kick",			PRIV_KICK,		AM_Kick_f,			qtrue },
	{ "amkillvote", "killvote",		PRIV_KILLVOTE,	AM_KillVote_f,		qtrue },
	{ "amlogin",	"x",			PRIV_NONE,		AM_Login,			qfalse },
	{ "amlogout",	"x",			PRIV_NONE,		AM_Logout,			qfalse },
	{ "ammerc",		"merc",			PRIV_MERC,		AM_Merc_f,			qtrue },
	{ "amnoclip",	"noclip",		PRIV_NOCLIP,	AM_Noclip_f,		qtrue },
	{ "amnotarget", "notarget",		PRIV_NOTARGET,	AM_Notarget_f,		qtrue },
	{ "amnpc",		"npc",			PRIV_NPCSP,		AM_NPCSpawn_f,		qtrue },
	{ "ampoll",		"poll",			PRIV_POLL,		AM_Poll_f,			qtrue },
	{ "amprotect",	"protect",		PRIV_PROTECT,	AM_Protect_f,		qtrue },
	{ "ampsay",		"psay",			PRIV_PSAY,		AM_Psay_f,			qtrue },
	{ "amremap",	"remap",		PRIV_REMAP,		AM_Remap_f,			qtrue },
	{ "amrename",	"rename",		PRIV_RENAME,	AM_Rename_f,		qtrue },
	{ "amsilence",	"silence",		PRIV_MUTE,		AM_Silence_f,		qtrue }, 
	{ "amslap",		"slap",			PRIV_SLAP,		AM_Slap_f,			qtrue }, 
	{ "amslay",		"slay",			PRIV_SLAY,		AM_Slay_f,			qtrue }, 
	{ "amsleep",	"sleep",		PRIV_SLEEP,		AM_Sleep_f,			qtrue },
	{ "amspeed",	"speed",		PRIV_SPEED,		AM_Speed_f,			qtrue },
	{ "amteleport", "teleport",		PRIV_TELEPORT,	AM_Teleport_f,		qtrue },
	{ "amunsilence", "unsilence",	PRIV_MUTE,		AM_Unsilence_f,		qtrue },
	{ "amvstr",		"vstr",			PRIV_VSTR,		AM_Vstr_f,			qtrue },
	{ "amwake",		"wake",			PRIV_SLEEP,		AM_Wake_f,			qtrue },
	{ "amweather",	"weather",		PRIV_WEATHER,	AM_Weather_f,		qtrue },
	{ "amwhois",	"whois",		PRIV_WHOIS,		AM_WhoIs_f,			qtrue },
	{ "v_account",	"accountmgr",	PRIV_ACTMGR,	v_AM_Accountmanage, qtrue },
};
static const size_t numVisionCommands = ARRAY_LEN( VisionCommands );

// Ja++ CONTENT (HAS TO BE RECODED):

//Author: Ja++ ?
static int v_cmdcmp( const void *a, const void *b) {
	return Q_stricmp( (const char *)a, ((VisionCommand_t*)b)->cmd );
}

//Author: Ja++
//Even though mildly modified.
qboolean AM_HasPrivilege( const gentity_t *ent, uint64_t privilege, qboolean authrequired ) {
	account_t *user = ent->client->pers.account;

	if ( !authrequired ) {
		return qtrue;
	}

	else if ( (user && ( user->v_privileges & privilege ) ) ) {
		return qtrue;
	}

	return qfalse;
}

void AM_PrintCommands( gentity_t *ent, printBufferSession_t *pb ) {
	const VisionCommand_t *command = NULL;
	account_t *user = ent->client->pers.account;
	int toggle = 0;
	unsigned int count = 0;
	const unsigned int limit = 102;
	size_t i;

	Q_PrintBuffer(pb, "^5[^3JaVision+^5] ^7commands:\n ");

	/*if (!user) {
		Q_PrintBuffer(pb, " " S_COLOR_RED "Unavailable " S_COLOR_WHITE "\n\n");
		return;
	}*/

	for (i = 0, command = VisionCommands; i < numVisionCommands; i++, command++) {
		if (AM_HasPrivilege(ent, command->v_privileges, command->v_admin_authorization)) {

			/*if (ent->client->pers.account)
			{*/

				const unsigned int limit = 92;

				if (i < 10)
				{
					const char *tmpMsg = va("" S_COLOR_RED "0%d ^%c%-16s", i, (++toggle & 1 ? COLOR_GREEN : COLOR_YELLOW), command->cmd);

					if (count >= limit) {
						tmpMsg = va("   \n %-18s", tmpMsg);
						count = 0;
					}

					count += strlen(tmpMsg);
					Q_PrintBuffer(pb, tmpMsg);
				}
				else
				{
					const char *tmpMsg = va("" S_COLOR_RED "%d ^%c%-16s", i, (++toggle & 1 ? COLOR_GREEN : COLOR_YELLOW), command->cmd);

					//newline if we reach limit
					if (count >= limit) {
						tmpMsg = va("   \n %-18s", tmpMsg);
						count = 0;
					}

					count += strlen(tmpMsg);
					Q_PrintBuffer(pb, tmpMsg);
				}

			//}
		}
	}

	Q_PrintBuffer(pb, S_COLOR_WHITE "\n\n");
}

//Author: Ja++ ?
// handle admin related commands.
// return true if the command exists and/or everything was handled fine.
// return false if command didn't exist, so we can tell them.
qboolean v_HandleCommands( gentity_t *ent, const char *cmd ) {
	VisionCommand_t *command = NULL;
	
	if ( ent == NULL ) { // call from console or anything else (which shouldn't happen)
		command = (VisionCommand_t *)bsearch( cmd, VisionCommands, numVisionCommands, sizeof(VisionCommands[0]), v_cmdcmp );
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
	else if ( !AM_HasPrivilege( ent, command->v_privileges, command->v_admin_authorization ) ) {
		trap->SendServerCommand( ent->s.number, "print \"^5[^3JaVision+^5]^7: Insufficient privileges / Not logged in.\n\"" );
		return qtrue;
	}



	command->func( ent );
	return qtrue;
}