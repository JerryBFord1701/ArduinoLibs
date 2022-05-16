/*
    DATE-START: 01/14/2022
          TIME: 14:22
     DATE-LAST: 5/8/2022
         TITLE: MCP4728 Driver (MCP4728 HAL)
        AUTHOR: Jerome B. Ford
          NOTE: Still under development. as of 5/08/2022
         LINKS: http://ww1.microchip.com/downloads/en/DeviceDoc/22187E.pdf
           https://learn.adafruit.com/adafruit-mcp4728-i2c-quad-dac
 
*/
 
#include "MCP4728.h"

// 00 ---------------------------------------------------------------------------//
MCP4728::MCP4728()
{
    Wire.begin(); // Join i2c bus as master.
}
// 01 ---------------------------------------------------------------------------//
MCP4728::MCP4728(uint8_t newValue)
{
    _deviceAddress = (newValue & 0x67);
    Wire.begin(); // Join i2c bus as master.
}
// 02 ---------------------------------------------------------------------------//
MCP4728::MCP4728(uint8_t newValue, uint8_t newAlertPin)
{
    _deviceAddress = (newValue & 0x67);
    _AlertPin = newAlertPin;
    pinMode(_AlertPin, INPUT);
    Wire.begin(); // Join i2c bus as master.
}
// 03 ---------------------------------------------------------------------------//
MCP4728::~MCP4728()
{
     EndWire();
}
// 04 ---------------------------------------------------------------------------//
void MCP4728::EndWire()
{
    Wire.endTransmission(); // Not sure if this is needed!
    Wire.end();
    TWCR &= ~0x04;          // <= NOT SURE ABOUT THIS!
}
// 10 ---------------------------------------------------------------------------//
void MCP4728::Set_DAC_CHNL(MCP4728::MULTI_WR_CMND theDacCfg)
{
    Wire.beginTransmission(_deviceAddress); 
    
    // MULTI_WR_CMND multiWrVal;
    // multiWrVal.fields.DACCount = newValue;                                      // Bits [0-11]
    // multiWrVal.fields.GainSelect = MCP4728::GAIN_REF::GAIN_2X;                  // Bits [12]
    // multiWrVal.fields.PowerDownSelect = MCP4728::PWR_DOWN_MODE::DAC_OUT_NORMAL; // Bits [13-14]
    // multiWrVal.fields.VrefSelect = MCP4728::VREF_SOURCE::INTERNAL;              // Bits [15]
    // multiWrVal.fields.UpDacEnbl = MCP4728::UPDATE_DAC::ENABLE;                  // Bits [16]
    // multiWrVal.fields.DacChnl = chnl;                                           // Bits [17-18]
    // multiWrVal.fields.WrComand = 0;                                             // Bits [19-20]
    // multiWrVal.fields.Command = 2;                                              // Bits [21-23]

    Wire.write(theDacCfg.regBytes[2]);
    Wire.write(theDacCfg.regBytes[1]);
    Wire.write(theDacCfg.regBytes[0]);
    Wire.endTransmission();
}
// 11 ---------------------------------------------------------------------------//
void MCP4728::Set_DAC_CHNL_DAC(MCP4728::CHANNEL chnl, uint16_t newValue)
{
    _dacCfg[chnl].fields.DACCount = newValue;   // Bits [0-11]
    _dacCfg[chnl].fields.DacChnl = chnl;        // Bits [17-18]
    _dacCfg[chnl].fields.WrComand = 0;          // Bits [19-20]
    _dacCfg[chnl].fields.Command = 2;           // Bits [21-23]
    Set_DAC_CHNL(_dacCfg[chnl]);
}
// 13 ---------------------------------------------------------------------------//
void MCP4728::Set_DAC_CHNL_GainSelect(MCP4728::CHANNEL chnl, MCP4728::GAIN_REF newValue)
{
    _dacCfg[chnl].fields.GainSelect = newValue;
}
// 15 ---------------------------------------------------------------------------//
void MCP4728::Set_DAC_CHNL_PowerDownSelect(MCP4728::CHANNEL chnl, MCP4728::PWR_DOWN_MODE newValue)
{
    _dacCfg[chnl].fields.PowerDownSelect = newValue;
}
// 17 ---------------------------------------------------------------------------//
void MCP4728::Set_DAC_CHNL_VrefSelect(MCP4728::CHANNEL chnl, MCP4728::VREF_SOURCE newValue)
{
    _dacCfg[chnl].fields.VrefSelect = newValue;
}
// 19 ---------------------------------------------------------------------------//
void MCP4728::Set_DAC_CHNL_UpDacEnbl(MCP4728::CHANNEL chnl, MCP4728::UPDATE_DAC newValue)
{
    _dacCfg[chnl].fields.UpDacEnbl = newValue;
}
