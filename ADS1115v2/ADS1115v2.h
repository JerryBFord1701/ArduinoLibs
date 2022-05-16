/*
    DATE: 01/04/2022
    TIME: 14:22
   TITLE: ADS1115 Driver (ADS1115 HAL)
  AUTHOR: Jerome B. Ford
    NOTE: Still under development. as of 1/14/2022

 * REF:
 * https://www.ti.com/lit/ds/symlink/ads1113.pdf?HQS=dis-dk-null-digikeymode-dsf-pf-null-wwe&ts=1641161543112&ref_url=https%253A%252F%252Fwww.ti.com%252Fgeneral%252Fdocs%252Fsuppproductinfo.tsp%253FdistId%253D10%2526gotoUrl%253Dhttps%253A%252F%252Fwww.ti.com%252Flit%252Fgpn%252Fads1113
 * 
  Adafruit's ADS1115
  ADS1115 16 Bit 860 Samples per Second Analog to Digital Converter (ADC) Evaluation Board
  https://www.digikey.com/en/products/detail/adafruit-industries-llc/1085/5761229
  https://www.adafruit.com/product/1085

  ADS115 DATA SHEET INFO:

  DEVICE ADDRESSING
    ADDR_GND =  0x48;   // 1001000  Default
    ADDR_VCC =  0x49;   // 1001001  
    ADDR_SDA =  0x4A;   // 1001010  
    ADDR_SCL =  0x4B;   // 1001011  
  
  Address Pointer Register
    BIT   NAME        DESCRIPTION
      7   ---         Reserved 0h Always write 0h
      6   ---         Reserved 0h Always write 0h
      5   ---         Reserved 0h Always write 0h
      4   ---         Reserved 0h Always write 0h
      3   ---         Reserved 0h Always write 0h
      2   ---         Reserved 0h Always write 0h
      1               00 : Conversion register  (16bits)
      0               01 : Config register      (16bits)
                      10 : Lo_thresh register   (16bits)
                      11 : Hi_thresh register   (16bits)
     
  //---------------------------------------------------------------------------------------------//
  Config Register (P[1:0] = 1h) [reset = 8583h]
    BIT   NAME        DESCRIPTION
     15   OS          Operational status or single-shot conversion start 
                      WR=H: Start a single conversion (when in power-down state)
                      WR=L: No effect
                      RD=L: Device is currently performing a conversion
                      RD=H: Device is not currently performing a conversion
  
     14   MUX[2]      Input multiplexer[0-2] configuration (ADS1115 only)
     13   MUX[1]      000 : AINP = AIN0 and AINN = AIN1 (default)
     12   MUX[0]      001 : AINP = AIN0 and AINN = AIN3
                      010 : AINP = AIN1 and AINN = AIN3
                      011 : AINP = AIN2 and AINN = AIN3
                      100 : AINP = AIN0 and AINN = GND
                      101 : AINP = AIN1 and AINN = GND
                      110 : AINP = AIN2 and AINN = GND
                      111 : AINP = AIN3 and AINN = GND
       
     11   PGA[2]      Programmable gain amplifier configuration
     10   PGA[1]      000 : FSR = ±6.144 V (1)
      9   PGA[0]      001 : FSR = ±4.096 V (1)
                      010 : FSR = ±2.048 V (default)
                      011 : FSR = ±1.024 V
                      100 : FSR = ±0.512 V
                      101 : FSR = ±0.256 V
                      110 : FSR = ±0.256 V
                      111 : FSR = ±0.256 V
        
      
      8   MODE        Device operating mode
                      WR=L : Continuous-conversion mode
                      WR=H : Single-shot mode or power-down state (default)
                    
      7   DR[2]       Data rate
      6   DR[1]       000 : 8 SPS
      5   DR[0]       001 : 16 SPS
                      010 : 32 SPS
                      011 : 64 SPS
                      100 : 128 SPS (default)
                      101 : 250 SPS
                      110 : 475 SPS
                      111 : 860 SPS
       
      4   COMP_MODE   Comparator mode (ADS1114 and ADS1115 only)
                      0 : Traditional comparator (default)
                      1 : Window comparator
                      
      3   COMP_POL    Comparator polarity (ADS1114 and ADS1115 only)
                      0 : Active low (default)
                      1 : Active high
  
      2   COMP_LAT    Latching comparator (ADS1114 and ADS1115 only)
                      0 : Nonlatching comparator . The ALERT/RDY pin does not latch when asserted (default).
                      1 : Latching comparator. The asserted ALERT/RDY pin remains latched until
                          conversion data are read by the master or an appropriate SMBus alert response
                          is sent by the master. The device responds with its address, and it is the lowest
                          address currently asserting the ALERT/RDY bus line.
                          
      1   COMP_QUE[1] Comparator queue and disable (ADS1114 and ADS1115 only)
      0   COMP_QUE[0] 00 : Assert after one conversion
                      01 : Assert after two conversions
                      10 : Assert after four conversions
                      11 : Disable comparator and set ALERT/RDY pin to high-impedance (default)
  
*/
 
#ifndef ADS1115v2_h
#define ADS1115v2_h

#include "Wire.h"
#include "Arduino.h"

class ADS1115v2
{
  public:
  
    enum DEVICE_ADDRESS
    {
        ADDR_GND = 0x48,  // <= Default!
        ADDR_VCC = 0x49, 
        ADDR_SDA = 0x4A, 
        ADDR_SCL = 0x4B
    };

    enum PGA
    {
       FSR0_6144mV = 0,
       FSR1_4096mV = 1,
       FSR2_2048mV = 2, // <= Default!
       FSR3_1024mV = 3,
       FSR4_0512mV = 4,
       FSR5_0256mV = 5,
       FSR6_0256mV = 6,
       FSR7_0256mV = 7,
    };

    enum MUX
    {
       CHNL0_P0_N1 = 0, // <= Default!
       CHNL1_P0_N3 = 1,
       CHNL2_P1_N3 = 2,
       CHNL3_P2_N3 = 3,
       CHNL4_SE0 = 4,
       CHNL5_SE1 = 5,
       CHNL6_SE2 = 6,
       CHNL7_SE3 = 7,
    };

    enum RATE
    {
        SPS0_8Hz = 0,
        SPS1_16Hz = 1,
        SPS2_32Hz = 2,
        SPS3_64Hz = 3,
        SPS4_128Hz = 4, // <= Default!
        SPS5_250Hz = 5,
        SPS6_475Hz = 6,
        SPS7_860Hz = 7,
    };

    const uint8_t RegAddress_Conversion   = 0;
    const uint8_t RegAddress_Config       = 1;
    const uint8_t RegAddress_CompLoThresh = 2;
    const uint8_t RegAddress_CompHiThresh = 3;
    
    //0------------------------------------------------------------------------------//
    ADS1115v2();
    //1------------------------------------------------------------------------------//
    ADS1115v2(DEVICE_ADDRESS newValue);
    //-------------------------------------------------------------------------------//
    ADS1115v2(DEVICE_ADDRESS newValue, uint8_t newAlertPin);
    //2------------------------------------------------------------------------------//
    ~ADS1115v2();
    //3------------------------------------------------------------------------------//
    void SetDeviceAddress(DEVICE_ADDRESS newValue);
    //4------------------------------------------------------------------------------//
    DEVICE_ADDRESS GetDeviceAddress();
    //5------------------------------------------------------------------------------//
    void SetConfig_OS(uint8_t newValue, bool updateNow);
    //6------------------------------------------------------------------------------//
    uint8_t GetConfig_OS(bool updateNow);
    //-------------------------------------------------------------------------------//
    void SetConfig_Mux(uint8_t newValue, bool updateNow);
    //-------------------------------------------------------------------------------//
    uint8_t GetConfig_Mux(bool updateNow);
    //-------------------------------------------------------------------------------//
    void SetConfig_SingleEndedMux(uint8_t chnl, bool updateNow);
    //-------------------------------------------------------------------------------//
    uint8_t GetConfig_SingleEndedMux(bool updateNow);
    //-------------------------------------------------------------------------------//
    void SetConfig_DifferentialMux(uint8_t newValue, bool updateNow);
    //-------------------------------------------------------------------------------//
    uint8_t GetConfig_DifferentialMux(bool updateNow);
    //-------------------------------------------------------------------------------//
    void SetConfig_PGA(uint8_t newValue, bool updateNow);
    //-------------------------------------------------------------------------------//
    uint8_t GetConfig_PGA(bool updateNow);
    //-------------------------------------------------------------------------------//
    void SetConfig_Mode(uint8_t newValue, bool updateNow);
    //-------------------------------------------------------------------------------//
    uint8_t GetConfig_Mode(bool updateNow);
    //-------------------------------------------------------------------------------//
    void SetConfig_Rate(uint8_t newValue, bool updateNow);
    //-------------------------------------------------------------------------------//
    uint8_t GetConfig_Rate(bool updateNow);
    //-------------------------------------------------------------------------------//
    void SetConfig_COMP_MODE(uint8_t newValue, bool updateNow);
    //-------------------------------------------------------------------------------//
    uint8_t GetConfig_COMP_MODE(bool updateNow);
    //-------------------------------------------------------------------------------//
    void SetConfig_COMP_POL(uint8_t newValue, bool updateNow);
    //-------------------------------------------------------------------------------//
    uint8_t GetConfig_COMP_POL(bool updateNow);
    //-------------------------------------------------------------------------------//
    void SetConfig_COMP_LAT(uint8_t newValue, bool updateNow);
    //-------------------------------------------------------------------------------//
    uint8_t GetConfig_COMP_LAT(bool updateNow);
    //-------------------------------------------------------------------------------//
    void SetConfig_COMP_QUE(uint8_t newValue, bool updateNow);
    //-------------------------------------------------------------------------------//
    uint8_t GetConfig_COMP_QUE(bool updateNow);
    //-------------------------------------------------------------------------------//
    uint16_t ReadConversionReg();
    //-------------------------------------------------------------------------------//
    float ReadConversionRegFloat();
    //-------------------------------------------------------------------------------//
    float ReadConversionScalled();
    //-------------------------------------------------------------------------------//
    void WriteConfigReg(uint16_t newRegValue);
    //-------------------------------------------------------------------------------//
    uint16_t ReadConfigReg();
    //-------------------------------------------------------------------------------//
    void WriteCompLoThreshReg(uint16_t newRegValue);
    //-------------------------------------------------------------------------------//
    uint16_t ReadCompLoThreshReg();
    //-------------------------------------------------------------------------------//
    void WriteCompHiThreshReg(uint16_t newRegValue);
    //-------------------------------------------------------------------------------//
    uint16_t ReadCompHiThreshReg();
    //-------------------------------------------------------------------------------//
    void SetRegister(uint8_t reg, uint16_t newVal);
    //-------------------------------------------------------------------------------//
    uint16_t GetRegister(uint8_t reg); 
    //-------------------------------------------------------------------------------//
    void SetCalSlope(uint8_t chnl, float newSlope);
    //-------------------------------------------------------------------------------//
    float GetCalSlope(uint8_t chnl);
    //-------------------------------------------------------------------------------//
    void SetCalOffset(uint8_t chnl, float newOffset);
    //-------------------------------------------------------------------------------//
    float GetCalOffset(uint8_t chnl);
    //-------------------------------------------------------------------------------//
    void SetChannelPGA(uint8_t chnl, uint8_t newPGA);
    //-------------------------------------------------------------------------------//
    uint8_t GetChannelPGA(uint8_t chnl);
    //-------------------------------------------------------------------------------//
    void SetChannelRate(uint8_t chnl, uint8_t newRate);
    //-------------------------------------------------------------------------------//
    uint8_t GetChannelRate(uint8_t chnl);
    //-------------------------------------------------------------------------------//
    void EndWire();
    //-------------------------------------------------------------------------------//
    float Convert16bit2sComplToFloat(uint16_t regValue, float ABS_FSR);
    //-------------------------------------------------------------------------------//
    uint16_t ConvertFloatTo2sCompl16Bit(float inpVal, float ABS_FSR);
    //-------------------------------------------------------------------------------//
    void EnableConversionReady();
    //-------------------------------------------------------------------------------//
    void DisableConversionReady();
    //-------------------------------------------------------------------------------//
  private:
    
    union ByteField16
    {
      float valFloat;
      uint16_t valUint16;
      int16_t valSint16;
      uint8_t bytes[4];
    };
    

    struct ConfigRegBitsFields
    {
      uint8_t COMP_QUE:2;   // bits 0, 1    
      uint8_t COMP_LAT:1;   // bit  2
      uint8_t COMP_POL:1;   // bit  3
      uint8_t COMP_MODE:1;  // bit  4
      uint8_t DataRate:3;   // bit  5, 6, 7
      uint8_t Mode:1;       // bit  8
      uint8_t PGA:3;        // bits 9, 10, 11
      uint8_t MUX:3;        // bits 12, 13, 14
      uint8_t OpStatus:1;   // bit  15
    };
    
    DEVICE_ADDRESS _deviceAddress = ADDR_GND;
    uint16_t _RegValue_Conversion = 0;
    //uint16_t _RegValue_Config = 0;

    union CONFIG_REG 
    {
      uint16_t Register;
      ConfigRegBitsFields Fields;
    }
    CONFIG_REG _RegValue_Config;
    
    uint16_t _RegValue_CompLoThresh = 0;
    uint16_t _RegValue_CompHiThresh = 0;
    uint8_t _AlertPin = 0;

    float       _fsr[8] = {6.144f, 4.096f, 2.048f, 1.024f, 0.512f, 0.256f, 0.256f, 0.256f}; // Full scale range. Selected by PGA.
    float  _calSlope[8] = {1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f}; // Slopes for channel. Selected by MUX.
    float _calOffset[8] = {0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f, 0.000f}; // Offset for channel. Selected by MUX.
    uint8_t    _gain[8] = {     2,      2,      2,      2,      2,      2,      2,      2}; // Channel Gain.
    uint8_t    _rate[8] = {     4,      4,      4,      4,      4,      4,      4,      4}; // Channel Rate.
     
};
#endif