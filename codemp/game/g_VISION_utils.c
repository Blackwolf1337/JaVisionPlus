#include "g_local.h"

/* Copied from Ja++ source q_shared.cpp 
   I do not take any credits for this.  */
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