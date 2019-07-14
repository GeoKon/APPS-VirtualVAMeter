#include <macros.h>
#include <ticClass.h>

// ------------------ TIC CLASS DEFINITION ----------------

// uint32_t    prevtic;            // millis()/ms after ready() is true
// uint32_t    readytic;           // millis()/ms set every time ready() is called
// uint32_t    readycount;
// int ms;
   
    TICBASE::TICBASE()
    {
        prevtic     = 0;
        readycount  = 0;
    }
    bool TICBASE::ready()
    {
        readytic = millis()/ms;
        if( prevtic != readytic )
        {
            prevtic = readytic;
            readycount++;
            return true;
        }
        return false;
    }
    bool TICBASE::countMod( int d, int r )
    {
        return (readycount % d)==r;
    }
	uint32_t TICBASE::count()
	{
		return readycount;
	}
    bool TICBASE::overrun()
    {
        return (millis()/ms) != prevtic;    
    }

// ------------------ PROFILE CLASS DEFINITION ----------------
  
	// int delays[ DELAY_COUNTERS ];
    // int counts[ DELAY_COUNTERS+1 ];
    // uint32_t T0, T1;    

    PROF::PROF( int i0, int i1, int i2, int i3, int i4 )
    {
        delays[0]=i0;
        delays[1]=i1;
        delays[2]=i2;
        delays[3]=i3;
		delays[4]=i4;
        reset();
    }
    void PROF::start()
    {
        T0 = millis();    
    }
    void PROF::reset()
    {
        for(int i=0; i<DELAY_COUNTERS+1; i++ ) 
            counts[i] = 0;  
    }
    void PROF::profile()
    {
        uint32_t D = millis()-T0;
        
        int i;
		for( i=0; i<DELAY_COUNTERS; i++ )	// 0...4
		{
			if( D<delays[i] )
			{
				counts[i]++;
				break;
			}
		}
		if( i==DELAY_COUNTERS )				// 5
			counts[ DELAY_COUNTERS ]++;
    }
    void PROF::report( bool prf )
    {
        uint32_t D = millis()-T0;
        //PF("Delay %d\r\n", D );
        if( prf )
		{
			for(int i=0; i<DELAY_COUNTERS; i++)
				PF( "[<%dms]:%d\t", delays[i], counts[i] );
			
			PF( "[>=%dms]:%d\r\n", delays[DELAY_COUNTERS-1], counts[DELAY_COUNTERS] );
		}
    }
