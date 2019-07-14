#pragma once
#include <macros.h>
#include <Arduino.h>

#define DEBUG_CHAR '@'

// -------------------------------- Base class for HEAP or STACK ----------------------

class BUFBASE						// base class
{
  protected:
    char *pntr;						// to be moved to "protected"
    size_t maxsiz;					// max size of available buffer
	const char *name;

  public:
   
	void init();
	
	char *copy( const char *s );		
	char *set( const char *format, ... );
    char *add( const char *format, ... );

	char *quotes();
	void print( const char *prompt="" );

	char * operator !();    	// pointer to buffer
	char *c_str();
	size_t length();			// strlen()
    size_t size();				// max size
};

// -------------------------------- BUF located at the HEAP --------------------------
class BUF: public BUFBASE
{
public:
    BUF( int size )
    {
        pntr = new char[ size ];
        maxsiz = size;
		name = "";			
		*pntr = 0;		
    }
	BUF( const char *first, int size=512 )
    {
        pntr = new char[ size ];
        maxsiz = size;
		name = first;		// for debugging purposes
        copy( first );		// this also initializes BUF
		if( *name==DEBUG_CHAR )
			PF("Allocated %s[%d]\r\n", name, maxsiz );
    }
    ~BUF()
    {
        if( maxsiz )			// this is required!
		{
			delete [] pntr;
			maxsiz = 0;
			if( *name==DEBUG_CHAR )
				PF("Deallocated %s\r\n", name );
		}
    }
    BUF & operator = ( const char *s ) 
    {
        copy( s );
        return *this;
    }
};

// -------------------------------- BUF located at the STACK --------------------------
template < int T>
class BUFT: public BUFBASE
{
private:
    char s[T];
public:
    BUFT( const char* first="", int ignore = 0 ) 
    {
        pntr = s;
        maxsiz = sizeof s;
		name = first;		// for debugging purposes
        copy( first );		// this also initializes BUF
		if( *name==DEBUG_CHAR )
			PF("Allocated %s[%d]\r\n", name, maxsiz );
		
    }
	~BUFT()
	{
		maxsiz=0;
	}
};

#define B32 BUFT<32>
#define B64 BUFT<64>
#define B80 BUFT<80>

