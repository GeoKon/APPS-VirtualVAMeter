#include "macros.h"
#include "bufClass.h"

// ---------------- BASE Buf Class Definition --------------------------------------

	void BUFBASE::init()
	{
		if( maxsiz )
			*pntr = 0;
	}
    char *BUFBASE::copy( const char *s ) 
    {
        if( maxsiz )
        {
			strncpy( pntr, (const char*)s, maxsiz-1 );		
			*(pntr+maxsiz-1) = 0;
		}    
        return pntr;
    }
    char *BUFBASE::set( const char *format, ... )
    {
        if( maxsiz )
        {
            va_list args;
            va_start (args, format );
            vsnprintf(pntr, maxsiz - 1, format, args);
			*(pntr+maxsiz-1) = 0;
            va_end( args );
        }
        return pntr;
    }
    char *BUFBASE::add( const char *format, ... )
    {
        if( maxsiz )
        {
			va_list args;
			va_start (args, format );
			char *p = pntr + strlen( pntr );    		// where to store new data
            int avail = maxsiz - 1 - strlen( pntr );	// left over space
			vsnprintf(p, avail, format, args);
			*(pntr+maxsiz-1) = 0;						// EOS
			va_end( args );
        }
        return pntr;
    }
	char *BUFBASE::quotes()
    {
		int i=0;
		for( char *p = pntr; (*p) && (i<maxsiz); p++, i++ )
		{	
			if( *p=='\'' )
				*p= '\"';
		}
        return pntr;
    }
    void BUFBASE::print( const char *prompt )
    {
        PR( prompt );   // print prompt
        if( pntr )      // ...then string
        {
			PR( pntr );
			if( pntr[ strlen(pntr)-1 ] != '\n' )
				PR("\r\n");        		// ...then CRLF  
		}
    }
    char * BUFBASE::operator ! ()    	// pointer to buffer
    {
        return pntr;
    }
    char *BUFBASE::c_str()
    {
        return pntr;
    }
    size_t BUFBASE::length()
    {
        return strlen( pntr );
    }
	size_t BUFBASE::size()
    {
        return maxsiz;
    }