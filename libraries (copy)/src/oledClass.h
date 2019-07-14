/*
  06/01/2016
  Author: Makerbro
  Platforms: ESP8266
  Language: C++
  File: SSD1306.h
  ------------------------------------------------------------------------
  Description: 
  SSD1306 OLED Driver Library.
  ------------------------------------------------------------------------
  Please consider buying products from ACROBOTIC to help fund future
  Open-Source projects like this! We'll always put our best effort in every
  project, and release all our design files and code for you to use. 
  https://acrobotic.com/
  ------------------------------------------------------------------------
  License:
  Released under the MIT license. Please check LICENSE.txt for more
  information.  All text above must be included in any redistribution. 
*/

#pragma once

#include "Arduino.h"
#include <pgmspace.h>
#define OLEDFONT(name) static const uint8_t name[]
#include "Wire.h"
#include "fonts/font8x8W.h"

//#include "fonts/font8x8.h"
//#include "fonts/font5x7.h"
//#include "allocated.h"

//#define SSD1306_Max_X                 127    //128 Pixels
//#define SSD1306_Max_Y                 63     //64  Pixels

#define PAGE_MODE                     01
#define HORIZONTAL_MODE               02

//#define SSD1306_Address             0x3C
#define SSD1306_Command_Mode          0x80
#define SSD1306_Data_Mode             0x40
#define SSD1306_Display_Off_Cmd       0xAE
#define SSD1306_Display_On_Cmd        0xAF
#define SSD1306_Normal_Display_Cmd    0xA6
#define SSD1306_Inverse_Display_Cmd   0xA7
#define SSD1306_Activate_Scroll_Cmd   0x2F
#define SSD1306_Dectivate_Scroll_Cmd  0x2E
#define SSD1306_Set_Brightness_Cmd    0x81

#define Scroll_Left                   0x00
#define Scroll_Right                  0x01

#define Scroll_2Frames                0x7
#define Scroll_3Frames                0x4
#define Scroll_4Frames                0x5
#define Scroll_5Frames                0x0
#define Scroll_25Frames               0x6
#define Scroll_64Frames               0x1
#define Scroll_128Frames              0x2
#define Scroll_256Frames              0x3

enum dsptype_t
{
    SMALL = 0,
    OLED091 = 0,    // SSD1306 128x32 oled
    LARGE = 1,
    OLED096 = 1,    // SSD1306 128x64 oled
    BIG = 2,
    OLED130 = 2     // SH1106 128x64 oled
};

class OLED {
  public:
    OLED(uint8_t sda=4, uint8_t scl=5, uint8_t address=0x3c );
    void init( dsptype_t type, const uint8_t* font = font8x8W );
    
    void initLarge();
	void initSmall();

    void setTextXY(unsigned char Row, unsigned char Column);

    void clearDisplay();
    void displayOn();
    void displayOff();
    
    void setBrightness(unsigned char Brightness);
    bool putChar(unsigned char c);

#ifdef OLED_EXPANDED   
    void setPageMode();
    void setHorizontalMode();
	
    void setNormalDisplay();
    void setInverseDisplay();
	
    void putString(const char *string);
    void putString(String string);
    unsigned char putNumber(long n);
    unsigned char putFloat(float floatNumber,unsigned char decimal);
    unsigned char putFloat(float floatNumber);
    void drawBitmap(unsigned char *bitmaparray,int bytes);
    void setHorizontalScrollProperties(
        bool direction,
        unsigned char startPage, 
        unsigned char endPage, 
        unsigned char scrollSpeed);
    void activateScroll();
    void deactivateScroll();
#endif

	const char *dsp( int r, int c, const char *format, ... );
    const char *dsp( int r, const char *format, ... );          // CENTERED
	const char *display( int r, int c, const char *s );
	void clearLine( int x, int y );

    void sendCommand(unsigned char command);
    void sendData(unsigned char Data);

 protected:
    const uint8_t* m_font;      // Current font.
    uint8_t m_font_offset = 2;  // Font bytes for meta data.
    uint8_t m_font_width;       // Font witdth.
    bool  m_size;		 	    // true=128x64, false=128x32
 private:
    char temp[20];				// format string
    uint8_t m_col;              // Cursor column.
    uint8_t m_row;              // Cursor row (RAM). 
    uint8_t _sda, _scl, _address, _offset;
    int format;
    char addressingMode;
};
