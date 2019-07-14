#pragma once

#include "ads15Class.h"

    ADS15::ADS15( ) 
    {
        ;
    }
	void ADS15::init( int i2cAddress ) 
	{
		my_i2cAddress = i2cAddress;
		Wire.begin();
	}
	adsGain_t ADS15::setFS( fullscale_t f )
    {
        switch( f )
        {
            default:
            case FS_6144mV: return GAIN_TWOTHIRDS; 
            case FS_4096mV: return GAIN_ONE;
            case FS_2048mV: return GAIN_TWO;
            case FS_1024mV: return GAIN_FOUR;
            case FS_512mV: return GAIN_EIGHT;
            case FS_256mV: return GAIN_SIXTEEN;
        }
    }
    void ADS15::initChannel( int channel, fullscale_t f ) 
    {
        // Start with default values
        uint16_t config = ADS1115_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                        ADS1115_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                        ADS1115_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                        ADS1115_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                        ADS1115_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
                        ADS1115_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)
    
        config |= setFS( f );         // Set PGA/voltage range

        // Set single-ended input channel
        switch (channel)
        {
            case (0): 	config |= ADS1115_REG_CONFIG_MUX_SINGLE_0;  break;
            case (1): 	config |= ADS1115_REG_CONFIG_MUX_SINGLE_1;  break;
            case (2): 	config |= ADS1115_REG_CONFIG_MUX_SINGLE_2;  break;
            case (3): 	config |= ADS1115_REG_CONFIG_MUX_SINGLE_3;  break;
			case (4): 	config |= ADS1115_REG_CONFIG_MUX_DIFF_0_1;  break; // AIN0 = P, AIN1 = N
			case (5): 	config |= ADS1115_REG_CONFIG_MUX_DIFF_2_3;  break; // AIN2 = P, AIN3 = N
        }    
        // Set 'start single-conversion' bit
        config |= ADS1115_REG_CONFIG_OS_SINGLE;
        my_config[ channel ] = config;
    }

    void ADS15::startConversion( int channel )
    {
        // Write config register to the ADC
        writeRegister(my_i2cAddress, ADS1115_REG_POINTER_CONFIG, my_config[ channel ] );
        my_tstart = millis();
    }
    int16_t ADS15::readConversion()
    {
        if( millis() < (my_tstart + ADS1115_CONVERSIONDELAY) ) // Wait for the conversion to complete
            delay( ADS1115_CONVERSIONDELAY );
        return readRegister(my_i2cAddress, ADS1115_REG_POINTER_CONVERT);
    }
	float ADS15::toVolts( int channel, int reading  )
    {
        float frac = (float) reading / (float) 0x7FFF;
        float volts;
		
		int gain = my_config[ channel ] & ADS1115_REG_CONFIG_PGA_MASK;	// get the gain for this channel

        switch( (adsGain_t) gain )
        {
            default:            volts = 2.048 * 3 * frac; break;
            case GAIN_ONE:      volts = 4.096 * frac; break;
            case GAIN_TWO:      volts = 2.048 * frac; break;
            case GAIN_FOUR:     volts = 1.024 * frac; break;
            case GAIN_EIGHT:    volts = 0.512 * frac; break;
            case GAIN_SIXTEEN:  volts = 0.256 * frac; break;            
        }
        return volts;
    } 
	int16_t ADS15::toADC( int channel, float volts  )
    {
        float frac;
		
		int gain = my_config[ channel ] & ADS1115_REG_CONFIG_PGA_MASK;	// get the gain for this channel

        switch( (adsGain_t) gain )
        {
            default:            frac = volts/(2.048 * 3.0); break;
            case GAIN_ONE:      frac = volts/4.096; break;
            case GAIN_TWO:      frac = volts/2.048; break;
            case GAIN_FOUR:     frac = volts/1.024; break;
            case GAIN_EIGHT:    frac = volts/0.512; break;
            case GAIN_SIXTEEN:  frac = volts/0.256; break;            
        }
		if( frac >= 1.00 )
			frac = 0.99996; // this is the 1-1/0x7FFFF
		if( frac <= -1.00 )
			frac = -0.99996;
		return (int16_t) (frac * 32767.0);
    } 	
