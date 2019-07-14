/* ---------------------------------------------------------------------------------
 *          CLI Handlers
 *  
 * See VirtualVAMeter.cpp for file and class hierarchy
 * Copyright (c) George Kontopidis 2019, All Rights Reserved
 * ----------------------------------------------------------------------------------
 */
    #include <gkeL1io.h>          // from GKE Lib1 (includes of usual base functions)
    #include <tcpClass.h>       // from GKE Lib1
    #include <mgnClass.h>       // from GKE Lib1

//    #include <filtClass.h>      // from GKE Lib1
    #include "filtTemp.h"      // localy here

    #include <Ticker.h>         // from Arduino library
    
    #include "mscSupport.h"     // in this local directory
    #include "cliHandlers.h"    // in this local directory
    #include "batTest.h"        // in this local directory
    #include "globals.h"        // in this local directory
    
// --------- allocation of classes used by this module or main() -------------------
    
    TCP tcp;                                    // used by this and main() 
    MGN mgn;                                    // used only by this module
    RinTEST rin;
    
// --------------- references to classes allocated elsewhere -------------------------

    extern CPU cpu;         // allocated in global.cpp
    extern CLI cli;         // allocated in global.cpp
    extern EXE exe;         // allocated in global.cpp
    extern GLOBALS myp;     // allocated in global.cpp
    extern SCAN scan;       // allocated in mscSupport.cpp
    extern ADS15 ads;       // allocated in mscSupport.cpp
    
    extern DIIR qvolts1, qvolts2, qamps;

// ------------------------ IMPORTANT CLI MACROS -------------------------------------

// This modulo does not use the BUF buf directly. It used args[0] of the CLI handler.

#define CLI_ARGS int n, char **args
#define DECLARE(A) BUF *A; A = (BUF *)args[0]
#define RESPONSE( format, ... ) bp->add( format, ##__VA_ARGS__ )
#define M_VOLTS M_VOLTS1

float getVolts()
{
    return myp.selectV1V2 ? M_VOLTS2 : M_VOLTS1;
}
int getADCV()
{
    return myp.selectV1V2 ? ADC_VOLTS2 : ADC_VOLTS1;
}
bool setRelay()
{
    if( myp.relayON )
    {
        if( myp.relaySel )  // true for V2
        {
            if( myp.relayComp ) // true for LT
            {
                if( M_VOLTS2 < myp.relayVolt )
                    return true;
            }
            else                // true if GT
            {
                if( M_VOLTS2 > myp.relayVolt )
                    return true;  
            }
        }
        else                // check V1
        {
            if( myp.relayComp ) // true for LT
            {
                if( M_VOLTS1 < myp.relayVolt )
                    return true;
            }
            else                // true if GT
            {
                if( M_VOLTS1 > myp.relayVolt )
                    return true;  
            }
        }
    }
    return false;
}
// ------------------------ STREAMING UPDATES. ALSO CALLABLED BY CLI -----------------

void updateMeters( CLI_ARGS )
{
    DECLARE( bp );
    
    if( !STREAMCODE )
        return;
    
    mgn.init( bp, "METERS" );
    if( STREAMCODE & SCMASK_ADCV )
    {
        char s[16];
        sprintf( s, "%04X", getADCV() );
        mgn.controlSetText( "bVCount", s );  
    }
    if( STREAMCODE & SCMASK_ADCA )
    {
        char s[16];
        sprintf( s, "%04X", ADC_AMPS );
        mgn.controlSetText( "bACount", s );   
    }
    if( STREAMCODE & SCMASK_VOLTS )
    {
        // Move the Gauge Needle
        char s[16];
        if( !myp.selectV1V2 )        // display V1
        {
            SF( s, "gVolts%d", myp.mee.vscale );                      
            mgn.controlSetValue( (const char *)s, M_VOLTS1 ); 

            float v = M_VOLTS1;      // display Label
            if( v<10.0f )
                SF( s, "%.3f V", v );
            else
                SF( s, "%.2f V", v );
            mgn.controlSetText( "bVolts", s );
        }
        else                                    // gauge V2
        {
            SF( s, "gVolts%d", myp.mee.vscale );                      
            mgn.controlSetValue( (const char *)s, M_VOLTS2 ); 

            float v = M_VOLTS2;      // display Label
            if( v<10.0f )
                SF( s, "%.3f V", v );
            else
                SF( s, "%.2f V", v );
            mgn.controlSetText( "bVolts", s );
        }
    }
    if( STREAMCODE & SCMASK_AMPS )
    {
        // Move the Gauge Needle
        char s[16];
        SF( s, "gAmps%d", myp.mee.ascale );                         
        char temp[16];
        if( myp.mee.ascale == 1 ) // mA
            SF( temp, "%.0f", M_AMPS*1000.0 );
        else
            SF( temp, "%.2f", M_AMPS );
        mgn.controlSetValue( (const char *)s,  (float) atof(temp) ); 

        // Create the Amps Label 
        float a = M_AMPS;
        if( abs(a)<1.0f )
            SF( s, "%.0f mA", a*1000.0 );
        else
            SF( s, "%.2f A", a );
        mgn.controlSetText( "bAmps", s ); 
    }
    if( STREAMCODE & SCMASK_PWR )
    {
        char s[20];
        
        float a = myp.power;
        if( abs(a)<1.0f )
            SF( s, "%.0f mW", a*1000.0 );
        else
            SF( s, "%.2f W", a );
        mgn.controlSetText( "bPower", s ); 

        a = myp.energy;   // this is in Ws
        
        if( !myp.punits )           // in Watts-seconds
        {
            if( abs(a)<1.0f )
                SF( s, "%.0f mWs", a*1000.0 );
            else if( abs(a)<1000.0 )
                SF( s, "%.0f Ws", a );
            else
                SF( s, "%.2f Wh", a/3600 );
            
        }
        else                        // in Ampere-hours
        {
            a /= 3600.0*3.6;
            if( abs(a)<1.0f )
                SF( s, "%.0f mAh", a*1000.0 );
            else
                SF( s, "%.2f Ah", a );            
        }
        mgn.controlSetText( "bEnergy", s ); 

        int sec = (myp.pduration * myp.mee.meterperiod)/10;
        SF( s, "%d min, %d sec", sec/60, sec%60 );
        mgn.controlSetText( "dDuration", s ); 
    }
    // At this point, the mgn.getBuf() contains the string to be send.
}

void updateGraph( CLI_ARGS )
{
    DECLARE( bp );
    if( !(STREAMCODE & SCMASK_GRAPH) )
        return;

    mgn.init( bp, "METERS" );
    mgn.tplotData( "Volts:<", getVolts()  ); 
    mgn.tplotData( "Amps:>",  M_AMPS );   
}
//------------------------------- CLI HANDLERS ----------------------------------------

namespace CLIADC									// all CLI functions
{
    
// =============================== CONFIG TAB ===========================================
	
	void initConfig( CLI_ARGS )
    {
        DECLARE( bp );

        mgn.init(  bp, "CONFIG" );
        mgn.controlSetValue( "dMeterSa", myp.mee.meterperiod );
        mgn.controlSetValue( "dGraphSa", myp.mee.graphperiod );
        mgn.controlSetValue( "dFilterV", myp.mee.smoothvolts );
        mgn.controlSetValue( "dFilterA", myp.mee.smoothamps );
        mgn.controlSetValue( "chn",      5 );
        mgn.controlSetValue( "dFullScale", 0 );
    }    
    
    void selectChannel( CLI_ARGS )             // shows parameter and channel data
	{
	    DECLARE( bp );
	    if( n<=1 )
	        return;
	    int i = atoi( args[1] );
		
		mgn.init(  bp,"CONFIG");
	    mgn.tableClear();
        if( i <= 4 )                      // channel selection
        {
    	    mgn.tableSet( "EEPROM:"," " );
    	    mgn.tableSet( "Channel",    i,                      "ADC Channel");
    	    mgn.tableSet( "FS",         (int) myp.chn[i].fs,    "Full Scale (mV)" );
            if( i==2 || i==3 )
                mgn.tableSet( "Filter-V", myp.mee.smoothvolts*100,      "V-samples averaged (ms)");
            else
                mgn.tableSet( "Filter-A", myp.mee.smoothamps*100,      "A-samples averaged (ms)");  
               
            mgn.tableSet( "Scale",      myp.chn[i].scale,       "Scale to eng units" );
            mgn.tableSet( "Offset",     myp.chn[i].offset,      "Offset of eng units" );
            
            mgn.tableSet( "MEASUREMENTS:", " " );               // blank line

    	    char s[16];
    	    SF( s, "%04X", myp.chn[i].reading );
    	    mgn.tableSet( "ADC Count",        s,                "MEASUREMENTS" );
    	    mgn.tableSet( "ADC Volts",  myp.chn[i].volts,       "Volts" );
    	    
    	    if( i==2 )
    	        mgn.tableSet( "Voltage",	myp.chn[i].engunits,   myp.simulV1ON?"Volts (simulated)":"Volts (measured)" );
            if( i==3 )
                mgn.tableSet( "Voltage",    myp.chn[i].engunits,   myp.simulV2ON?"Volts (simulated)":"Volts (measured)" );
            if( i==4 )
                mgn.tableSet( "Current",   myp.chn[i].engunits,    myp.simulAON?"Amps (simulated)":"Amps (measured)" );

            mgn.controlSetValue("dFullScale", myp.chn[i].fs );
            SF( s, "Channel %d", i );
            mgn.controlSetText("calChannel", s );
        }
        else
        {
            mgn.tableSet( "Parameters", " " );             // blank line
            mgn.tableSet( "ScanInterval",  myp.mee.scanperiod*100,  "Timebase for Eng Units (ms)" );
            mgn.tableSet( "MeterUpdates",  myp.mee.meterperiod*100, "Meter Update Interval (ms)" );
            mgn.tableSet( "GraphUpdates", myp.mee.graphperiod*100,  "Graph Update Interval (ms)" );
            mgn.tableSet( "Filter-V",     myp.mee.smoothvolts,      "Number of V-samples to 99%");
            mgn.tableSet( "Filter-A",     myp.mee.smoothamps,       "Number of A-samples to 99%");  
            mgn.tableSet( "ACS-offset",   myp.mee.acsoffset,        "ACS Voltage offset (at 0.00A)" ); 
            if( myp.selectV1V2 )
                mgn.tableSet( "V2",           M_VOLTS2,  "Volts" ); 
            else
                mgn.tableSet( "V1",           M_VOLTS1,  "Volts" ); 
            mgn.tableSet( "Current",         M_AMPS,   "Amperes" ); 
            mgn.tableSet( "Power",           myp.power,             "Watts" ); 
            mgn.tableSet( "Energy",          myp.energy/3.6F/3.6F,  "mAh" ); 

            mgn.controlSetValue("dFullScale", (int) 0 );
            mgn.controlSetText("calChannel", "No Channel Selected" );
        }
	}
    // ---------------------------- EEPROM PARAMETERS -----------------------------------
    void fetchEEParms( CLI_ARGS )
    {
        DECLARE( bp );
        myp.fetchMyParms();
        RESPONSE("ok\r\n");
    }
    void saveEEParms( CLI_ARGS )
    {
        DECLARE( bp );
        myp.saveMyParms();
        RESPONSE("ok\r\n");
    }
    void initEEParms( CLI_ARGS )
    {
        DECLARE( bp );
        myp.initMyParms( false );           // initialize but not save
        RESPONSE("ok\r\n");
    }
    // ---------------------------- SAMPLING PARAMETERS -----------------------------------
    void setUpdates( CLI_ARGS )
    {
        DECLARE( bp );
        if( n<=2 )
            return;
         
        myp.mee.meterperiod = atoi( args[1] );
        myp.mee.graphperiod = atoi( args[2] );

        RESPONSE("\tok Meters:%dms Graph:%dms\r\n", myp.mee.meterperiod*100, myp.mee.graphperiod*100 );
    }
    void smooth( CLI_ARGS )
    {
        DECLARE( bp );
        if( n<=2 )
            return;
                    
        scan.reset( myp.mee.smoothvolts=atoi( args[1] ), myp.mee.smoothamps=atoi( args[2] ) );
    }
    void setFS( CLI_ARGS )
    {
        DECLARE( bp );
        int m;
        if( n<=2 )
            return;
        int i = atoi( args[1] );             // channel
    
        myp.chn[i].fs = (fullscale_t)atoi( args[2] );
        RESPONSE("ok\r\n");
    }
    
    // ----------------------------- CALIBRATION ----------------------------------
    void calDefault( CLI_ARGS )
    {
        DECLARE( bp );
        if( n<=1 )                          // no arguments: Use default calibration
            return;
        int chn = atoi( args[1] ) ;
        if( chn> 4 )
            chn = 4;
        myp.defaultScale( chn );
    }
    void calSaveP1( CLI_ARGS )
    {
        DECLARE( bp );
        if( n<=2 )
            return;
        int i = atoi( args[1] );         // channel number
        if( i>4 )
            i = 4;
        myp.adpoint1 = myp.chn[i].volts;
        myp.desired1 = atof( args[2] );   
    
        PF("\tPoint 1 is: %.3fV, fraction: %.3f)\r\n", myp.desired1, myp.adpoint1 );
    }
    void calibrate( CLI_ARGS )
    {
        DECLARE( bp );
        if( n<=2 )
            return;
        int i = atoi( args[1] );         // channel number
        if( i>4 )
            i = 4;                              // calibrate now
    
        myp.adpoint2 = myp.chn[i].volts;
        myp.desired2 = atof( args[2] );
        float dif = myp.adpoint2-myp.adpoint1;
    
        PF("\tPoint 2 is: %.3fV, fraction: %.3f, dif: %.3f\r\n", myp.desired2, myp.adpoint2, dif );        
        
        myp.chn[i].scale  = (myp.desired2-myp.desired1)/((float) dif);
        myp.chn[i].offset = (myp.desired2+myp.desired1) - myp.chn[i].scale*(myp.adpoint2+myp.adpoint1);
        myp.chn[i].offset /= 2.0;
      
        PF("\tScale=%.3f Offset=%.3f\r\n", myp.chn[i].scale, myp.chn[i].offset );
    }
    void nullACS( CLI_ARGS )
    {
        DECLARE( bp );
        myp.mee.acsoffset = myp.chn[4].volts;
    }
    
    // =============================== METER & GRAPH TAB ===========================================
    
    void resetSelected( scmask_t which )    // helper routine used below multiple times
    {
        if( which & SCMASK_ADCV )
        {
            STREAMCODE = BRES( SCMASK_ADCV );
        
            //mgn.controlSetValue( "dScaleV", 0 );
            mgn.controlSetCheck( "rVMeter", false );
            mgn.controlSetCheck( "rADCV", false );
            mgn.controlSetText( "bVCount",   "0000" );
        }
        if( which & SCMASK_VOLTS )
        {
            STREAMCODE = BRES( SCMASK_VOLTS );
        
            mgn.controlSetProperty( "gvolts1.Visible=True" );
            mgn.controlSetProperty( "gvolts2.Visible=False" );
            mgn.controlSetProperty( "gvolts3.Visible=False" );
            mgn.controlSetValue( "gvolts1", 0.0f ); 
        }
        if( which & SCMASK_ADCA )
        {
            STREAMCODE = BRES( SCMASK_ADCA );
        
            //mgn.controlSetValue( "dScaleA", 0 );
            mgn.controlSetCheck( "rAMeter", false );
            mgn.controlSetCheck( "rADCA", false );
            mgn.controlSetText( "bACount",   "0000" );
        }
        if( which &  SCMASK_AMPS )
        {
            STREAMCODE = BRES( SCMASK_AMPS );
            
            mgn.controlSetProperty( "gamps1.Visible=False" );
            mgn.controlSetProperty( "gamps2.Visible=False" );
            mgn.controlSetProperty( "gamps3.Visible=True" );
            mgn.controlSetValue( "gamps3", 0.0f ); 
        }
//        if( which & SCMASK_3VAL ) // not used
//        {
//            STREAMCODE = BRES( SCMASK_3VAL );
//
//            mgn.controlSetText( "bVolts",  "0.000V" );
//            mgn.controlSetText( "bAmps",   "0.00A" );
//            mgn.controlSetText( "bVolts1", "0.000V" );
//            mgn.controlSetCheck( "r3Meter", false );
//        }
        if( which & SCMASK_PWR ) 
        {
            STREAMCODE = BRES( SCMASK_PWR );

            mgn.controlSetText( "bPower",  "0.00W" );
            mgn.controlSetText( "bEnergy", myp.punits? "0.000Ah" : "0.00Ws" );
        }
        if( which & SCMASK_GRAPH )
        {
            // STREAMCODE = BRES( SCMASK_GRAPH );   // do not modify graph streaming state
            
            // mgn.controlSetCheck( "rChart", false );
            mgn.tplotClear();
            mgn.tplotSet("Title=Voltage and Current Measurements");
            mgn.tplotSet("X-Label=Time");
            mgn.tplotSet("Y-Label=Volts");
            mgn.tplotSet("Y2-Visible=1");
            mgn.tplotSet("Y2-Label=Amps");
            
            if( myp.mee.vscale == 3)                // for graph, use the voltmeter scale
                mgn.tplotYRange( 0, 30.0 );
            else if( myp.mee.vscale == 2)
                mgn.tplotYRange( 0, 10.0 );
            else
                mgn.tplotYRange( 0, 6.0 );

            if( myp.mee.ascale == 3)                // for graph, use the voltmeter scale
                mgn.tplotY2Range( -5.0, 5.0 );
            else if( myp.mee.ascale == 2)
                mgn.tplotY2Range( 0, 5.0 );
            else
                mgn.tplotY2Range( 0, 1.0 );
        }
    }
    void initMeters( CLI_ARGS )         // called when the INITIALIZE BUTTON is pressed
    {
        DECLARE( bp );
        mgn.init(  bp,"METERS" );
        resetSelected( SCMASK_VOLTS );
        resetSelected( SCMASK_ADCV  );
        resetSelected( SCMASK_AMPS  );
        resetSelected( SCMASK_ADCA  );
        //resetSelected( SCMASK_3VAL  );
        resetSelected( SCMASK_PWR );
        resetSelected( SCMASK_GRAPH );

        mgn.controlSetCheck( "cSimulateV", false );
        mgn.controlSetCheck( "cSimulateA", false );
        myp.simulV1ON       = false;
        myp.simulV2ON       = false;
        myp.simulAON        = false;
        
        mgn.controlSetText( "dLabel",  "V1" );
        myp.selectV1V2      = false;

        mgn.controlSetCheck( "relayEnable", false );
        myp.relayON         = false;

        STREAMCODE = SCMASK_NONE;
    }
    void tplotInit( CLI_ARGS )
    {
        DECLARE( bp );
        mgn.init(  bp, "METERS" );

        resetSelected( SCMASK_GRAPH );
    }   
    /* ------------------------------ STREAM SELECTION --------------------------------

     Defines the type of streaming as checkboxes from UI are pressed.
     Supports:
 
      S1/R1   enable/disable voltmeter gauge and display
      S2/R2   enable/disable voltmeter ADC 
      S4/R4   enable/disable amp-meter gauge and display
      S8/R8   enable/disable amp-meter ADC 
     
      S16/R16 enable/disable plot
    */
    void stream( CLI_ARGS )
    {
        DECLARE( bp );
        int mcode;
        bool setres;
        
        if( n<=1 )
        {
            STREAMCODE = SCMASK_NONE;
        }
        else
        {
            if( *args[1] == 'S' )
                setres = true;
            else
                setres = false;
                
            scmask_t code = (scmask_t) atoi( args[1]+1 );            // get the code
            if( setres )                    // true = enable 
            {            
                STREAMCODE = BSET( code );
                RESPONSE("\tEnabling %d\r\n", code&0xFF );    
            }
            else
            {
                STREAMCODE = BRES( code );
                RESPONSE("\tDisabling %d\r\n", code );
            }
        }
    }
    // ============================================ GAUGES CONTROL ================================================

    void selectV1V2( CLI_ARGS )
    {
        DECLARE( bp );

        mgn.init(  bp,"METERS");
        if( myp.selectV1V2 )    // if V2
        {
            myp.selectV1V2 = false;
            mgn.controlSetText( "dLabel",  "V1" );
        }
        else
        {
            myp.selectV1V2 = true;
            mgn.controlSetText( "dLabel",  "V2" );
        }
    }
    void selVMeter( CLI_ARGS )
    {
        DECLARE( bp );
        if( n<=1 )
            return;
        
        myp.mee.vscale = atoi( args[1] );       // 0=none, 1=0-6V, 2=0-10V, 3=0-30V

        mgn.init(  bp,"METERS");
        
        switch( myp.mee.vscale )
        {
            default:
                    resetSelected( SCMASK_VOLTS );
            		break;
            case 1:
                    //STREAMCODE = BSET( SCMASK_VOLTS );
                    mgn.controlSetProperty( "gvolts1.Visible=True" );
                    mgn.controlSetProperty( "gvolts2.Visible=False" );
                    mgn.controlSetProperty( "gvolts3.Visible=False" );
                    break;
            case 2:
                    //STREAMCODE = BSET( SCMASK_VOLTS );
                    mgn.controlSetProperty( "gvolts1.Visible=False" );
                    mgn.controlSetProperty( "gvolts2.Visible=True" );
                    mgn.controlSetProperty( "gvolts3.Visible=False" );
                    break;
            case 3:
                    //STREAMCODE = BSET( SCMASK_VOLTS );
                    mgn.controlSetProperty( "gvolts1.Visible=False" );
                    mgn.controlSetProperty( "gvolts2.Visible=False" );
                    mgn.controlSetProperty( "gvolts3.Visible=True" );
                    break;
        }
        mgn.controlSetCheck( "rVMeter", true );
        STREAMCODE = BSET( SCMASK_VOLTS );
        PF( "Streamcode %d\r\n", STREAMCODE );
        
    }
    void selAMeter( CLI_ARGS )
    {
        DECLARE( bp );
        if( n<=1 )
            return;
        myp.mee.ascale = atoi( args[1] );   // 0=none, 1=0-1000mA, 2=5A, 3=+/-5A
        mgn.init(  bp,"METERS");
        
        switch( myp.mee.ascale )
        {
            default:
                    resetSelected( SCMASK_AMPS );
                    break;
            case 1:
                    //STREAMCODE = BSET( SCMASK_AMPS );
                    mgn.controlSetProperty( "gamps1.Visible=True" );
                    mgn.controlSetProperty( "gamps2.Visible=False" );
                    mgn.controlSetProperty( "gamps3.Visible=False" );
                    break;
            case 2:
                    //STREAMCODE = BSET( SCMASK_AMPS );
                    mgn.controlSetProperty( "gamps1.Visible=False" );
                    mgn.controlSetProperty( "gamps2.Visible=True" );
                    mgn.controlSetProperty( "gamps3.Visible=False" );
                    break;
            case 3:
                    //STREAMCODE = BSET( SCMASK_AMPS );
                    mgn.controlSetProperty( "gamps1.Visible=False" );
                    mgn.controlSetProperty( "gamps2.Visible=False" );
                    mgn.controlSetProperty( "gamps3.Visible=True" );
                    break;
        }
        mgn.controlSetCheck( "rAMeter", true );
        STREAMCODE = BSET( SCMASK_AMPS );
        PF( "Streamcode %d\r\n", STREAMCODE );
    }

    
    void oledONOFF( CLI_ARGS )
    {
        DECLARE( bp );
        if( n<=1 )
            return errMissingArgs();
        
        int value = atoi( args[1] );
        
        if( value )
            myp.oledON = true;
        else
        {
            myp.oledON = false;
            initOLED();
        }
    }
    
    // ============================================ SIMULATION CONTROL ==========================================    
    void simulVA( CLI_ARGS )
    {
        DECLARE( bp );
        if( n<=3 )
            return errMissingArgs();
        
        bool flag = atoi( args[2] );    // if flag=0, stop simulation
        int value = atoi( args[3] );
        
        mgn.init(  bp, "METERS" );
        if( *args[1] == 'V' )           // simulate voltage depending on the 
        {
            if( flag )
            {
                if( !myp.selectV1V2 )   // V1 is requested to be simulated
                {                
                    myp.simulV1ON  = true;
                    myp.simulV1    = (float)value;
                    myp.simulV1adc = ads.toADC( 2, (myp.simulV1 - myp.chn[2].offset)/myp.chn[2].scale );
                    
                    //qvolts1.reset();    // reset the IIR filter
                    PF("Simulated Volts1=%.3fV\r\n", myp.simulV1 );
                }
                else
                {                
                    myp.simulV2ON  = true;
                    myp.simulV2    = (float)value;
                    myp.simulV2adc = ads.toADC( 3, (myp.simulV2 - myp.chn[3].offset)/myp.chn[3].scale );

                    //qvolts2.reset();    // reset the IIR filter
                    PF("Simulated Volts2=%.3fV\r\n", myp.simulV2 );
                }
                mgn.controlSetCheck( "cSimulateV", true );
                mgn.controlSetCheck( "rVMeter", true );
            }
            else
            {
                if( !myp.selectV1V2 )
                {
                    myp.simulV1ON = false;
                    qvolts1.reset();
                }
                else
                {
                    myp.simulV2ON = false;
                    qvolts2.reset();
                }
            }
        }
        if( *args[1] == 'A' )
        {
            if( flag )
            {
                myp.simulA    = (float)value;
                myp.simulAadc = ads.toADC( 4, (myp.simulA - myp.chn[4].offset)/myp.chn[4].scale );
                //qamps.reset();
                
                myp.simulAON = true;
                PF("Simulated Amps=%.3fV\r\n", myp.simulA );
                mgn.controlSetCheck( "cSimulateA", true );
                mgn.controlSetCheck( "rAMeter", true );
            }
            else
            {
                myp.simulAON = false;
                qamps.reset();
            }
        }
    }
    // ============================================ RELAY CONTROL ==========================================
    void enableRelay( CLI_ARGS )
    {
        DECLARE( bp );
        if( n<=1 )
            return errMissingArgs();

        mgn.init(  bp, "METERS" );
        if( atoi( args[1] )==0 )        // if disable
        {
            myp.relayON   = false;
            mgn.controlSetCheck( "relayEnable", false );
        }
        else                            // if enable
        {
            if( n<=4 )
                return errMissingArgs();
            myp.relayON   = true;
            myp.relaySel  = atoi( args[2] );
            myp.relayComp = atoi( args[3] );
            myp.relayVolt = atof( args[4] );
        }
    }
    // ============================================ POWER & CAPACITY ==========================================
    void setEnergy( CLI_ARGS )
    {
        DECLARE( bp );
        if( n<=1 )
            return errMissingArgs();

        if( atoi( args[1] )==0 )        // if 0, reset energy
        {
            myp.energy = 0.0;
            myp.pduration = 0;            
        }
        else                            // if 1, toggle units
        {
            myp.punits = !myp.punits;
        }
    }
    // ============================================ BATTERY RIN TEST ==========================================
    void startBatTest( CLI_ARGS )
    {
        DECLARE( bp );
        if( n<=6 )
            return errMissingArgs();
        
        int i1 = atoi( args[1] );
        int i2 = atoi( args[2] );
        int i3 = atoi( args[3] );
        int i4 = atoi( args[4] );
        int i5 = atoi( args[5] );
        int i6 = atoi( args[6] );
        
        mgn.init(  bp, "BATRIN" );
        rin.startCycle( i1, i2, i3, i4, i5, i6 ); 
    }
    void help( CLI_ARGS )
    {
        DECLARE( bp );
        exe.help( n, args );
    }
    // ============================================ COMMAND TABLES ==========================================    
	CMDTABLE Table[]
	{
	    {"h",    			"\t[select] help",                                  help },

        // ----------------- CONFIG PANEL ---------------------------
        {"!initConfig",   "Set Panel to default state",                            initConfig },
	    {"!getChannel",   "chn. Display channel properties and values",       selectChannel },
	    {"!setFS",          "chn FS=(512,,,,4096,6144). Set full scale",   		setFS },

        {"!fetchEEParms",   "Fetch parameters",                                   fetchEEParms },
        {"!saveEEParms",    "Save parameters",                                    saveEEParms },
        {"!initEEParms",    "Initialize parameters to defaults",                  initEEParms },

        //{"!calUpdateV",    "Update Calibration V readings",                     stream },
        {"!calSaveP1",     "chn v1 Sets desired point 1",                       calSaveP1  },
        {"!calibrate",     "chn v2 Calibrates using v1 and v2",                 calibrate  },
        {"!calDefault",    "chn. Sets default calibration values",              calDefault },
        {"!nullACS",       "Zero current offset for ACS sensor",                nullACS },

        {"!setUpdates",    "meters100ms graph100ms. Updates of Meters/Graph",   setUpdates },

        {"!oled",          "0|1 Enable/disable OLED",                               oledONOFF },
        {"!toggleV1V2",    "Select V1 or V2",                                       selectV1V2 },
        
        {"!simulVA",       "V|A 1|0 value. Turn simulation ON/OFF",                 simulVA },

        // ----------------- METER PANEL ---------------------------    
	    {"!stream",        "S|R 0=off 1=adc 2=volts etc. Enable/disable streaming",     stream },
	    {"!initMeters",   "Initialize and zero metter UI",                         initMeters },

	    {"!plotInit",     "Initialize plot",      								    tplotInit  },

        {"!selVMeter",    "0:0-6V 1:0-10V 2:0-30V. Select V-meter scale",           selVMeter },
        {"!selAMeter",    "0:0-1000mA 1:0-5A 2:-/+5A. Select A-meter scale",        selAMeter },
        
	    {"!smooth",       "Vms Ams. Smooth V or A by so many ms",                   smooth },

        {"!initMeters",   "Initialize and zero metter UI",                         initMeters },
        
        {"!relay",        "1=on|0=off 0=V1|1=V2 0=GT|1=LT volts. Activate relay",  enableRelay },
        {"!setEnergy",    "0=reset 1=toggle Ws/Ah. Set energy counter",            setEnergy },
        
        {"!BatTest",      "args. Start battery Testing",                           startBatTest },
     
	  {NULL, NULL, NULL}
	};
}
