#pragma once

#include "mgnClass.h"
#include "Globals.h"

#define M_VOLTS myp.chn[2].engunits
#define M_AMPS  myp.chn[4].engunits

extern MGN mgn;

class RinTEST
{
	int state;          // state
	int seconds;        // second counter from the beginning of a state
	bool active;        // true if test cycle is active
    
    float avevolts;
    int   avecount;
    
    int _initload;       // initial delay without taking a measurement
    int _measload;       // measuring with load
    int _initrelax;      // relax time without taking a measurement
    int _measrelax;      // measuring at relax period
    int _ncycles;        // number of cycles
    int _ncyclegap;      // gap between cycles

public:	
	RinTEST()           // consider adding mgn pointer
	{
	    state = 0;
	    active = false;
	}
	
	void startCycle( int iL, int mL, int iR, int mR, int nC, int nG )		// called by the CLI
	{
	    _initload  = iL; _measload  = mL;
        _initrelax = iR; _measrelax = mR;
        _ncycles   = nC; _ncyclegap = nG;
        
		active  = true;
		state   = 0;
		seconds = 0;

        mgn.tableClear();
	}
	bool ready()			// checked by loop()
	{
		return active;
	}
	void update()			        // should be called every second after if( ready() )
	{
		int nextstate = state;
		
		mgn.clear();                // clear mgn buffer
		switch( state )
		{
		case 0:                     // waiting for N-seconds elapsed
			
			mgn.tableSet( "State", state );
			mgn.tableSet( "Elapsed Time", seconds, "seconds" );
            mgn.tableSet( "Vload",   M_VOLTS, "Voltage under load");
            mgn.tableSet( "Iload",   M_AMPS,  "Current");
            
			digitalWrite( 14, 1 );  //relayON;
			seconds++;
			if( seconds > _initload )
			{
			    nextstate = 1;
				seconds = 0;
                
			}
			break;
          
		case 1:                     // measuring under load
            mgn.tableSet( "State", state );
            mgn.tableSet( "Elapsed Time", seconds, "seconds" );
            mgn.tableSet( "Vload",   M_VOLTS, "Voltage under load (measuring)");
            mgn.tableSet( "Iload",   M_AMPS,  "Current (measuring)");
            
            if( !seconds ) 
            {
                avevolts = M_VOLTS;
            }
            else
                avevolts += M_VOLTS;
                
			seconds++;
            
			if( seconds > _measload )
			{
				avevolts = avevolts/ seconds;				
				nextstate = 2;
				seconds = 0;
			}
			break;
		case 2:	                    // no load
            mgn.tableSet( "State", state );
            mgn.tableSet( "Elapsed Time", seconds, "seconds" );
            
            if( !seconds ) 
                mgn.tableSet( "Vaveload",  avevolts, "Average Voltage");

            digitalWrite( 14, 0 );  //relayOFF;
            seconds++;
			if( seconds > _initrelax )
			{
			    nextstate = 3;
				seconds = 0;
			}
            mgn.tableSet( "Vrelax",   M_VOLTS, "Voltage relaxed");
			break;
		case 3:                     // take measurement (no load)
            mgn.tableSet( "State", state );
            mgn.tableSet( "Elapsed Time", seconds, "seconds" );
            mgn.tableSet( "Vrelax",   M_VOLTS, "Voltage relaxed (measuring)");
            
            if( !seconds ) 
            {
                avevolts = M_VOLTS;
            }
            else
                avevolts += M_VOLTS;
                
            seconds++;
            if( seconds > _measrelax )
            {
                avevolts = avevolts/ seconds;   
                mgn.tableSet( "Vaverelax", avevolts, "Average Relaxed Voltage");            
                nextstate = 4;
                seconds = 0;
            }
            break;
        default:
        case 4:                     // all done
            active = false;
            break;
	    }
        mgn.print();                // print mgn buffer
	    state = nextstate;
	}
};
extern RinTEST rin;
