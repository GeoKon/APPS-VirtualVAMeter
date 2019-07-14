#include <string.h>
#include "bufClass.h"
#include "eepClass.h"

// -------------------------------- CLASS EEP --------------------------------------
	int EEP::notify( char *s )
	{
		PF( "*** EEP Class ERROR: %s ***\r\n", s );
		return -1;
	}
	void EEP::initHeadParms( uint16_t emagic, uint16_t nuser )
	{
		head.magic = emagic;
        head.headN = HEAD_PSIZE;
        head.userN = nuser;
		saveHeadParms();
	}
	bool EEP::checkEEParms( uint16_t emagic, uint16_t nuser )
	{
		fetchHeadParms();
		fetchWiFiParms();
		return  (head.reboots != 0 ) &&
				(head.magic == emagic) && 
				(head.headN == HEAD_PSIZE ) &&
				(head.userN == nuser ) &&
				(wifi.port != 0);
	}
	// If "" deletes value. If NULL or '*', leaves unmodified
    void EEP::initWiFiParms( char *myssid, 
                        char *mypwd, 
                        char *staticIP, 
                        int myport )    // initializes memory WiFi parms
    {
        if( myssid && (*myssid !='*') )
            strncpy( wifi.ssid, myssid, 16 );
        if( mypwd && (*mypwd !='*'))
            strncpy( wifi.pwd, mypwd, 16 );
        if( staticIP && (*staticIP !='*') )
            strncpy( wifi.stIP, staticIP, 16 );
        if( myport )
            wifi.port = myport;
		saveWiFiParms();
    }
    void EEP::saveHeadParms()
    {
        byte c;
        EEPROM.begin( HEAD_PSIZE + WIFI_PSIZE + USER_MAXSZ );
        EEPROM.put( 0, head );            
        EEPROM.end();                   
    }
    void EEP::saveWiFiParms()
    {
        EEPROM.begin( HEAD_PSIZE + WIFI_PSIZE + USER_MAXSZ );
        EEPROM.put( HEAD_PSIZE, wifi );                         // save the WiFi parm structure
        EEPROM.end();                   // commit
    }
    void EEP::saveUserStruct( byte *pntr, int actsiz )
    {
		//prnbuf("SAVE ", pntr, actsiz );
		if( actsiz > USER_MAXSZ )
		{
			notify( "USER_MAXSZ exceeded" );
			return;
		}
        byte c;
		byte *up = pntr;
		head.userN = actsiz;									// save this into header
		saveHeadParms();
		
		EEPROM.begin( HEAD_PSIZE + WIFI_PSIZE + USER_MAXSZ );		
		for( int i=0; i<actsiz; i++ )                       	// write user
			EEPROM.put( HEAD_PSIZE+WIFI_PSIZE+i, (c=*up++) );
        EEPROM.end();                   						// commit
    }
    int EEP::fetchHeadParms() 
    {
        EEPROM.begin( HEAD_PSIZE + WIFI_PSIZE + USER_MAXSZ );
        EEPROM.get( 0, head );                     				// fetch HEADER_MASK candidate
        EEPROM.end();                   						// commit
        return 0;
    }
    int EEP::fetchWiFiParms()
    {
        EEPROM.begin( HEAD_PSIZE + WIFI_PSIZE + USER_MAXSZ );
        EEPROM.get( HEAD_PSIZE, wifi );  
        EEPROM.end();    
        return 0;
    }
    int EEP::fetchUserStruct( byte *pntr, int maxsiz )
    {
		byte *p  = pntr;

		fetchHeadParms();										// get header
		if( maxsiz < head.userN )
			return notify( "Too small user buffer" );
		
        EEPROM.begin( HEAD_PSIZE + WIFI_PSIZE + USER_MAXSZ );
		int c;
		for( int i=0; i<head.userN; i++ )     
		{
			EEPROM.get( HEAD_PSIZE+WIFI_PSIZE+i, c );
			*p++ = c;
		}
        EEPROM.end(); 
		// prnbuf("FTCH ", pntr, head.userN );		
		return 0;
    }	
    void EEP::incrBootCount()
    {
        head.reboots++;
        saveHeadParms();
    }
    String EEP::getHeadString()
    {
        BUFT <256> s;
		s.set("Magic:%04x, Head_sz:%d, User_sz:%d, Boot_count:%d\r\n", 
                head.magic, head.headN, head.userN, head.reboots );
		return String( !s );
    }
    String EEP::getWiFiString()
    {
		BUFT <256> s;
		s.set("SSID:%s, ", 	 &wifi.ssid[0] );
		s.add("PWD:%s, ", 	 &wifi.pwd[0] );
		s.add("stIP:%s, ", 	 &wifi.stIP[0] );
		s.add("Port:%d\r\n", wifi.port );
		return String( !s );
    }
	void EEP::printHeadParms( char *prompt )
	{
		if( *prompt )
            PF( "%s\r\n", prompt );
		PR( getHeadString() );
	}
	void EEP::printWiFiParms( char *prompt )
	{
		if( *prompt )
            PF( "%s\r\n", prompt );
		PR( getWiFiString() );
	}

