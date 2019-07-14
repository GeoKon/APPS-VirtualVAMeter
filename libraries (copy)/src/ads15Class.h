#pragma once
#include "Arduino.h"
#include <Wire.h>

//    I2C ADDRESS/BITS
    #define ADS1115_ADDRESS                 (0x48)    // 1001 000 (ADDR = GND)

//    CONVERSION DELAY (in mS)
    #define ADS1115_CONVERSIONDELAY         (1L)
    #define ADS1115_CONVERSIONDELAY         (8L)

//    POINTER REGISTER
    #define ADS1115_REG_POINTER_MASK        (0x03)
    #define ADS1115_REG_POINTER_CONVERT     (0x00)
    #define ADS1115_REG_POINTER_CONFIG      (0x01)
    #define ADS1115_REG_POINTER_LOWTHRESH   (0x02)
    #define ADS1115_REG_POINTER_HITHRESH    (0x03)

//    CONFIG REGISTER
    #define ADS1115_REG_CONFIG_OS_MASK      (0x8000)
    #define ADS1115_REG_CONFIG_OS_SINGLE    (0x8000)  // Write: Set to start a single-conversion
    #define ADS1115_REG_CONFIG_OS_BUSY      (0x0000)  // Read: Bit = 0 when conversion is in progress
    #define ADS1115_REG_CONFIG_OS_NOTBUSY   (0x8000)  // Read: Bit = 1 when device is not performing a conversion

    #define ADS1115_REG_CONFIG_MUX_MASK     (0x7000)
    #define ADS1115_REG_CONFIG_MUX_DIFF_0_1 (0x0000)  // Differential P = AIN0, N = AIN1 (default)
    #define ADS1115_REG_CONFIG_MUX_DIFF_0_3 (0x1000)  // Differential P = AIN0, N = AIN3
    #define ADS1115_REG_CONFIG_MUX_DIFF_1_3 (0x2000)  // Differential P = AIN1, N = AIN3
    #define ADS1115_REG_CONFIG_MUX_DIFF_2_3 (0x3000)  // Differential P = AIN2, N = AIN3
    #define ADS1115_REG_CONFIG_MUX_SINGLE_0 (0x4000)  // Single-ended AIN0
    #define ADS1115_REG_CONFIG_MUX_SINGLE_1 (0x5000)  // Single-ended AIN1
    #define ADS1115_REG_CONFIG_MUX_SINGLE_2 (0x6000)  // Single-ended AIN2
    #define ADS1115_REG_CONFIG_MUX_SINGLE_3 (0x7000)  // Single-ended AIN3

    #define ADS1115_REG_CONFIG_PGA_MASK     (0x0E00)
    #define ADS1115_REG_CONFIG_PGA_6_144V   (0x0000)  // +/-6.144V range = Gain 2/3
    #define ADS1115_REG_CONFIG_PGA_4_096V   (0x0200)  // +/-4.096V range = Gain 1
    #define ADS1115_REG_CONFIG_PGA_2_048V   (0x0400)  // +/-2.048V range = Gain 2 (default)
    #define ADS1115_REG_CONFIG_PGA_1_024V   (0x0600)  // +/-1.024V range = Gain 4
    #define ADS1115_REG_CONFIG_PGA_0_512V   (0x0800)  // +/-0.512V range = Gain 8
    #define ADS1115_REG_CONFIG_PGA_0_256V   (0x0A00)  // +/-0.256V range = Gain 16

    #define ADS1115_REG_CONFIG_MODE_MASK    (0x0100)
    #define ADS1115_REG_CONFIG_MODE_CONTIN  (0x0000)  // Continuous conversion mode
    #define ADS1115_REG_CONFIG_MODE_SINGLE  (0x0100)  // Power-down single-shot mode (default)

    #define ADS1115_REG_CONFIG_DR_MASK      (0x00E0)  
    #define ADS1115_REG_CONFIG_DR_128SPS    (0x0000)  // 128 samples per second
    #define ADS1115_REG_CONFIG_DR_250SPS    (0x0020)  // 250 samples per second
    #define ADS1115_REG_CONFIG_DR_490SPS    (0x0040)  // 490 samples per second
    #define ADS1115_REG_CONFIG_DR_920SPS    (0x0060)  // 920 samples per second
    #define ADS1115_REG_CONFIG_DR_1600SPS   (0x0080)  // 1600 samples per second (default)
    #define ADS1115_REG_CONFIG_DR_2400SPS   (0x00A0)  // 2400 samples per second
    #define ADS1115_REG_CONFIG_DR_3300SPS   (0x00C0)  // 3300 samples per second

    #define ADS1115_REG_CONFIG_CMODE_MASK   (0x0010)
    #define ADS1115_REG_CONFIG_CMODE_TRAD   (0x0000)  // Traditional comparator with hysteresis (default)
    #define ADS1115_REG_CONFIG_CMODE_WINDOW (0x0010)  // Window comparator

    #define ADS1115_REG_CONFIG_CPOL_MASK    (0x0008)
    #define ADS1115_REG_CONFIG_CPOL_ACTVLOW (0x0000)  // ALERT/RDY pin is low when active (default)
    #define ADS1115_REG_CONFIG_CPOL_ACTVHI  (0x0008)  // ALERT/RDY pin is high when active

    #define ADS1115_REG_CONFIG_CLAT_MASK    (0x0004)  // Determines if ALERT/RDY pin latches once asserted
    #define ADS1115_REG_CONFIG_CLAT_NONLAT  (0x0000)  // Non-latching comparator (default)
    #define ADS1115_REG_CONFIG_CLAT_LATCH   (0x0004)  // Latching comparator

    #define ADS1115_REG_CONFIG_CQUE_MASK    (0x0003)
    #define ADS1115_REG_CONFIG_CQUE_1CONV   (0x0000)  // Assert ALERT/RDY after one conversions
    #define ADS1115_REG_CONFIG_CQUE_2CONV   (0x0001)  // Assert ALERT/RDY after two conversions
    #define ADS1115_REG_CONFIG_CQUE_4CONV   (0x0002)  // Assert ALERT/RDY after four conversions
    #define ADS1115_REG_CONFIG_CQUE_NONE    (0x0003)  // Disable the comparator and put ALERT/RDY in high state (default)

typedef enum
{
  GAIN_TWOTHIRDS    = ADS1115_REG_CONFIG_PGA_6_144V,
  GAIN_ONE          = ADS1115_REG_CONFIG_PGA_4_096V,
  GAIN_TWO          = ADS1115_REG_CONFIG_PGA_2_048V,
  GAIN_FOUR         = ADS1115_REG_CONFIG_PGA_1_024V,
  GAIN_EIGHT        = ADS1115_REG_CONFIG_PGA_0_512V,
  GAIN_SIXTEEN      = ADS1115_REG_CONFIG_PGA_0_256V
} adsGain_t;

typedef enum 
{
	FS_6144mV = 6144,
	FS_4096mV = 4096,
	FS_2048mV = 2048,
	FS_1024mV = 1024,
	FS_512mV = 512,
	FS_256mV = 256
} fullscale_t;

#define i2cwrite(A) Wire.write(A)
#define i2cread     Wire.read

class ADS15
{
protected:
									// Instance-specific properties
    uint8_t   	my_i2cAddress;
    int16_t 	my_config[6];		// configuration value per channel
    uint32_t 	my_tstart;			// millis() at the beginning of the conversion
    
    // Writes 16-bits to the specified destination register
    static void writeRegister(uint8_t i2cAddress, uint8_t reg, uint16_t value) 
    {
        Wire.beginTransmission(i2cAddress);
        i2cwrite((uint8_t)reg);
        i2cwrite((uint8_t)(value>>8));
        i2cwrite((uint8_t)(value & 0xFF));
        Wire.endTransmission();
    }
    // Writes 16-bits to the specified destination register
    static uint16_t readRegister(uint8_t i2cAddress, uint8_t reg) 
    {
        Wire.beginTransmission(i2cAddress);
        i2cwrite(ADS1115_REG_POINTER_CONVERT);
        Wire.endTransmission();
        Wire.requestFrom(i2cAddress, (uint8_t)2);
        return ((i2cread() << 8) | i2cread());  
    }
 	adsGain_t setFS( fullscale_t f );
	
public:
    ADS15(); 
    void init( int i2cAddress = 0x48 );				// ADDR pin to GND
	void initChannel( int channel, fullscale_t f ); // 0,1,2,3 Single Ended; 4,5 Differential
    void startConversion( int channel );
    int16_t readConversion();
    float toVolts( int channel, int reading  );
    int16_t toADC( int channel, float volts  );
};	