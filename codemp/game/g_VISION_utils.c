#include "g_local.h"

// A collection of utility functions, to make life easier
// copied from Ja++ source q_shared.cpp, g_utils.cpp 
// I am not the author nor I take credits for the listed functions below.


// replace strings in a string
// caller must free the return value
char *Q_strrep( const char *subject, const char *search, const char *replace ) {
	char *tok = NULL, *newstr = NULL;
	size_t searchLen, replaceLen;

	if ( !( tok = ( char * )strstr( subject, search ) ) )
		return strdup( subject );

	searchLen = strlen( search );
	replaceLen = strlen( replace );

	newstr = ( char * )malloc( strlen( subject ) - searchLen + replaceLen + 1);
	if ( !newstr )
		return NULL;

	memcpy( newstr, subject, tok - subject );
	memcpy( newstr + ( tok - subject ), replace, replaceLen );
	memcpy( newstr + ( tok - subject ) + replaceLen, tok + searchLen, strlen( subject ) - searchLen - ( tok - subject ) );
	memset( newstr + strlen( subject ) - searchLen + replaceLen, 0, 1);

	return newstr;
}

void Q_ConvertLinefeeds( char *string ) {
	qboolean doPass = qtrue;
	char *r, *w; // read, write

	while ( doPass ) {
		doPass = qfalse;
		r = w = string;
		while ( *r ) {
			if ( *r == '\\' && *( r + 1 ) && *( r + 1 ) == 'n' ) {
				doPass = qtrue;
				*w = '\n';
				r += 2;
				w++;
			}
			else {
				// Avoid writing the same data over itself
				if ( w != r )
					*w = *r;
				w++, r++;
			}
		}
		// Add trailing NUL byte if string has shortened
		if ( w < r )
			*w = '\0';
	}
}

// true if s is an integer
qboolean Q_StringIsInteger( const char *s ) {
	int i, len;
	qboolean foundDigit = qfalse;

	for (i = 0, len = strlen(s); i < len; i++) {
		if (i == 0 && s[i] == '-') {
			continue;
		}
		if (!isdigit(s[i])) {
			return qfalse;
		}

		foundDigit = qtrue;
	}

	return foundDigit;
}

// put all of this into a switch statement wtf?
static qboolean cmpSubCase( const char *s1, const char *s2 ) {
	return ( strstr( s1, s2 ) != NULL ) ? qtrue : qfalse;
}
static qboolean cmpSub( const char *s1, const char *s2 ) {
	return ( Q_stristr( s1, s2 ) != NULL ) ? qtrue : qfalse;
}
static qboolean cmpWholeCase( const char *s1, const char *s2 ) {
	return ( !strcmp( s1, s2 ) ) ? qtrue : qfalse;
}
static qboolean cmpWhole( const char *s1, const char *s2 ) {
	return ( !Q_stricmp( s1, s2 ) ) ? qtrue : qfalse;
}

int G_ClientFromString( const gentity_t *ent, const char *match, uint32_t flags ) {
	char cleanedMatch[MAX_NETNAME];
	int i;
	gentity_t *e;
	qboolean substr = !!( flags & FINDCL_SUBSTR );
	qboolean firstMatch = !!( flags & FINDCL_FIRSTMATCH );
	qboolean print = !!( flags & FINDCL_PRINT );
	qboolean caseSensitive = !!( flags & FINDCL_CASE );
	qboolean( *compareFunc )( const char *s1, const char *s2 );
	if ( caseSensitive)
		compareFunc = substr ? cmpSubCase : cmpWholeCase;
	else
		compareFunc = substr ? cmpSub : cmpWhole;
	
	// First check for clientNum match
	if ( Q_StringIsInteger( match ) ) {
		i = atoi( match );
		if ( i >= 0 && i < level.maxclients ) {
			e = g_entities + i;
			if ( e->inuse && e->client->pers.connected != CON_DISCONNECTED )
				return i;
			if ( print ) {
				if ( ent ) {
					trap->SendServerCommand( ent->s.number, va( "print \"Client %d is not on the server\n\"", i ) );
				}
				else {
					trap->Print( va( "Client %d is not on the server\n", i ) );
				}
			}
			return -1;
		}
		else {
			if ( print ) {
				if ( ent ) {
					trap->SendServerCommand(ent - g_entities, va("print \"Client %d is out of range [0, %d]\n\"", i, level.maxclients - 1));
				}
				else {
					trap->Print( va( "Client %d is out of range [0, %d]\n", i, level.maxclients - 1 ) );
				}
			}
			return -1;
		}
	}

	// Failed, check for a name match
	Q_strncpyz( cleanedMatch, match, sizeof( cleanedMatch ) );
	Q_StripColor( cleanedMatch );

	if ( firstMatch ) {
		for (i = 0, e = g_entities; i < level.maxclients; i++, e++) {
			if (compareFunc(e->client->pers.netname_nocolor, cleanedMatch) && e->inuse
				&& e->client->pers.connected != CON_DISCONNECTED) {
				return i;
			}
		}
	}
	else {
		int numMatches, matches[MAX_CLIENTS];

		// find all matching names
		for ( i = 0, numMatches = 0, e = g_entities; i < level.maxclients; i++, e++ ) {
			if ( !e->inuse || e->client->pers.connected == CON_DISCONNECTED )
				continue;
			if ( compareFunc( e->client->pers.netname_nocolor, cleanedMatch ) )
				matches[numMatches++] = i;
		}

		// success
		if ( numMatches == 1 )
			return matches[0];

		// multiple matches, can occur on substrings and if duplicate names are allowed
		else if ( numMatches ) {
			char msg[MAX_TOKEN_CHARS];
			Com_sprintf( msg, sizeof(msg), "Found %d matches:\n", numMatches );
			for ( i = 0; i < numMatches; i++ ) {
				Q_strcat( msg, sizeof(msg), va(S_COLOR_WHITE "  (" S_COLOR_CYAN "%02i" S_COLOR_WHITE ") %s\n",
					matches[i], g_entities[matches[i]].client->pers.netname )
				);
			}
			if ( ent ) {
				trap->SendServerCommand( ent->s.number, va( "print \"%s\"", msg ) );
			}
			else {
				trap->Print( va( "print %s", msg ) );
			}
			return -1;
		}
	}

	//Failed, target client does not exist
	if ( print ) {
		if ( ent ) {
			trap->SendServerCommand( ent->s.number, va("print \"Client %s does not exist\n\"", cleanedMatch ) );
		}
		else {
			trap->Print( va( "Client %s does not exist\n", cleanedMatch ) );
		}
	}
	return -1;
}

void Q_NewPrintBuffer( printBufferSession_t *session, size_t length, void( *callback )( const char *buffer, int clientNum ), int clientNum ) {
	memset( session, 0, sizeof( *session ) );
	session->buffer = (char *)malloc( length );
	session->buffer[0] = '\0';
	session->length = 0u;
	session->maxLength = length;
	session->callback = callback;
	session->clientNum = clientNum;
}

void Q_PrintBuffer( printBufferSession_t *session, const char *append ) {
	const size_t appendLen = strlen( append );
	if ( session->length + appendLen >= session->maxLength ) {
		if ( session->callback ) {
			session->callback( session->buffer, session->clientNum );
		}
		session->length = 0u;
		session->buffer[0] = '\0';
	}
	session->length += appendLen;
	Q_strcat( session->buffer, session->maxLength, append );
}

void Q_DeletePrintBuffer( printBufferSession_t *session ) {
	if ( session->callback ) {
		session->callback( session->buffer, session->clientNum );
	}
	if ( session->buffer ) {
		free( session->buffer );
		session->buffer = NULL;
	}
}

void PB_Callback( const char *buffer, int clientNum ) {
	if ( clientNum == -1 ) {
		trap->Print( buffer );
	}
	else {
		trap->SendServerCommand( clientNum, va( "print \"%s\"", buffer ) );
	}
}
