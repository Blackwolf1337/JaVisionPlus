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


static const VisionCommand_t VisionCommands[] = {
	{ "amlogin", "x", 0xBADC0DED, AM_Login, qfalse },
	{ "amlogout", "x",  0xBADC0DED, AM_Logout, qfalse },
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