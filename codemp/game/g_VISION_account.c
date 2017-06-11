// JaVision+ administration system
//
//
//
//
//
//
//
//
//
// Admin data is saved in plain custom format (vision data) in <fs_homepath>/<fs_game>/VisionData.vbin
// Possible key encryption (AES 256)
// --- Functionality ---

#include "g_local.h"
#include "g_VISION.h"


char signature[22] = {	SOH,  0x56, 0x49,
						0x53, 0x49, 0x4F,
						0x4E, LF,	0x31,
						0x2E, 0x30, LF,
						STX,  0x41, 0x43, 
						0x43, 0x4F, 0x55, 
						0x4E, 0x54, 0x53, 
						LF };

char trailer[6] = { FS, LF,  ETX,
					LF, EOF, NUL };

void v_Account_Create( char *user, char *password, uint64_t privileges, char *rank, char *loginEffect, char *loginMsg ) {
	account_t *account = NULL;

	for ( account = accounts; account; account = account->next ) 
	{
		
		if ( !strcmp( user, account->v_User ) ) {
			//Overwrite
			trap->Print("Overwriting existing admin. ");
			/*trap->Print( "Overwriting account %s with: %s %s %llu %s %s %s\n",	user, account->v_User
																					, account->v_Password
																					, account->v_privileges
																					, account->v_rank
																					, account->v_loginEffect
																					, account->v_loginMsg );*/
			break;
		}
	}

	if ( !account ) {
		account = (account_t *)malloc( sizeof( account_t ) );
		memset( account, 0, sizeof( account_t ) );
		account->next = accounts;
		accounts = account;
	}

	Q_strncpyz( account->v_User, user, sizeof( account->v_User ) );
	Q_strncpyz( account->v_Password, password, sizeof( account->v_Password ) );
	account->v_privileges = privileges;
	Q_strncpyz( account->v_rank, rank, sizeof( account->v_rank ) );
	Q_strncpyz( account->v_loginEffect, loginEffect, sizeof( account->v_loginEffect ) );
	Q_strncpyz( account->v_loginMsg, loginMsg, sizeof( account->v_loginMsg ) );

	trap->Print( "Creating a new account at address 0x%p with size %zu \n", account, sizeof(account) );
}

void v_Write_Binary( qboolean silent ) {
	FILE *pfile;
	account_t *account = NULL;

	pfile = fopen( VISION_DATA, "wb" );

	if (!pfile) {
		trap->Print( "Cannot open %s", VISION_DATA );
		return;
	}

	if (!silent) {
		Com_Printf( "Writing Binary > " VISION_DATA "\n" );
	}

	fwrite( signature, sizeof( signature ), 1, pfile );

	for (account = accounts; account; account = account->next)
	{
		fwrite( account, sizeof( account_t ), 1, pfile );
	}

	fwrite( trailer, sizeof(trailer), 1, pfile );

	fclose(pfile);
}