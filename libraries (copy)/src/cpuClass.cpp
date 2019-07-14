// file: cpuClass.cpp

#include "cpuClass.h"

// ------- DEFAULT HARDWARE DEFINITIONS FOR NodeMCU -------------------------------

    void CPU::init(  int baud, int ledp, int button ) 
	{  
      ledpin   = ledp;
      pinMode( ledpin&0x7FFF, OUTPUT);
      
      btnpin = button;
      pinMode( btnpin&0x7FFF, (btnpin & NEGATIVE_LOGIC) ? INPUT_PULLUP : INPUT );

      led( OFF );
      Serial.begin( baud );
      delay( 500 );
      blink(3);
      PRN("\r\nCPU initialized");
      heapref = ESP.getFreeHeap();
      heapmax = 0;
    }
    void CPU::blink( int times )
    {
      for( int i=0; i<times; i++ )
      {
        digitalWrite( ledpin&0x7FFF, (ledpin & NEGATIVE_LOGIC) ? OFF: ON );
        delay(100);
        digitalWrite( ledpin&0x7FFF, (ledpin & NEGATIVE_LOGIC) ? ON: OFF );
        delay(200);
      }
    } 
    void CPU::led( onoff_t onoff, int times ) // defines a led
    {
      if( onoff == 2 )
        blink( times );      
      else // onoff is either 0 or 1
        digitalWrite( ledpin&0x7FFF, (ledpin & NEGATIVE_LOGIC) ? !onoff: onoff );
    }  
    bool CPU::button()
    {
      return (btnpin & NEGATIVE_LOGIC) ? !digitalRead( btnpin&0x7FFF ) : digitalRead( btnpin&0x7FFF );
    }   
    bool CPU::buttonPressed()     // true if button is pressed and released
    {
        static bool justpressed = false;
        if( !justpressed )
        {
            if( button() )
            {
                led( ON );
                justpressed = true;
            }
            return false;
        }
        else // if( justpressed )
        {
            if( button() )  	// continuing pressed
                return false;
            justpressed = false;
            led( OFF );
            return true;        // released
        }   
    }
    // Buf CPU::prompt( char *prmpt )
    // {
        // Buf t(80);      // allocation of 80-bytes
        // PN( prmpt );
        // Serial.setTimeout( -1 );
        // size_t n = Serial.readBytesUntil( '\r', t.pntr, 79);
        // t.pntr[n] = 0;
        // PR("");
        // return t;       // buffer is deallocated after exit
    // }
	char * CPU::prompt( char *prmpt )
    {
        PR( prmpt );
		memset( temp, 0, MAX_PROMPT );
        Serial.flush();
		int i=0, c=0;
		for(;;)
		{
			if( Serial.available() && (c = Serial.read())>0 )
			{
				if( (c=='\r') || (i>=MAX_PROMPT-2) ) 
					break;
				temp[i++] = c;	
				PR( (char) c);
			}
			yield();
		}
		temp[i]=0;
		CRLF();
        return temp;       
    }
    void CPU::heapUpdate()
    {
      uint32_t heapnow = ESP.getFreeHeap();     // heap will decrease as used
      heapused = heapref - heapnow;
      if( heapused > heapmax )
        heapmax = heapused;
    }
    uint32_t CPU::heapUsedMax()
    {
      heapUpdate();
      return heapmax;
    }
    uint32_t CPU::heapUsedNow()
    {
      heapUpdate();
      return heapused;
    }
    void CPU::die( char *prompt, int times )
    {
      PF( "%s Heap used=%d (max used=%d)\r\n", prompt, heapUsedNow(), heapUsedMax() );
      for(;;)
      {
        blink( times );
        delay( 500 );
      }
    }
	void die( const char *s1, const char *s2, int s3, const char *s4 )
	{
		PF( "ASSERT(%s), line %d of %s\r\n", s1, s3, s4 );
		bool flip;
		for(;;)
		{
			digitalWrite( LED, flip );
			if( flip ) flip=false; else flip=true;
			//ESP.wdtDisable();
			//ESP.wdtFeed();
			yield();
			delay(200);
		}
	}

char *sf( char *sp, size_t sL, const char *format, ... )
{
	va_list args;
	va_start (args, format );
	vsnprintf( sp, sL-1, format, args );
	*(sp+sL-1) = 0;						// EOS
	va_end( args );
	return sp;
}

// -------------------------- PIPE OUT ----------------------------------------

    COUT::COUT( const char *fo )
    {
        strcpy( format, fo);
        cont = false;
    }
    COUT & COUT::operator << ( double x ) 
    {
        PF( format, x );
        cont = true;
        return *this;
    }
    COUT & COUT::operator << ( int x ) 
    {
        PF( " %d", x );
        cont = true;
        return *this;
    }
    COUT & COUT::operator << (char *s) 
    {
        if(*s==0)
        {
            PF( cr );
            cont = false;
            return *this;
        }
        if(*s==',')
        {
            PF( "%s", s );
        }
        else
        {
            if( cont )
                PF(" %s",  s );
            else
                PF( "%s", s );
        }
        cont = true;
        return *this;
    }
    COUT & COUT::operator << ( byte c ) 
    {
        PF(" %02X",  (int)c );
        cont = true;
        return *this;
    }
