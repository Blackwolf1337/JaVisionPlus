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

account_t *accounts = NULL;

/*
I was too lazy to build the signature and trailer into the structure.
*/
const char signature[22] = {	SOH,  0x56, 0x49,
								0x53, 0x49, 0x4F,
								0x4E, LF,	0x31,
								0x2E, 0x30, LF,
								STX,  0x41, 0x43, 
								0x43, 0x4F, 0x55, 
								0x4E, 0x54, 0x53, 
								LF };

const char trailer[6] = { FS, LF,  ETX,
						  LF, SUB, NUL };

/*
Author: Ja++, Raz0r(?)
Desc..: Add an account (will be tweaked)
*/
void v_Account_Create( char *user, char *password, uint64_t privileges, char *rank, int  *loginEffect, char *loginMsg ) {
	account_t *account = NULL;

	for ( account = accounts; account; account = account->next ) 
	{
		
		if ( !strcmp( user, account->v_User ) ) {
			//Overwrite
			trap->Print( "Overwriting account > %s\n", account->v_User );
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
	account->v_loginEffect = loginEffect;
	//Q_strncpyz( account->v_loginEffect, loginEffect, sizeof( account->v_loginEffect ) );
	Q_strncpyz( account->v_loginMsg, loginMsg, sizeof( account->v_loginMsg ) );
	Q_strncpyz( account->v_additionalInfo, "none", sizeof( account->v_additionalInfo ) );
	Q_strncpyz( account->v_banned, "0",  sizeof( account->v_banned ) );

	trap->Print( "Creating a new account at address 0x%p with size %zu \n", account, sizeof( account_t ) );
}

/*
Author: Blackwolf
Desc..: Removes an account out of memory and also binary.
*/
int v_Account_Delete( char *user ) {
	account_t *account = NULL, *prev = NULL, *next = NULL;

	for ( account = accounts; account; account = account->next ) {
		next = account->next;

		if ( !strcmp( user, account->v_User ) ) {
			
			trap->Print( "Deleting account: %s\n", user );

			free( account );
			if ( prev ) {
				prev->next = next;
			}

			// root node
			if ( account == accounts ) {
				accounts = next;
			}

			return 1;
		}

		prev = account;
	}

	trap->Print( "No such account found (%s)\n", user );
	return 0;
}

/*
Author: Ja++, Raz0r(?)
Desc..: Free allocated accounts
*/
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

/*
Author: Blackwolf
Desc..: Write data in binary mode into VisionData.vbin [VISION_DATA]
*/
void v_Write_Binary( qboolean silent ) {
	FILE *pfile;
	account_t *account = NULL;
	accountBin_t *accountBin = NULL;
	int i = 0;

	if ( !silent )
		trap->Print( "Preparing to open %s\n", VISION_DATA );

	pfile = fopen( VISION_DATA, "wb" );

	if ( !pfile ) {
		if( !silent )
			trap->Print( "Cannot open %s\n", VISION_DATA );
		return;
	}

	if ( !silent )
		Com_Printf( "Writing Binary > " VISION_DATA "\n" );
	
	fseek( pfile, 0, SEEK_SET );
	fwrite( signature, sizeof( char ), sizeof( signature ), pfile );

	for ( account = accounts; account; account = account->next, i++ )
	{
		accountBin = account;
		fseek( pfile, sizeof( signature ) + ( sizeof( accountBin_t )*i ), SEEK_SET );
		fwrite( accountBin, sizeof( char ), sizeof( accountBin_t ), pfile );
	}

	fwrite( trailer, sizeof( char ), sizeof( trailer ), pfile );

	fclose( pfile );
}


/*
Author: Blackwolf
Desc..: Read data out of the Vision binary.
*/
void v_Read_Binary( qboolean silent ) {
	FILE *pfile;
	account_t *account = NULL;
	char *membuffer = NULL; //temporary memoryblock buffer for the signature and trailer.
	size_t fileSize;
	size_t i;

	v_memfree_Accounts();

	pfile = fopen( VISION_DATA, "rb" );
		
	if ( !pfile ) {
		trap->Print( "Cannot open %s\n", VISION_DATA );
		return;
	}

	/*
	signature	= 22 bytes
	accountBin	= 1240 bytes
	trailer		= 6 bytes

	([FILE]-signature-trailer)/account = amount of accounts

	*/

	fseek( pfile, 0, SEEK_SET );									//set filepointer to the beginning of the file
	membuffer = ( char *)malloc( sizeof( signature ) );
	fread( membuffer, sizeof( char ), sizeof( signature ), pfile );	//store content into membuffer
	
	if ( memcmp( membuffer, signature, sizeof( signature ) ) )  {	//memcmp instead of strcmp, we're working with binary blocks not strings	
		if( !silent )
			trap->Print( "No signature.\n" );
		return;
	} else if ( !silent ) { trap->Print( "Signature is fine.\n" );  }
	
	
	realloc( membuffer, sizeof( trailer ) );						//Realloc the block.
	memset( membuffer, 0, sizeof( trailer ) );						//Make sure it's squeaky clean. Zero it out.

	fseek( pfile, 0 - sizeof( trailer ), SEEK_END );				//set filepointer to the end of the file
	fread( membuffer, sizeof( char ), sizeof( trailer ), pfile );	//repeat step Line>142

	if ( ( memcmp( membuffer, trailer, sizeof( trailer ) ) ) ) {
		if( !silent )
			trap->Print( "No trailer.\n" );
		return;
	} else if ( !silent ) { trap->Print( "Trailer is fine.\n" ); }
		
	fseek( pfile, 0, SEEK_END );
	fileSize = ftell( pfile );
	i = ( ( fileSize-sizeof( signature )-sizeof( trailer ) ) / sizeof( accountBin_t ) ); //calculate the amount of admins
	fseek( pfile, sizeof( signature ), SEEK_SET );										 //set filepointer at the end of the signature header
	
	for ( int j = 1; j <= i; j++ )
	{
		account = (account_t *)malloc( ACCOUNTSIZE );
		memset( account, 0, sizeof( account_t ) );
		fread( account, sizeof( char ),  sizeof( accountBin_t ), pfile );
		account->next = accounts;
		accounts = account;
		
		if( !silent )
			trap->Print( "%s %s %lld %s %d %s %s %s\n\tSIZE->%zu\n",	account->v_User, 
																		account->v_Password,
																		account->v_privileges,
																		account->v_rank,
																		account->v_loginEffect,
																		account->v_loginMsg,
																		account->v_additionalInfo,
																		account->v_banned,
																		account);
	}

	fclose( pfile );
	
	if( !silent )
		trap->Print( "Success at parsing binaries.\n" );
}