#pragma once
#include <Arduino.h>

// --------------------- TIC CLASSES -------------------
class TICBASE
{
protected:
    uint32_t    prevtic;            // millis()/ms after ready() is true
    uint32_t    readytic;           // millis()/ms set every time ready() is called
    uint32_t    readycount;
    int ms;
   
public: 
    TICBASE();
    bool ready();
    bool countMod( int d, int r=0 );
	uint32_t count();
    bool overrun();
};

class TICsec : public TICBASE
{
public: 
    TICsec( int sec )
    {
        ms = 1000*sec;
		TICBASE();
    }
};

class TICms : public TICBASE
{
public: 
    TICms( int msec )
    {
        ms = msec;
		TICBASE();
    }
};
// --------------------- PROFILE CLASS -------------------
class PROF
{
#define DELAY_COUNTERS 5   
	int delays[ DELAY_COUNTERS ];
    int counts[ DELAY_COUNTERS+1 ];
    uint32_t T0, T1;    
public:
    PROF( int i0=5, int i1=25, int i2=50, int i3=75, int i4=100 );
    void start();
    void reset();
    void profile();
    void report( bool prf = false );
};