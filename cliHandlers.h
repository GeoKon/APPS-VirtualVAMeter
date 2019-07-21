#pragma once
    #include <cliClass.h>
    #include <tcpClass.h>       // from GKE Lib1
    #include "filtTemp.h"
    
#define BSET( MASK ) (scmask_t) (((int)STREAMCODE) | (  (int)MASK) )
#define BRES( MASK ) (scmask_t) (((int)STREAMCODE) & (~ (int)MASK) )

void updateMeters( int, char **);
void updateGraph( int, char ** );
bool setRelay();
void initPanel( int, char **);
void initEEParms();

namespace CLIADC
{
	extern CMDTABLE Table[];
}

#define TOENG(i) myp.chn[i].scale * myp.chn[i].volts + myp.chn[i].offset

// ---------------------------- exported functions ----------------------------------

// ---------------------------- exported classes ----------------------------------

    extern TCP tcp;                                    // used by this and main() 
    
    
