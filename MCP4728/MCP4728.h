/*
    DATE: 01/14/2022
    TIME: 14:22
   TITLE: MCP4728 Driver (MCP4728 HAL)
  AUTHOR: Jerome B. Ford
    NOTE: Still under development. as of 5/08/2022
    LINKS: http://ww1.microchip.com/downloads/en/DeviceDoc/22187E.pdf
           https://learn.adafruit.com/adafruit-mcp4728-i2c-quad-dac
 
*/

#ifndef MCP4728_h
#define MCP4728_h

#include "Wire.h"
#include "Arduino.h"

class MCP4728
{

    public:

        enum COMMAND_TYPE
        {
            MULTI_INP_REG_CMD = 0x40,   //< Command to write to the input register only
            MULTI_EEPROM_CMD = 0x50,    //< Command to write to the input register and EEPROM
            FAST_WRITE_CMD = 0xC0,      //< Command to write all channels at once with
        };

        enum PWR_DOWN_MODE 
        {
            DAC_OUT_NORMAL, //< Normal; the channel outputs the given value as normal.
            DAC_GND_1K,     //< VOUT is loaded with 1 kΩ resistor to ground. Most of the channel circuits are powered off.
            DAC_GND_100K,   //< VOUT is loaded with 100 kΩ resistor to ground. Most of the channel circuits are powered off.
            DAC_GND_500K,   //< VOUT is loaded with 500 kΩ resistor to ground. Most of the channel circuits are powered off.
        }; 

        enum GAIN_REF
        {
            GAIN_1X = 0,
            GAIN_2X = 1,
        };


        enum VREF_SOURCE
        {
            VDD = 0,
            INTERNAL = 1,
        };

        enum UPDATE_DAC
        {
            ENABLE = 0,
            DISABLED = 1,
        };

        enum CHANNEL
        {
            DAC_A = 0,
            DAC_B = 1,
            DAC_C = 2,
            DAC_D = 3,
        };  

        enum WRITE_COMMANDS
        {
            SEL_VREF = 0,
            SEL_WRITE_I2C_ADDRESS = 3,
            SEL_POWER_DOWN = 5,
            SEL_GAIN = 6,
        };

        union ByteField32
        {
            float valFloat;
            uint32_t valUint32;
            uint16_t valUint16;
            int16_t valSint16;
            uint8_t bytes[4];
        };
         
        // 00 ---------------------------------------------------------------------------//
        MCP4728();
        // 01 ---------------------------------------------------------------------------//
        MCP4728(uint8_t newValue);
        // 02 ---------------------------------------------------------------------------//
        MCP4728(uint8_t newValue, uint8_t newAlertPin);
        // 03 ---------------------------------------------------------------------------//
        ~MCP4728();
        // 04 ---------------------------------------------------------------------------//
        void EndWire();
        // 11 ---------------------------------------------------------------------------//
        void Set_DAC_CHNL_DAC(MCP4728::CHANNEL chnl, uint16_t newValue);
        // 13 ---------------------------------------------------------------------------//
        void Set_DAC_CHNL_GainSelect(MCP4728::CHANNEL chnl, MCP4728::GAIN_REF newValue);
        // 15 ---------------------------------------------------------------------------//
        void Set_DAC_CHNL_PowerDownSelect(MCP4728::CHANNEL chnl, MCP4728::PWR_DOWN_MODE newValue);
        // 17 ---------------------------------------------------------------------------//
        void Set_DAC_CHNL_VrefSelect(MCP4728::CHANNEL chnl, MCP4728::VREF_SOURCE newValue);
        // 19 ---------------------------------------------------------------------------//
        void Set_DAC_CHNL_UpDacEnbl(MCP4728::CHANNEL chnl, MCP4728::UPDATE_DAC newValue);
        
//-------------------------------------------------------------------------------//
    private:
        uint8_t _deviceAddress = 0x60; // 
        uint8_t _AlertPin = 0;
        
        ByteField32 _dacChnlReg[4] = {0,0,0,0}; 
        /*
        struct MCP4728CFGREG
        {
            uint16_t DACCount:12;       // Bits [0-11]
            uint8_t GainSelect:1;       // Bits [12]
            uint8_t PowerDownSelect:2;  // Bits [13-14]
            uint8_t VrefSelect:1;       // Bits [15]
            uint8_t DacChnl:2;          // Bits [16-17]
            uint8_t NotUsedbit18:1;     // Bits [18]
            uint8_t DeviceAddress:3;    // Bits [19-21]
            uint8_t RDY_BSY:1;          // Bits [22- ]
            uint8_t NotUsedbit22:1;     // Bits [0-11]
            uint8_t NotUsedbit23:1;     // Bits [0-11]
        };
        */

        struct MCP4728CFGREG_MULTI_WR_CMND
        {
            uint16_t DACCount:12;       // Bits [0-11]
            uint8_t GainSelect:1;       // Bits [12]
            uint8_t PowerDownSelect:2;  // Bits [13-14]
            uint8_t VrefSelect:1;       // Bits [15]
            uint8_t UpDacEnbl:1;        // Bits [16]
            uint8_t DacChnl:2;          // Bits [17-18]
            uint8_t WrComand:2;         // Bits [19-20]
            uint8_t Command:3;    // Bits [21-23]
        };

        union MULTI_WR_CMND
        {
            uint32_t register;
            uint8_t regBytes[4];
            MCP4728CFGREG_MULTI_WR_CMND fields;
        };

        MULTI_WR_CMND _dacCfg[4];

         
        // 10 ---------------------------------------------------------------------------//
        void Set_DAC_CHNL(MCP4728::MULTI_WR_CMND theDacCfg);

};
#endif

















