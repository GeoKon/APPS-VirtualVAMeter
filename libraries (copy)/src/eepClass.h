#pragma once

// See eepClass.md for use

#include "macros.h"
#include <EEPROM.h>

#define EEP_MAGIC ('K'*256+'O')
#define CHECK PF

#define HEAD_PSIZE 8            	// sizeof( head_t )
#define WIFI_PSIZE 3*16+2   		// sizeof( wifi_t )
#define USER_MAXSZ 200				// max byte size user parms allowed

class EEP
{
public:	
    struct head_t					// ACCESSIBLE HEAD STRUCTURE
    {
        uint16_t magic;
        uint16_t headN;    			// byte count of this HEAD
        uint16_t userN;    			// byte count of this WIFI
        uint16_t reboots;  			// increments avery time reboots
    } head;         				
    
	struct wifi_t					// ACCESSIBLE WIFI STRUCTURE
	{
		char ssid[16];
		char pwd [16];
		char stIP[16];
		uint16_t port;
	} wifi;
    
    void initHeadParms(					// Initializes HEAD. Saves them to header
		uint16_t  emagic = EEP_MAGIC,	
		uint16_t  nuser = 0 );

	bool checkEEParms( 					// fetches header+WiFi, checks for magic & size
		uint16_t  emagic = EEP_MAGIC,
		uint16_t  nuser = 0 );
		
    void initWiFiParms( 					// initializes memory WiFi parms
		char *myssid="kontopidis2GHz", 		// if "" deletes value. If NULL, leaves unmodified
        char *mypwd="123456789a", 			// Saves into EEPROM
        char *staticIP="", 
        int myport=80 );    
		
	void incrBootCount();					// Changes the boot count and updates the EEPROM
		
	void saveHeadParms();			// saves memory structures to eeprom
	void saveWiFiParms();			// saves memory structures to eeprom
	
	void saveUserStruct( byte *pntr, int actsiz );			// saves memory structures to eeprom
	int fetchUserStruct( byte *pntr, int maxsiz );
		
    int fetchHeadParms();			// fetches to memory; assumes consistent structures. Returns error code
    int fetchWiFiParms();
	
	String getHeadString();
	String getWiFiString();
	
	void printHeadParms( char *prompt="" );			// prints the structure data. Does not fetch()
	void printWiFiParms( char *prompt="" );
	
	int notify( char *s );
};


