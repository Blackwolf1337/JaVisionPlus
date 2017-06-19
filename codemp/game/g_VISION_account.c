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
					LF, SUB, NUL };

void v_Account_Create( char *user, char *password, uint64_t privileges, char *rank, char *loginEffect, char *loginMsg ) {
	account_t		*account	= NULL;

	for ( account = accounts; account; account = account->next ) 
	{
		
		if ( !strcmp( user, account->v_User ) ) {
			//Overwrite
			trap->Print("Overwriting existing admin.\n");
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
		account = (account_t *)malloc( ACCOUNTSIZE );
		memset( account, 0, ACCOUNTSIZE );
		account->next = accounts;
		accounts = account;
	}

	Q_strncpyz( account->v_User, user, sizeof( account->v_User ) );
	Q_strncpyz( account->v_Password, password, sizeof( account->v_Password ) );
	account->v_privileges = privileges;
	Q_strncpyz( account->v_rank, rank, sizeof( account->v_rank ) );
	Q_strncpyz( account->v_loginEffect, loginEffect, sizeof( account->v_loginEffect ) );
	Q_strncpyz( account->v_loginMsg, loginMsg, sizeof( account->v_loginMsg ) );
	Q_strncpyz( account->v_additionalInfo, "none", sizeof( account->v_additionalInfo ) );
	Q_strncpyz( account->v_banned, "0",  sizeof( account->v_banned ) );

	trap->Print( "Creating a new account at address 0x%p with size %zu \n", account, sizeof(account) );
}

void v_memfree_Accounts( void ) {
	account_t *account = accounts;

	while ( account )
	{
		account_t *next = account->next;

		free( account );
		account = next;
	}

	accounts = NULL;
}

void v_Write_Binary( qboolean silent ) {
	FILE *pfile;
	account_t *account = NULL;
	accountBin_t *accountBin = NULL;
	int i;

	pfile = fopen( VISION_DATA, "wb" );

	if ( !pfile ) {
		trap->Print( "Cannot open %s", VISION_DATA );
		return;
	}

	if ( !silent ) {
		Com_Printf( "Writing Binary > " VISION_DATA "\n" );
	}
	
	fseek( pfile, 0, SEEK_SET );
	fwrite( signature, sizeof( char ), sizeof( signature ), pfile );

	for ( account = accounts, i = 0; account; account = account->next, i++ )
	{
		accountBin = account;
		fseek( pfile, sizeof( signature ) + ( sizeof( accountBin_t )*i ), SEEK_SET );
		fwrite( accountBin, sizeof( char ), sizeof( accountBin_t ), pfile );
	}

	fwrite( trailer, sizeof( char ), sizeof( trailer ), pfile );

	fclose( pfile );
}

void v_Read_Binary( qboolean silent ) {
	FILE *pfile;
	account_t *account = NULL;
	char *membuffer = NULL; //front-buffer
	size_t fileSize;
	size_t i;

	v_memfree_Accounts();

	pfile = fopen( VISION_DATA, "rb" );
		
	if (!pfile) {
		trap->Print( "Cannot open %s\n", VISION_DATA );
		return;
	}

	/*
	signature	= 22 bytes
	account		= 1240 bytes
	trailer		= 6 bytes

	([FILE]-signature-trailer)/account = amount of accounts

	*/

	fseek( pfile, 0, SEEK_SET );
	membuffer = ( char *)malloc( sizeof( signature ) );				//Since we fread parses through the memory we gotta allocate some.
	fread( membuffer, sizeof( char ), sizeof( signature ), pfile );	//fread stores the buffer into the membuffer.
	
	if ( !( memcmp( membuffer, signature, sizeof( signature ) ) ) )  {	//memcmp instead of strcmp, we're working with binary blocks not strings
		trap->Print( "Signature seems to be OK.\n" );
	}
	else { 
		trap->Print( "No signature.\n" ); 
		return; 
	}
	
	realloc( membuffer, sizeof( trailer ) );	//Realloc the block.
	memset( membuffer, 0, sizeof( trailer ) );	//Make sure it's squeaky clean.

	fseek( pfile, 0 - sizeof( trailer ), SEEK_END );				
	fread( membuffer, sizeof( char ), sizeof( trailer ), pfile );

	if ( !( memcmp( membuffer, trailer, sizeof( trailer ) ) ) ) {
		trap->Print( "Trailer seems to be OK.\n" );
	}
	else { 
		trap->Print( "No trailer.\n" );
		return;
	}
		
	fseek( pfile, 0, SEEK_END );
	fileSize = ftell( pfile );
	i = ( ( fileSize-sizeof( signature )-sizeof( trailer ) ) / sizeof( accountBin_t ) );
	fseek( pfile, sizeof( signature ), SEEK_SET );

	for (int j = 1; j <= i; j++)
	{
		account = (account_t *)malloc( ACCOUNTSIZE );
		memset( account, 0, sizeof( account_t ) );
		fread( account, sizeof( char ),  sizeof( accountBin_t ), pfile );
		account->next = accounts;
		accounts = account;
		trap->Print( "%s %s %lld %s %s %s %s\n\tSIZE->%zu | %zd | %p | %d\n",	account->v_User, 
																				account->v_Password,
																				account->v_privileges,
																				account->v_rank,
																				account->v_loginMsg,
																				account->v_additionalInfo,
																				account->v_banned,
																				account);
		}

	//free(membuffer);
	fclose( pfile );

	trap->Print( "OK.\n" );

	//fread( trailer, sizeof( trailer ), 1, pfile );
}