/* ---------------------------------------------------------------------------------
 *          MISC SUPPORT FUNCTIONS & CLASSES
 *  
 * See VirtualVAMeter.cpp for file and class hierarchy
 * Copyright (c) George Kontopidis 2019, All Rights Reserved
 * ----------------------------------------------------------------------------------
 */
#include <oledClass.h>
#include <ads15Class.h>
//#include <filtClass.h>            // from GKE Lib1
#include "filtTemp.h"               // localy here
    
#include "Globals.h"                // includes also exports of <externIO.h> for cpu...eep
#include "mscSupport.h"             // exported functions/tables of this module

// ---------------------- allocation of classes -------------------------------------
    
    OLED oled;                      // defined in oledClass.h, allocated here
    ADS15 ads;                      // defined in ads15Class.h, allocated here
    DIIR qvolts1, qvolts2, qamps;   // decimal IIR filder
    SCAN scan;
    
// --------------------------------- OLED SUPPORT --------------------------------
void updateOLED( int func )
{
    switch( func )
    {
        case 0: oled.init( OLED130 );
                break;
        case 1:
                oled.dsp( 0, "\b%s", niceVolts(myp.chn[2].engunits) );
                break;
        case 2:
                oled.dsp( 3, "\b%s", niceAmps (myp.chn[4].engunits) );
                break;
        case 3:
                oled.dsp( 6, "\aM:%dms G:%ds",  (myp.mee.meterperiod/myp.mee.scanperiod)*100, 
                                                (myp.mee.graphperiod/myp.mee.scanperiod)/10 );
                break;
        default:
                break;
    }
}
void initOLED()
{
    oled.init( OLED130 );
}

// --------------------------------- WIFI SUPPORT --------------------------------
const char* _ssid = "kontopidis2GHz";
const char* _password = "123456789a";

void initWiFi()
{
    PF("Connecting to %s\r\n", _ssid);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid, _password);

    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }
    PR ("\r\nWiFi connected. IP=");
    PRN(WiFi.localIP());
}

// --------------------------------- MATH SUPPORT --------------------------------
char *niceVolts( float x )
{
    static char s[32];
    
    if( x >= 100.0 )
        sprintf( s, ">99.99V");
    else if( x>=10.0 )
        sprintf( s, "%.2fV", x );
    else if( x>=1.0 ) 
        sprintf( s, "%.3fV", x );
    else
        sprintf( s, "%.0fmV", x*1000.0 );
    return s;
}
char *niceAmps( float x )
{
    static char s[32];
    
    if( x >= 10.0 )
        sprintf( s, ">9.99A");
    else if( x>=1.0 )
        sprintf( s, "%.2fA", x );
    else
        sprintf( s, "%.0fmA", x*1000.0 );
    return s;
}
float round0( float x )
{
    bool pos = false;
    if( x>= 0.0 )
        pos = true;
    else
        x = -x;
    if( x < 1000.0 )
    {
        int ix = (int) x;
        x = (float) ix;
    }
    return pos?x:-x;
}

// --------------------- aquisition state machine ------------------------------------
/* USAGE:
 *      SCAN scan;
 *      
 *      setup()
 *          scan.init( V_q_depth, INDX_q_depth, N_tics_for_ready )
 *      
 *      loop()
 *          scan.update()
 *          OR
 *          if( timer )
 *              scan.update()
 *          
 *          if( scan.ready() )
 *              x[i] = scan.reading[i];        
 * DEPENDS:         
 *       ADS15 ads;
 *       CBIQ  qvolts1, qvolts2, qamps
 */


/*
    uint counter;               // increments every call
    uint mreading;              // how often to update readings
    uint mreadcount;            // counts 0...mreading-1
    bool iamready;              // true if mreading has been reached.
    uint readycounter;          // increments every time iamready is true
    bool scancomplete;          // true if all channels have started conversion

 */
    void SCAN::init( int nticready )
    {        
        qvolts1.init( 1 );
        qvolts2.init( 1 );
        qamps.init( 1 );
            
        counter  = 0;
        readycounter = 0;
        mreadcount = 0;
        mreading = nticready;
        iamready = false;
        scancomplete = false;
    }
    void SCAN::reset( int vdepth, int idepth )
    {
        if( vdepth==0 )         // keep the previous value
        {
            qvolts1.reset();
            qvolts2.reset();
        }
        else                    // use new one
        {
            qvolts1.init( vdepth );
            qvolts2.init( vdepth );    
        }
        if( idepth == 0 )
            qamps.reset();
        else
            qamps.init( idepth );
            
        mreadcount = 0;
        iamready = false;
        readycounter = 0;
        scancomplete = false;
    }
    bool SCAN::ready()
    {
        bool retcode = iamready;
        iamready = false;
        return retcode;
    }
    void SCAN::update()   // worst case computation delay is 12ms
    {        
         uint32_t T0 = millis();
         iamready = false;
         int v;
         
        switch( counter % 5 )
        {
            case 0:
                    if( scancomplete )
                    {
                        // Get the last channel (Current)
                        // But is in simulation mode, use the derived ADC value
                                              
                        v = myp.simulAON ? myp.simulAadc : ads.readConversion();
                        
                        qamps.feedInput( v );
                        mreadcount++;
                        if( mreadcount >= mreading )
                        {
                            mreadcount = 0;
                            reading[ INDX_VOLTS1 ] = qvolts1.getOutput();
                            reading[ INDX_VOLTS2 ] = qvolts2.getOutput();
                            reading[ INDX_AMPS   ] = qamps.getOutput();
                            iamready = true;
                            readycounter++;
                        }
                    }
                    ads.startConversion( INDX_ACSVAL );
                    scancomplete = false;
                    break;
            case 1:
                    reading[ INDX_ACSVAL ] = ads.readConversion();  
                    ads.startConversion( INDX_ACSREF );
                    scancomplete = false;
                    break;
            case 2:
                    reading[ INDX_ACSREF ] = ads.readConversion(); 
                    ads.startConversion( INDX_VOLTS1 );
                    scancomplete = false;
                    break;
            case 3:
                    v = myp.simulV1ON ? myp.simulV1adc : ads.readConversion();
                    qvolts1.feedInput( v );
                    
                    ads.startConversion( INDX_VOLTS2 );
                    scancomplete = false;
                    break;
            case 4:
                    v = myp.simulV2ON ? myp.simulV2adc : ads.readConversion();
                    qvolts2.feedInput( v );   
                    scancomplete = true;
                    break;
        }
        counter++;
        if( millis() > (T0+13) )
            PF("*** SCAN ABOVE 13ms ***\r\n");
        yield();
    }
    // if mod=1 triggers every iamready
    // if mod=2 triggers every 2nd time iamready
    bool SCAN::readyMod( int mod )
    {
        if( (readycounter % mod) == 1 )
            return true;
        else 
            return false;
    }
