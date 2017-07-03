// JaVision+ administration system
//
//
//
//
//  * * * * PLACE HOLDER * * * * 
//
//
//
//
// Admin data is saved in plain custom format (vision data) in <fs_homepath>/<fs_game>/admins.vd
// Possible key encryption (AES 256)
// --- Functionality ---

#include "g_local.h"

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
	VectorClear(&cl->ps.velocity);
	cl->ps.forceHandExtend = HANDEXTEND_KNOCKDOWN;
	cl->ps.forceHandExtendTime = INT32_MAX;
	cl->ps.forceDodgeAnim = 0;
}


void AM_Sleep_f( gentity_t *ent ) {
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
	if (arg1[0] == '-' && arg1[1] == '1') {
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

void v_AM_Accountmanage( gentity_t *ent, account_t *account ) {
	//place holder
}

static const VisionCommand_t VisionCommands[] = {
	{ "amlogin", "x", 0xBADC0DED, AM_Login, qfalse },
	{ "amlogout", "x",  0xBADC0DED, AM_Logout, qfalse },
	{ "amsleep", "sleep", PRIV_SLEEP, AM_Sleep_f, qfalse },
	{ "v_account", "accountmgr", PRIV_ACTMGR, v_AM_Accountmanage, qfalse },
};
static const size_t numVisionCommands = ARRAY_LEN( VisionCommands );

// Ja++ CONTENT (HAS TO BE RECODED):

//Author: Ja++ ?
static int v_cmdcmp( const void *a, const void *b ) {
	return Q_stricmp( (const char *)a, ((VisionCommand_t*)b)->cmd );
}


//Author: Ja++ ?
// handle admin related commands.
// return true if the command exists and/or everything was handled fine.
// return false if command didn't exist, so we can tell them.
qboolean v_HandleCommands( gentity_t *ent, const char *cmd ) {
	VisionCommand_t *command = NULL;
	gentity_t *e;
	int i;
	
	if ( ent == NULL ) { // call from console or anything else (which shouldn't happen)
		command = (VisionCommand_t *)bsearch( cmd, VisionCommands, numVisionCommands, sizeof( VisionCommands[0] ), v_cmdcmp );
		if (command) {
			command->func( NULL );
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