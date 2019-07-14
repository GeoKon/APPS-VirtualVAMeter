#pragma once
#include <ESP8266WiFi.h>

class TCP
{
    WiFiServer *pserver;	// pointer to this TCP server
    WiFiClient client;		// pointer to the remote TCP client
    
    char *buffer;			// pointer to command buffer
    int bufidx;				// number of bytes into buffer
    
    bool available;			// true = client connected, ready to exchange data
    bool echo;				// true = do echo
    char *sprompt;			// pointer to user supplied prompt

public:
    void init( int port, bool echo, char *prompt="" );
    bool ready();
    char *gets();
    char prompt();
	bool respond( char *s );	// true if client available
};

