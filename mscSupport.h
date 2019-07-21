#pragma once
/* ---------------------------------------------------------------------------------
 *          MISC SUPPORT FUNCTIONS & CLASSES
 *  
 * See VirtualVAMeter.cpp for file and class hierarchy
 * Copyright (c) George Kontopidis 2019, All Rights Reserved
 * ----------------------------------------------------------------------------------
 */
    #include <oledClass.h>
    #include <ads15Class.h>
    #include "filtTemp.h" 

// -------------------------------- SAMPLING ----------------------------------------
class SCAN
{
    uint counter;               // increments every call
    uint mreading;              // how often to update readings
    uint mreadcount;            // counts 0...mreading-1
    bool iamready;              // true if mreading has been reached.
    uint readycounter;          // increments every time iamready is true
    bool scancomplete;          // true if all channels have started conversion
    
public:
    int reading[5];
    void init( int nticready );
    void reset( int vdepth, int idepth );
    bool ready();
    void update();   // worst case computation delay is 12ms

    // if mod=1 triggers every iamready
    // if mod=2 triggers every 2nd time iamready
    bool readyMod( int mod );
};

// ---------------------------- exported functions ----------------------------------

    char *niceAmps( float x );
    char *niceVolts( float x );
    float round0( float x );
    
    void initOLED();
    void updateOLED( int func );
    
    void initWiFi();
// ---------------------------- exported classes ----------------------------------

    extern OLED oled; 
    extern ADS15 ads;  
    extern DIIR qvolts1, qvolts2, qamps;   // decimal IIR filder
    extern SCAN scan;
