#pragma once

/* ---------------------------------------------------------------------------------
 *          GLOBAL Classes and Structures
 *  
 * See VirtualVAMeter.cpp for file and class hierarchy
 * Copyright (c) George Kontopidis 2019, All Rights Reserved
 * ----------------------------------------------------------------------------------
 */
    #include <gkeL1io.h>              
    #include "ads15Class.h"         // needed only for the SCALE

// ----------------------- MAPPING OF ADC CHANNELS ----------------------------------

    #define INDX_ACSVAL     0
    #define INDX_ACSREF     1
    #define INDX_VOLTS1     2
    #define INDX_VOLTS2     3
    #define INDX_AMPS       4

    #define M_ACSVAL myp.chn[0].engunits
    #define M_ACSREF myp.chn[1].engunits
    #define M_VOLTS1 myp.chn[2].engunits
    #define M_VOLTS2 myp.chn[3].engunits
    #define M_AMPS   myp.chn[4].engunits

    #define ADC_ACSVAL myp.chn[0].reading
    #define ADC_ACSREF myp.chn[1].reading
    #define ADC_VOLTS1 myp.chn[2].reading 
    #define ADC_VOLTS2 myp.chn[3].reading
    #define ADC_AMPS   myp.chn[4].reading

// ----------------------------- STREAMING MASKS -------------------------------------
typedef enum 
{
    SCMASK_NONE  = 0,
    SCMASK_VOLTS = 1,
    SCMASK_ADCV  = 2,
    SCMASK_AMPS  = 4,
    SCMASK_ADCA  = 8,
    SCMASK_GRAPH = 16,
    SCMASK_3VAL  = 32,
    SCMASK_PWR   = 64
} scmask_t;
#define STREAMCODE myp.streamcode

// ----------------------------- ALL GLOBAL VARIABLES --------------------------------

class GLOBALS
{
private:
    EEP *eep;                   // pointer to the EEP class

public:
    scmask_t streamcode;        // 0=off, 1=cal adc, 2=cal volts

    struct mee_t                // EEPROM PARAMETERS
    {                           
        int scanperiod;         // how often to compute ENG units (in 100ms increments)
        
        int smoothvolts;        // depth of the voltage filter in 100ms increments; e.g. 10 means average 10 samples
        int smoothamps;         // depth of the amps filter in 100ms increments; e.g. 20 means average 20 samples
                
        int meterperiod;        // how often to update the meters (in 100ms increments).
        int graphperiod;        // how often to update the meters (in 100ms increments)

        float acsoffset;         // difference between ACS and ref channel

        int vscale;              // Meter Scale Selection. Does not need to be in EEPROM!
        int ascale;
        
    } mee;    
    #define MEE_SIZE sizeof( mee_t )
    
    struct chn_t
    {
        fullscale_t fs;     // full scale/gain
        int reading;        // raw A/D reading
        float volts;        // converted volts (depends on fscale)

        float scale;        // scale to convert to eng units
        float offset;       // offset to convert to eng units
        float engunits;     // engineering units

    } chn[5];				// 0-3 correspond to chn0...chn3; 4 is the differential chn01
	#define CHN_SIZE 5*sizeof( chn_t )
    
    bool relayON;           // RELAY control
    bool relaySel;
    bool relayComp;
    float relayVolt;
            
    float adpoint1;         // CALIBRATION
    float desired1;
    float adpoint2;         // fraction of full scale
    float desired2;

    bool  simulV1ON;         // SIMULATION CONTROL
    float simulV1;
    int   simulV1adc;       // raw ADC    
    bool  simulV2ON;        // for V2
    float simulV2;
    int   simulV2adc;       
    bool  simulAON;         // for A
    int   simulAadc;        
    float simulA;

    bool  powerON;          // POWER MEASUREMENTS
    float power;            // instanetenous power
    float energy;           // energy used in said duration
    int   pduration;        // duration for which energy is computed
    bool  punits;           // 0=Ws, 1=Ah (3.6V)
    
    bool oledON;            // OLED CONTROL
    
    bool selectV1V2;        // 0=select V1, 1=select V2
    
    byte *bpntr;            // pointer to free space
    int bsize;              // set by constructor

    void initMyParms( bool saveflag )                   // (2a) initialize parameters in memory
    {
        for( int i=0; i<5; i++ )
            defaultScale( i );

        streamcode = SCMASK_NONE;
        
        mee.scanperiod =1;      // every 100ms compute smoothed voltage and amps
        mee.smoothvolts=1;      // no voltage smoothing
        mee.smoothamps =1;      // no amps smoothing
        
        mee.meterperiod=5;     // every 500ms (1sec) update the meters
        mee.graphperiod=10;    // every 1000ms (1sec) update the graph
        
        mee.vscale = 1;         // 0-10V
        mee.ascale = 1;         // 0-5A
        
        mee.acsoffset = 0.006;
        
        simulV1ON = simulV2ON = simulAON = false;
        simulV1   = simulV2   = simulA   = 0.0;
        simulV1adc= simulV2adc= simulAadc = 0;
        
        oledON = true;
        selectV1V2 = false;

        relayON = false;
        relaySel = false;
        relayComp = false;
        relayVolt = 0.0;

        powerON = true;          // POWER MEASUREMENTS
        power = 0.0; 
        energy = 0.0;
        pduration = 0;
        punits = true;
    
        if (saveflag) 
            saveMyParms();                                  // and save the new values     
    }
    GLOBALS( EEP *ep )    
    {
        eep = ep;                                       // save pointer to the EPP class
        initMyParms( false );                           // initialize parameters in memory
        
        bsize = MEE_SIZE + CHN_SIZE;                    // size of all parameters
        bpntr = new byte [bsize];                       // allocate this in the heap
    }
    ~GLOBALS()
    {
        delete [] bpntr;
    }    
    void fetchMyParms()                                 // (2b) OR, fetch them from EEPROM
    {
         PF("Fetching %d mee_t and %d chn_t parms\r\n", sizeof( mee_t ), sizeof( chn_t ) );
         eep->fetchUserStruct( bpntr, bsize );   // fetch them
         memcpy( &mee, bpntr, MEE_SIZE );
         memcpy( &chn, bpntr+MEE_SIZE, CHN_SIZE );
    }        
    void saveMyParms()                                  // (3) use after modifying myp.parameters
    {
        memcpy( bpntr, &mee, MEE_SIZE );               // copy mee_t
        memcpy( bpntr+ MEE_SIZE, &chn, CHN_SIZE );
        PF("Saving %d mee_t and %d chn_t parms\r\n", MEE_SIZE, CHN_SIZE );
        
        eep->saveUserStruct( bpntr, bsize );
    }
    void defaultScale( int i )
    {
        // use switch to define defaults
        switch( i )
        {
            default:
            case INDX_ACSVAL:                                 // 0 to 4.096V; 400mV/A; 5A gives dV=2V, i.e. 0.5V...4.5V
                    chn[i].fs       = FS_4096mV; 
                    chn[i].scale    = 1.0;
                    chn[i].offset   = 0.0;
                    break;
            case INDX_ACSREF:                                 // 0 to 4.096V
                    chn[i].fs       = FS_4096mV; 
                    chn[i].scale    = 1.0;
                    chn[i].offset   = 0.0;
                    break;
            case INDX_VOLTS1:
                    chn[i].fs       = FS_2048mV;    // 0 to 10.0V
                    chn[i].scale    = 4.874;
                    chn[i].offset   = -0.003;
                    break;
            case INDX_VOLTS2:
                    chn[i].fs       = FS_6144mV;    // 0 to 24.0V
                    chn[i].scale    = 4.910;
                    chn[i].offset   = -0.006;
                    break;  
            case INDX_AMPS:
                    chn[i].fs       = FS_2048mV;    // 0 to 2.048V
                    chn[i].scale    = 5.0/2.0;		// 2V correspond to 5A (400mV/A)
                    chn[i].offset   = 0.0;
                    break;            
        }
        chn[i].reading  = 0; 
        chn[i].volts    = 0.0;
        chn[i].engunits = 0.0;
    }                  
};
