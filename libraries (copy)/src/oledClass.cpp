 /*
  06/01/2016
  Author: Makerbro
  Platforms: ESP8266
  Language: C++
  File: SSD1306.cpp
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

// GKE: NO MODIFICATIONS TO THIS FILE

#include "oledClass.h"

OLED::OLED(uint8_t sda, uint8_t scl, uint8_t address ) 
{
	_sda = sda;
	_scl = scl;
	_address = address;
	_offset = 0;            // set by the init()
}
#define sendcommand sendCommand
void OLED::init( dsptype_t tp, const uint8_t* font )
{
	Wire.begin(_sda, _scl);
  	Wire.setClock(700000);
	if( tp==OLED096 ) 
    {
        _offset = 0; initLarge();
    }
	else if( tp==OLED091 )
	{
	    _offset = 0; initSmall();
	}
    else
    {
        _offset = 2; initLarge();
    }    

    m_font = font;				// select font
  	m_font_width = pgm_read_byte(&m_font[0]);
  	m_size = tp;
    clearDisplay();
	format=0; // default font is small
}
void OLED::initLarge()
{
	sendCommand(0xAE);            //display off
	sendCommand(0xA6);            //Set Normal Display (default)
	sendCommand(0xAE);            //DISPLAYOFF
	
	sendCommand(0xD5);            //SETDISPLAYCLOCKDIV
	sendCommand(0x80);            // the suggested ratio 0x80
	
	sendCommand(0xA8);            //SSD1306_SETMULTIPLEX
	sendCommand(0x3F);
	//sendCommand(0x1F);
	sendCommand(0xD3);            //SETDISPLAYOFFSET
	sendCommand(0x0);             //no offset
	
	sendCommand(0x40|0x0);        //SETSTARTLINE
	sendCommand(0x8D);            //CHARGEPUMP
	sendCommand(0x14);
	
	sendCommand(0x20);            //MEMORYMODE
	sendCommand(0x00);            //0x0 act like ks0108
	// sendCommand(0x02);            //0x0 act like ks0108 ------------------------------
	sendCommand(0xA1);            //SEGREMAP   Mirror screen horizontally (A0)
	sendCommand(0xC8);            //COMSCANDEC Rotate screen vertically (C0)

	sendCommand(0xDA);            //0xDA
	sendCommand(0x12);            //COMSCANDEC
	//sendcommand(0x02);          // com pin HW config, sequential com pin config (bit 4), disable left/right remap (bit 5)
	
	sendCommand(0x81);            //SETCONTRAST
	sendCommand(0xCF);            //
	
	sendCommand(0xd9);            //SETPRECHARGE 
	sendCommand(0xF1); 
	sendCommand(0xDB);            //SETVCOMDETECT                
	sendCommand(0x40);

	sendCommand(0x2e);          	// Disable scroll
	sendCommand(0xA4);            //DISPLAYALLON_RESUME        
	sendCommand(0xA6);            //NORMALDISPLAY             

	sendCommand(0x20);            //Set Memory Addressing Mode
	sendCommand(0x00);            //Set Memory Addressing Mode ab Horizontal addressing mode
}
void OLED::initSmall()
{
    sendcommand(0xae);		      // display off
  	sendcommand(0xa6);          // Set Normal Display (default)

    sendcommand(0xAE);        	// DISPLAYOFF
    sendcommand(0xD5);        	// SETDISPLAYCLOCKDIV
    sendcommand(0x80);        	// the suggested ratio 0x80
    sendcommand(0xA8);        	// SSD1306_SETMULTIPLEX
    sendcommand(0x1F);
    sendcommand(0xD3);        	// SETDISPLAYOFFSET
    sendcommand(0x00);         	// no offset
    sendcommand(0x40 | 0x0);  	// SETSTARTLINE
    sendcommand(0x8D);        	// CHARGEPUMP
    sendcommand(0x14);          // 0x014 enable, 0x010 disable

    sendcommand(0x20);          // com pin HW config, sequential com pin config (bit 4), disable left/right remap (bit 5),
    sendcommand(0x02);          // 0x12 //128x32 OLED: 0x002,  128x32 OLED 0x012
    sendcommand(0xa1);          // segment remap a0/a1
    sendcommand(0xc8);          // c0: scan dir normal, c8: reverse
    
    sendcommand(0xda);
    sendcommand(0x02);          // com pin HW config, sequential com pin config (bit 4), disable left/right remap (bit 5)
    sendcommand(0x81);
    sendcommand(0xcf);          // [2] set contrast control
    sendcommand(0xd9);
    sendcommand(0xf1);          // [2] pre-charge period 0x022/f1
    sendcommand(0xdb);
    sendcommand(0x40);          // vcomh deselect level

    sendcommand(0x2e);          // Disable scroll
    sendcommand(0xa4);          // output ram to display
    sendcommand(0xa6);          // none inverted normal display mode
    sendcommand(0xaf);          // display on
}

void OLED::sendCommand(unsigned char command)
{
  Wire.beginTransmission(_address);    // begin I2C communication
  Wire.write(SSD1306_Command_Mode);           // Set OLED Command mode
  Wire.write(command);
  Wire.endTransmission();                       // End I2C communication
}

void OLED::setBrightness(unsigned char Brightness)
{
   sendCommand(SSD1306_Set_Brightness_Cmd);
   sendCommand(Brightness);
}



void OLED::setTextXY(unsigned char row, unsigned char col)
{
    sendCommand(0xB0 + row);                          //set page address
    sendCommand(0x00 + _offset + (m_font_width*col & 0x0F));    //set column lower addr ---------------
    sendCommand(0x10 + ((m_font_width*col>>4)&0x0F)); //set column higher addr
}

void OLED::displayOn(void)
{
  sendcommand(SSD1306_Display_On_Cmd);        //display on
}
void OLED::displayOff(void)
{
  sendcommand(SSD1306_Display_Off_Cmd);		//display off
}
void OLED::clearDisplay()
{
  unsigned char i,j;
  sendCommand(SSD1306_Display_Off_Cmd);     //display off
  for(j=0;j<8;j++)
  {    
    setTextXY(j,0);    
    {
      for(i=0;i<16;i++)  //clear all columns
      {
        putChar(' ');    
      }
    }
  }
  sendCommand(SSD1306_Display_On_Cmd);     //display on
  setTextXY(0,0);    
}

void OLED::sendData(unsigned char Data)
{
     Wire.beginTransmission(_address); // begin I2C transmission
     Wire.write(SSD1306_Data_Mode);            // data mode
     Wire.write(Data);
     Wire.endTransmission();                    // stop I2C transmission
}

bool OLED::putChar(unsigned char ch)
{
    if (!m_font) return 0;
    //Ignore non-printable ASCII characters. This can be modified for
    //multilingual font.  
    if(ch < 32 || ch > 127) 
    {
        ch = ' ';
    }    
    for(unsigned char i=0;i<m_font_width;i++)
    {
       // Font array starts at 0, ASCII starts at 32
       sendData(pgm_read_byte(&m_font[(ch-32)*m_font_width+m_font_offset+i])); 
    }
    return 1;
}

#ifdef OLED_EXPANDED
		void OLED::setHorizontalMode()
		{
			addressingMode = HORIZONTAL_MODE;
			sendCommand(0x20);                      //set addressing mode
			sendCommand(0x00);                      //set horizontal addressing mode
		}

		void OLED::setPageMode()
		{
			addressingMode = PAGE_MODE;
			sendCommand(0x20);                      //set addressing mode
			sendCommand(0x02);                      //set page addressing mode
		}

		void OLED::putString(const char *string)
		{
			unsigned char i=0;
			while(string[i])
			{
				putChar(string[i]);     
				i++;
			}
		}

		void OLED::putString(String string)
		{
			char char_array[string.length()+1];
			string.toCharArray(char_array, sizeof(char_array));
			putString(char_array);
		}

		unsigned char OLED::putNumber(long long_num)
		{
		  unsigned char char_buffer[10]="";
		  unsigned char i = 0;
		  unsigned char f = 0;

		  if (long_num < 0) 
		  {
			f=1;
			putChar('-');
			long_num = -long_num;
		  } 
		  else if (long_num == 0) 
		  {
			f=1;
			putChar('0');
			return f;
		  } 

		  while (long_num > 0) 
		  {
			char_buffer[i++] = long_num % 10;
			long_num /= 10;
		  }

		  f=f+i;
		  for(; i > 0; i--)
		  {
			putChar('0'+ char_buffer[i - 1]);
		  }
		  return f;

		}

		unsigned char OLED::putFloat(float floatNumber,unsigned char decimal)
		{
		  unsigned int temp=0;
		  float decy=0.0;
		  float rounding = 0.5;
		  unsigned char f=0;
		  if(floatNumber<0.0)
		  {
			putString("-");
			floatNumber = -floatNumber;
			f +=1;
		  }
		  for (unsigned char i=0; i<decimal; ++i)
		  {
			rounding /= 10.0;
		  }
			floatNumber += rounding;
		  
		  temp = floatNumber;
		  f += putNumber(temp);
		  if(decimal>0)
		  {
			putChar('.');
			f +=1;
		 }
		  decy = floatNumber-temp;//decimal part, 
		  for(unsigned char i=0;i<decimal;i++)//4 
		  {
			decy *=10;// for the next decimal
			temp = decy;//get the decimal
			putNumber(temp);
			decy -= temp;
		  }
		  f +=decimal;
		  return f;
		}
		unsigned char OLED::putFloat(float floatNumber)
		{
		  unsigned char decimal=2;
		  unsigned int temp=0;
		  float decy=0.0;
		  float rounding = 0.5;
		  unsigned char f=0;
		  if(floatNumber<0.0)
		  {
			putString("-");
			floatNumber = -floatNumber;
			f +=1;
		  }
		  for (unsigned char i=0; i<decimal; ++i)
		  {
			rounding /= 10.0;
		  }
			floatNumber += rounding;
		  
		  temp = floatNumber;
		  f += putNumber(temp);
		  if(decimal>0)
		  {
			putChar('.');
			f +=1;
		 }
		  decy = floatNumber-temp;//decimal part, 
		  for(unsigned char i=0;i<decimal;i++)//4 
		  {
			decy *=10;// for the next decimal
			temp = decy;//get the decimal
			putNumber(temp);
			decy -= temp;
		  }
		  f +=decimal;
		  return f;
		}

		void OLED::drawBitmap(unsigned char *bitmaparray,int bytes)
		{
		  char localAddressMode = addressingMode;
		  if(addressingMode != HORIZONTAL_MODE)
		  {
			  //Bitmap is drawn in horizontal mode     
			  setHorizontalMode();
		  }

		  for(int i=0;i<bytes;i++)
		  {
			  sendData(pgm_read_byte(&bitmaparray[i]));
		  }

		  if(localAddressMode == PAGE_MODE)
		  {
			 //If pageMode was used earlier, restore it.
			 setPageMode(); 
		  }
		  
		}

		void OLED::setHorizontalScrollProperties(bool direction,unsigned char startPage, unsigned char endPage, unsigned char scrollSpeed)
		{
		   if(Scroll_Right == direction)
		   {
				//Scroll right
				sendCommand(0x26);
		   }
		   else
		   {
				//Scroll left  
				sendCommand(0x27);

		   }
			sendCommand(0x00);
			sendCommand(startPage);
			sendCommand(scrollSpeed);
			sendCommand(endPage);
			sendCommand(0x00);
			sendCommand(0xFF);
		}

		void OLED::activateScroll()
		{
			sendCommand(SSD1306_Activate_Scroll_Cmd);
		}

		void OLED::deactivateScroll()
		{
			sendCommand(SSD1306_Dectivate_Scroll_Cmd);
		}

		void OLED::setNormalDisplay()
		{
			sendCommand(SSD1306_Normal_Display_Cmd);
		}

		void OLED::setInverseDisplay()
		{
			sendCommand(SSD1306_Inverse_Display_Cmd);
		}
#endif

static char b21( char c )
{
  char d = 0;
  
  if( c&1)
    d |= 0b00000011;
  if( c&2 )
    d |= 0b00001100;
  if( c&4 )
    d |= 0b00110000;
  if( c&8 )
    d |= 0b11000000;
  return d;
}
void OLED::clearLine( int X, int Y )
{
  for( int i=Y; (i<16) && (i>=0); i++ )
	putChar(' ');
  setTextXY( X, Y );
} 

const char * OLED::display( int X, int Y, const char *string)
{
    char data;
    int col;
    char *sp, *retp;
     
    setTextXY( X, Y);
    sp = (char *)string;
    retp = (*sp<' ')? sp+1 : sp;    // skip control character if there
    
    col = Y;
    format = 0;                     // default format
    while( data = *sp++ )
    {
      if( data < 0x20 )
      {
        switch( data )
        {
          default:
          case '\a': format=0; break;
          case '\v': format=1; break;
          case '\b': format=2; break;
          case '\r': clearLine( X, col ); 
                      break;
        }
      }
      else
      {
        for(int i=0;i<m_font_width;i++) //m_font_width
        {
//        char ch = pgm_read_byte(  myFont[data-0x20]                           +i);
	      char ch = pgm_read_byte( &m_font[(data-32   )*m_font_width+m_font_offset+i]); 

          
          if( format == 1 )
            sendData( b21(ch) );
          else if( format == 2 )
          {
            sendData( b21(ch) );
            sendData( b21(ch) );
          }
          else
            sendData( ch );
        }
        col = (format==2)? col+2 : col+1;
      }
    }
    if( format <=0 )
      return retp;

    setTextXY(X+1, Y);
    sp = (char *)string;
    col = Y;
    while( data = *sp++ )
    {
      if( data < 0x20 )
      {
        if( data=='\r' )          // erase to end of this row
          clearLine(X+1,col);
      }
      else
      {
        for(int i=0;i<m_font_width;i++)	
        {
//        char ch = pgm_read_byte( myFont[data-0x20]                        +i )>>4;
          char ch = pgm_read_byte(&m_font[(data-32)*m_font_width+m_font_offset+i])>>4;
          
          if(format==1)
            sendData( b21(ch) );
          if(format==2)
          {
            sendData( b21(ch) );
            sendData( b21(ch) );
          }
        }
        col = (format==2)? col+2 : col+1;
      }
    }
    return retp;
}

//const char *OLED::display( int X, int Y, const char *string)
//{
//    setTextXY( X, Y);
//    unsigned char i=0;
//    int ch;
//    
//    while( (ch = string[i]))
//    {
//        putChar( ch );  // EQUIVALENT! 
//        i++;
//    }
//    return string;
//}
const char *OLED::dsp( int X, int Y, const char *format, ... )
{
	va_list ap;
	va_start( ap, format );
	vsnprintf( temp, sizeof( temp ), format, ap );
	va_end( ap );
    return display( X, Y, temp );
}
const char *OLED::dsp( int X, const char *format, ... )
{
    va_list ap;
    va_start( ap, format );
    vsnprintf( temp, sizeof( temp ), format, ap );
    va_end( ap );
    
    // center text on blank line
    int siz = strlen( temp );   // length of the string to be displayed
    char newlin[20];
    memset( newlin, ' ', 17 );  // create a blank line with spaces

    switch( temp[0] )
    {
            case '\a':          // single width characters
            case '\v':
                newlin[17] = 0;
                strncpy( newlin+1+(16-siz+1)/2, temp+1, siz-1 );
                newlin[0] = temp[0];
                break;
            
            case '\b':          // double width
                newlin[8] = 0;
                strncpy( newlin+1+(8-siz+1)/2, temp+1, siz-1 );
                newlin[0] = '\b';
                break;
            
            default:            // assume \a or \v
                newlin[16] = 0;
                strncpy( newlin+(16-siz)/2, temp, siz );
                break;
    }
    display( X, 0, newlin );
    return format;
}
