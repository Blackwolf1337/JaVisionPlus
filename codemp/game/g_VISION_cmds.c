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
#include "g_VISION.h"

static account_t *accounts = NULL;	//This shouldn't be modified.

void AM_Login( gentity_t *ent ) {

}

void AM_Logout( gentity_t *ent ) {

}


static const VisionCommand_t VisionCommands[] = {
	{ "amlogin", 0xFF, AM_Login, qfalse },
	{ "amlogout", 0xFF, AM_Logout, qfalse },
};
static const size_t numVisionCommands = ARRAY_LEN( VisionCommands );

// Ja++ CONTENT (HAS TO BE RECODED):

//Author: Ja++ ?
static int v_cmdcmp(const void *a, const void *b) {
	return Q_stricmp((const char *)a, ((VisionCommand_t*)b)->cmd);
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