// macros.h
#pragma once

	#define PR(A)   			Serial.print(A)
    #define PRN(A)   			Serial.println(A)
	
    #define PF(format, ...)		Serial.printf( format,       ##__VA_ARGS__ )
	#define PFN(format, ...)	Serial.printf( format"\r\n", ##__VA_ARGS__ )
	
    #define CRLF()  			Serial.println("")
	

	