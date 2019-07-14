#include "tcpClass.h"
#include "cpuClass.h"

//#define DPRINT(...) Serial.printf(__VA_ARGS__)

void TCP::init( int port, bool echo1, char *prompt1 )
{	
	pserver = new WiFiServer( port ); 			// HEAP ALLOCATED use (ipaddr, port)
	pserver->begin();
	pserver->setNoDelay( true );
	
	buffer 	= new char[80];							// HEAP ALLOCATION
	bufidx 	= 0;
	echo 	= echo1;
	sprompt	= prompt1;
	available = false;
}
bool TCP::ready()
{
	if ( !available )
	{
		client = pserver->available();
		if (client)											// Wait until the client sends some data
		{			
			if ( client.connected() )
			{
				delay(100);
				PR("Client connected" );
				client.flush();
				available = true;
				if( sprompt && *sprompt )
					client.printf( "\r\n%s", sprompt );		// print prompt when client is connected
			}
		}
		bufidx = 0;
	}
	if( available )
	{
		if ( !client.connected() )
		{
			PR("Client disconnected");
			client.stop();
			available = false;
		}
		else
		{
			while(client.available() > 0)
			{
				char c = client.read();
				//PN( c );
				if ( echo )
				  client.write(c);
				
				if( c == '\r' )
				{
					//PN("\n");
					if ( echo )
						client.write('\n');
					
					client.flush();
					buffer[ bufidx ] = 0;
					bufidx = 0;						// next char goes back to the beginning
					return true;
				}
				if( (c >= 0x20) && (c < 0x7F) )
				{
					buffer[ bufidx ] = c;
					if( bufidx < 79 )
						bufidx++;
				}
			} 										// end of while()
		}
	}
	return false;
}
char *TCP::gets()
{
  return &buffer[0];
}
char TCP::prompt()
{
	if( sprompt )			// not NULL
		if( *sprompt )		// not EOS
		
			if ( available && client.connected() )
    			client.print( sprompt );
}
bool TCP::respond( char *s )
{
	bool ok;
	ok = available && client.connected();
	if( s && *s )							// not NULL and not EOS
		client.print( s );
	return ok;
}
