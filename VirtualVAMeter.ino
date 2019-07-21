/* 
 * ---------------------------------------------------------------------------------
 *          Main setup() and loop() for VIRTUAL VA METER
 *  
 *  Files needed:
 *      VirtualVAMeter.cpp  // main setup and loop
 *      cliHandlers.cpp     // CLI command handlers
 *      mscSupport.cpp      // misc utility functions and classes
 *      globals.cpp         // global variables and structures used by all above
 *
 *  Hierarchy:
 *      VirtualVAMeter  --> cliHandlers --> mscSupport --> globals
 *
 *  Summary of Class definitions and Allocations
 *  
 *      BUF buf;            // defined in bufClass.h,   allocated in VirtualVAMeters.cpp *      
 *      PROF prf;           // defined in ticClass.h,   allocated in VirtualVAMeters.cpp
 *      Ticker tic;         // defined in esp8266,      allocated in VirtualVAMeters.cpp
 *      TCP tcp;            // defined in tcpClass,     allocated in cliHandlers.cpp
 *      MGN mgn;            // defined in mgnClass,     allocated in cliHandlers.cpp
 *      OLED oled;          // defined in oledClass.h,  allocated in mscSupport.cpp
 *      ADS15 ads;          // defined in ads15Class.h, allocated in mscSupport.cpp
 *      DIIR qvolts12,qamps // defined in filtClass.h,  allocated in mscSupport.cpp
 *      SCAN scan;          // defined and allocated in mscSupport.cpp/.h
 *      CPU cpu;            // defined in cpuClass.h,   allocated in globals.cpp
 *      CLI cli;            // defined in cliClass.h,   allocated in globals.cpp
 *      EXE exe;            // defined in cpuClass.h,   allocated in globals.cpp
 *      EEP eep;            // defined in eepClass.h,   allocated in globals.cpp
 *      GLOBALS myp;        // defined in global.h      allocated in globals.cpp
 *      
 * Copyright (c) George Kontopidis 2019, All Rights Reserved
 * ----------------------------------------------------------------------------------
 */
    #include <ticClass.h>       // from GKE Lib1
    #include <Ticker.h>         // from Arduino library
    
    #include "mscSupport.h"     // in this local directory
    #include "cliHandlers.h"    // in this local directory
    #include "Globals.h"        // includes also exports of <externIO.h> for cpu...eep

// --------------- allocation of classes used only by this module -------------------
  
    static BUF buf(1500);              // defined in bufClass.h
    static PROF prf(20,30,50,75,90);   // defined in ticClass.h
    static Ticker tic;                 // defined in esp8266 library
    
// ----------------------------- main setup() ----------------------------------------------

static bool tic_ready = false;  // Ticker flips this to 'true' every 20ms
                                // A full scan is completed in 100ms
void setup(void) 
{
    cpu.init();
    pinMode( 14, OUTPUT );
   
    initOLED();                                             // Initialze SSD1306 OLED dsp. In mscSupport.cpp
    myp.initAllParms( /*Magic Code*/0x4467 );               // Initialize global parameters. Fetch from EEPROM as appropriate

    ads.init( 0x48 );                                       // Initialize ADC. ADDR pin to GROUND
    for( int i=0; i<5; i++ )                                // Initialize ADS channels
        ads.initChannel( i, myp.chn[i].fs );
  	    
    scan.init(  myp.mee.scanperiod );                        // Initialize scanner. scan.ready() every scanperiod*100ms increment
    scan.reset( myp.mee.smoothvolts, myp.mee.smoothamps );   // Depth of amps queues
    
    exe.registerTable( CLIADC::Table );
    exe.printTables( "See all tables" );  // instead of PF, you can optionally specify a Buf
    
    cli.init( ECHO_OFF, "cmd-buf:" );
    cli.prompt();
    
    #ifdef TCP_ACTIVE
    initWiFi();
    //tcp.init( 23, ECHO_ON, "cmd-tcp:" );
    tcp.init( 23, ECHO_OFF );
    #endif
    
    STREAMCODE = SCMASK_NONE;
    
    tic_ready = false;
    tic.attach_ms( 20, [](){ tic_ready=true; } );
}

// ----------------------------- main loop() ----------------------------------------------
/*
 * Every 20ms, Ticker makes tic_ready becomes TRUE
 * When tic_ready, we scan all channels and measure voltages; this is done by calling the 
 * SCAN.update() in the main loop.
 * Measurements are averaged by the SCAN.update() and every 100ms (or so), the SCAN.ready()
 * triggers computation of the engineering values.
 */
void loop()
{
	if( cli.ready() )                               // check to see if a CLI command was entered
	{            
	    char *cmd = cli.gets();                     // cmd points to the heap allocated by CLI
	    CRLF();
	    exe.dispatchBuf( cmd, buf );                // using Serial.printf(); unbuffered version
	    PR(!buf);                                   // display the buffer
        
        if( buf.length() > buf.size() - 40 )     // running low in buffer
    	    PF("*** CLI BUFFER[%d] OVERFLOW\r\n", buf.size() );
	    cli.prompt();
	}
    #ifdef TCP_ACTIVE
	if( tcp.ready() )                               // check to see if a TCP command was received
	{            
	    char *cmd = tcp.gets();                     // cmd points to the heap allocated by CLI
	    exe.dispatchBuf( cmd, buf );                // using Serial.printf(); unbuffered version
	    tcp.respond( !buf );                        // display the buffer
	    tcp.prompt();
	}
    #endif
    if( tic_ready )                                 // come here every 20ms
	{
        scan.update();                              // spin the state machine
        tic_ready = false;  
	}
	if( scan.ready() )                              // come here every scanperiod = 100ms, 200ms, etc
    {
        prf.start();                                // start the profiler
        
        for( int i=0; i<5; i++ )                    // copy the filtered measurements
        {
            myp.chn[i].reading = scan.reading[i];
            myp.chn[i].volts = ads.toVolts( i, myp.chn[i].reading );               // convert to volts
        }
        M_ACSVAL = TOENG(0);
        M_ACSREF = TOENG(1);
        M_VOLTS1 = TOENG(2);           
        M_VOLTS2 = TOENG(3);
        
        float x   = ads.toVolts( 4, ADC_AMPS );           // convert to ADC volts
        float ref = M_ACSREF;                            // calibrate 5V 
        if( (ref < 3.0) && (ref >2.0) )                             // make sure correct reference
            x *= 2.5/ref;            
        myp.chn[4].volts = x;
        
        M_AMPS = myp.chn[4].scale * (myp.chn[4].volts - myp.mee.acsoffset) + myp.chn[4].offset;  

        digitalWrite( 14, setRelay() ); // check and activate
    
        if( scan.readyMod( myp.mee.meterperiod/myp.mee.scanperiod ) )   // update the METERS
        {                                                    
            static int mcount = 0;
            char *arg[1]; 
            arg[0] = (char *)(&buf); buf.init();                    // simulate CLI call arguments

            updateMeters( 1, arg );                                 // at return, 'buf' has been filled

            myp.power = M_AMPS * (myp.selectV1V2 ? M_VOLTS2 : M_VOLTS1 );
            myp.energy += myp.power*((float)myp.mee.meterperiod)/10.0;
            myp.pduration++;

            if( myp.oledON && (mcount==0) )                     // every second update the OLED, i.e. 11, 21, 31...
                updateOLED(1);
            if( myp.oledON && (mcount==1) )                     // every second, 12, 22, 32...
                updateOLED(2);
            if( myp.oledON && (mcount==2) )                     // every second, 13, 23, 33...
                updateOLED(3);
            mcount++;
            if( mcount>=3) 
                mcount=0;
            
            #ifdef TCP_ACTIVE
            if( tcp.ready() )
                tcp.respond( !buf );
            else
            #endif
                PR( !buf );
        }
        if( scan.readyMod( myp.mee.graphperiod/myp.mee.scanperiod ) )   // update the GRAPH
        {
            char *arg[1]; 
            arg[0] = (char *)(&buf); buf.init();                    // simulate CLI call arguments

            updateGraph( 1, arg );                                  // at return, 'buf' has been filled
            #ifdef TCP_ACTIVE
            if( tcp.ready() )
                tcp.respond( !buf );
            else
            #endif
                PR( !buf );
        }
        prf.profile();
        if( scan.readyMod( 100 ) )                                  // print profile every 10 sec
        {
            prf.report( true );
            prf.reset();
        }
    }
/*                
        if( tic_mod( 10, 0 ) )          // every 1sec update the Rin counters
        {
            if( rin.ready() )
                rin.update();
        }
*/
}
