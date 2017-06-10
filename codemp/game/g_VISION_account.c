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
// Admin data is saved in plain custom format (vision data) in <fs_homepath>/<fs_game>/admins.vd
// Possible key encryption (AES 256)
// --- Functionality ---

#include "g_local.h"
#include "g_VISION.h"

void v_Account_Create( char user, char password, uint64_t privileges, byte rank, byte loginEffect, char loginMsg ) {
	account_t *account = NULL;

	for ( account = accounts; account; account = account->next ) 
	{
		if ( !strcmp( user, account->v_User ) ) {
			//Overwrite
			trap->Print( "Overwriting account %s with: %s %s %llu %s %s %s\n",	user, account->v_User
																					, account->v_Password
																					, account->v_privileges
																					, account->v_rank
																					, account->v_loginEffect
																					, account->v_loginMsg );

			break;
		}
	}

	if ( !account ) {
		account = (account_t *)malloc( sizeof( account_t ) );
		memset( account, NULL, sizeof( account_t ) );
		account->next = accounts;
		accounts = account;
		trap->Print( "Creating a new account at address %p with size %llu \n", account, sizeof ( account_t ) );
	}

	Q_strncpyz( account->v_User, user, sizeof( account->v_User ) );
	Q_strncpyz( account->v_Password, user, sizeof( account->v_Password ) );
	memcpy( account->v_privileges, privileges, sizeof( account->v_privileges) );
	memcpy( account->v_rank, rank, sizeof( account->v_rank ) );
	memcpy( account->v_loginEffect, loginEffect, sizeof( account->v_loginEffect) );
	Q_strncpyz( account->v_loginMsg, loginMsg, sizeof(account->v_loginMsg) );
}