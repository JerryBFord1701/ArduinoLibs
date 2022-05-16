/*
    DATE: 01/04/2022
    TIME: 14:22
   TITLE: ADS1115v2 Driver (ADS1115v2 HAL)
  AUTHOR: Jerome B. Ford
    NOTE: Still under development. as of 1/14/2022
*/
#include "Arduino.h"
#include "ADS1115v2.h"
//-------------------------------------------------------------------------------//
ADS1115v2::ADS1115v2()
{
    Wire.begin(); // Join i2c bus as master.
}
//-------------------------------------------------------------------------------//
ADS1115v2::ADS1115v2(DEVICE_ADDRESS newValue)
{
    _deviceAddress = newValue;
    Wire.begin(); // Join i2c bus as master.
}
//-------------------------------------------------------------------------------//
ADS1115v2::ADS1115v2(DEVICE_ADDRESS newValue, uint8_t newAlertPin)
{
    _deviceAddress = newValue;
    _AlertPin = newAlertPin;
    pinMode(_AlertPin, INPUT);
    Wire.begin(); // Join i2c bus as master.
}
//-------------------------------------------------------------------------------//
ADS1115v2::~ADS1115v2()
{
    EndWire();
}
//-------------------------------------------------------------------------------//
void ADS1115v2::SetDeviceAddress(DEVICE_ADDRESS newValue)
{
    _deviceAddress = newValue;
}
//-------------------------------------------------------------------------------//
ADS1115v2::DEVICE_ADDRESS ADS1115v2::GetDeviceAddress()
{
    return _deviceAddress;
}
//-------------------------------------------------------------------------------//
void ADS1115v2::SetConfig_OS(uint8_t newValue, bool updateNow)
{
    /*
    15   OS       Operational status or single-shot conversion start 
                    WR=H: Start a single conversion (when in power-down state)
                    WR=L: No effect
                    RD=L: Device is currently performing a conversion
                    RD=H: Device is not currently performing a conversion
    */

    _RegValue_Config.Fields.OpStatus = newValue;

    if(updateNow == true)
    {
        WriteConfigReg(_RegValue_Config.Register);
    }
}
//-------------------------------------------------------------------------------//
uint8_t ADS1115v2::GetConfig_OS(bool updateNow)
{
    if(updateNow == true)
    {
        ReadConfigReg();
    }
     
    return _RegValue_Config.Fields.OpStatus;
}
//-------------------------------------------------------------------------------//
void ADS1115v2::SetConfig_Mux(uint8_t newValue, bool updateNow)
{
    /*
    Input multiplexer[0-2] configuration (ADS1115v2 only)
    14   MUX[2] 
    13   MUX[1] 
    12   MUX[0]

        000 : AINP = AIN0 and AINN = AIN1, Differential, (default)
        001 : AINP = AIN0 and AINN = AIN3, Differential       
        010 : AINP = AIN1 and AINN = AIN3, Differential
        011 : AINP = AIN2 and AINN = AIN3, Differential
        100 : AINP = AIN0 and AINN =  GND, Single Ended
        101 : AINP = AIN1 and AINN =  GND, Single Ended
        110 : AINP = AIN2 and AINN =  GND, Single Ended
        111 : AINP = AIN3 and AINN =  GND, Single Ended
    */
    //1111110000000000
    //5432109876543210
    //0000000000000111 =     7dec, 0x0007
    //0111000000000000 = 28672dec, 0x7000
    //1000111111111111 = 36863dec, 0x8FFF
    
    _RegValue_Config.Fields.MUX = newValue;

    if(updateNow == true)
    {
        WriteConfigReg(_RegValue_Config.Register);
    }
}
//-------------------------------------------------------------------------------//
uint8_t ADS1115v2::GetConfig_Mux(bool updateNow)
{
    if(updateNow == true)
    {
        ReadConfigReg();
    }
     
    return _RegValue_Config.Fields.MUX;
}
//-------------------------------------------------------------------------------//
void ADS1115v2::SetConfig_SingleEndedMux(uint8_t chnl, bool updateNow)
{
    chnl &= 3;
    chnl |= 4;
    SetConfig_PGA(_gain[chnl], false);
    SetConfig_Rate(_rate[chnl], false); // <= As per Chuck G's suggestion. 1/8/2022 JBF
    SetConfig_Mux(chnl, updateNow);
}
//-------------------------------------------------------------------------------//
uint8_t ADS1115v2::GetConfig_SingleEndedMux(bool updateNow)
{
    return GetConfig_Mux(updateNow) & 3;
}
//-------------------------------------------------------------------------------//
void ADS1115v2::SetConfig_DifferentialMux(uint8_t newValue, bool updateNow)
{
    newValue &= 3;
    SetConfig_Mux(newValue, updateNow);
}
//-------------------------------------------------------------------------------//
uint8_t ADS1115v2::GetConfig_DifferentialMux(bool updateNow)
{
    return GetConfig_Mux(updateNow) & 3;
}
//-------------------------------------------------------------------------------//
void ADS1115v2::SetConfig_PGA(uint8_t newValue, bool updateNow)
{
    /*  
        11  PGA[2]  Programmable gain amplifier configuration
        10  PGA[1]  000 : FSR = ±6.144 V (1)
        9   PGA[0]  001 : FSR = ±4.096 V (1)
                    010 : FSR = ±2.048 V (default)
                    011 : FSR = ±1.024 V
                    100 : FSR = ±0.512 V
                    101 : FSR = ±0.256 V
                    110 : FSR = ±0.256 V
                    111 : FSR = ±0.256 V

        1111110000000000
        5432109876543210
        0000000000000111 =     7dec, 0x0007
        0000111000000000 =  3584dec, 0x0E00  = (7<<9)
        1111000111111111 = 61951dec, 0xF1FF       
    
    */  

    _RegValue_Config.Fields.PGA = newValue;
    
    if(updateNow == true)
    {
        WriteConfigReg(_RegValue_Config.Register);
    }
}
//-------------------------------------------------------------------------------//
uint8_t ADS1115v2::GetConfig_PGA(bool updateNow)
{
    if(updateNow == true)
    {
        ReadConfigReg();
    }
    
    return _RegValue_Config.Fields.PGA;
}
//-------------------------------------------------------------------------------//
void ADS1115v2::SetConfig_Mode(uint8_t newValue, bool updateNow)
{
    /*
        bit 8   MODE    Device operating mode
                        WR=L : Continuous-conversion mode
                        WR=H : Single-shot mode or power-down state (default)
    */
    
    _RegValue_Config.Fields.Mode = newValue;

    if(updateNow == true)
    {
        WriteConfigReg(_RegValue_Config.Register);
    }
}
//-------------------------------------------------------------------------------//
uint8_t ADS1115v2::GetConfig_Mode(bool updateNow)
{
    if(updateNow == true)
    {
        ReadConfigReg();
    }
    
    return _RegValue_Config.Fields.Mode;
}
//-------------------------------------------------------------------------------//
void ADS1115v2::SetConfig_Rate(uint8_t newValue, bool updateNow)
{
    /*
    Data rate
    Bit 7   DR[2]   
    Bit 6   DR[1]       
    Bit 5   DR[0] 

    000 :   8 SPS      
    001 :  16 SPS
    010 :  32 SPS
    011 :  64 SPS
    100 : 128 SPS (default)
    101 : 250 SPS
    110 : 475 SPS
    111 : 860 SPS

    1111110000000000
    5432109876543210
    0000000000000111 =     7dec, 0x0007
    0000000011100000 =   224dec, 0x00e0
    1111111100011111 = 65311dec, 0xF1FF
    */

    _RegValue_Config.Fields.DataRate = newValue;

    if(updateNow == true)
    {
        WriteConfigReg(_RegValue_Config.Register);
    }
}
//-------------------------------------------------------------------------------//
uint8_t ADS1115v2::GetConfig_Rate(bool updateNow)
{
    if(updateNow == true)
    {
        ReadConfigReg();
    }
    
    return _RegValue_Config.Fields.DataRate;
}
//-------------------------------------------------------------------------------//
void ADS1115v2::SetConfig_COMP_MODE(uint8_t newValue, bool updateNow)
{
    /*
    Bit 4   COMP_MODE   Comparator mode (ADS1114 and ADS1115v2 only)
                    0 : Traditional comparator (default)
                    1 : Window comparator
    */
    
    _RegValue_Config.Fields.COMP_MODE = newValue;
    
    if(updateNow == true)
    {
        WriteConfigReg(_RegValue_Config.Register);
    }
}
//-------------------------------------------------------------------------------//
uint8_t ADS1115v2::GetConfig_COMP_MODE(bool updateNow)
{
    if(updateNow == true)
    {
        ReadConfigReg();
    }
    
    return _RegValue_Config.Fields.COMP_MODE;
}
//-------------------------------------------------------------------------------//
void ADS1115v2::SetConfig_COMP_POL(uint8_t newValue, bool updateNow)
{
    /*
        Bit 3   COMP_POL    Comparator polarity (ADS1114 and ADS1115v2 only)
                            0 : Active low (default)
                            1 : Active high
    */
    
    _RegValue_Config.Fields.COMP_POL = newValue;

    if(updateNow == true)
    {
        WriteConfigReg(_RegValue_Config.Register);
    }
}
//-------------------------------------------------------------------------------//
uint8_t ADS1115v2::GetConfig_COMP_POL(bool updateNow)
{
    if(updateNow == true)
    {
        ReadConfigReg();
    }
    
    return _RegValue_Config.Fields.COMP_POL;
}
//-------------------------------------------------------------------------------//
void ADS1115v2::SetConfig_COMP_LAT(uint8_t newValue, bool updateNow)
{
    
    /*
    Bit 2   COMP_LAT    Latching comparator (ADS1114 and ADS1115v2 only)
                        0 : Nonlatching comparator . The ALERT/RDY pin does not latch when asserted (default).
                        1 : Latching comparator. The asserted ALERT/RDY pin remains latched until
                            conversion data are read by the master or an appropriate SMBus alert response
                            is sent by the master. The device responds with its address, and it is the lowest
                            address currently asserting the ALERT/RDY bus line.
    */
    
    _RegValue_Config.Fields.COMP_LAT = newValue;
    
    if(updateNow == true)
    {
        WriteConfigReg(_RegValue_Config.Register);
    }
}
//-------------------------------------------------------------------------------//
uint8_t ADS1115v2::GetConfig_COMP_LAT(bool updateNow)
{
    if(updateNow == true)
    {
        ReadConfigReg();
    }
    
    return _RegValue_Config.Fields.COMP_LAT;
}
//-------------------------------------------------------------------------------//
void ADS1115v2::SetConfig_COMP_QUE(uint8_t newValue, bool updateNow)
{
    /*
    bit
        1     
        0     
        COMP_QUE[1] Comparator queue and disable (ADS1114 and ADS1115v2 only)
        COMP_QUE[0] 00 : Assert after one conversion
                    01 : Assert after two conversions
                    10 : Assert after four conversions
                    11 : Disable comparator and set ALERT/RDY pin to high-impedance (default)
    */
    
    _RegValue_Config.Fields.COMP_QUE = newValue;

    if(updateNow == true)
    {
        WriteConfigReg(_RegValue_Config.Register);
    }
    
}
//-------------------------------------------------------------------------------//
uint8_t ADS1115v2::GetConfig_COMP_QUE(bool updateNow)
{
    if(updateNow == true)
    {
        ReadConfigReg();
    }
    
    return _RegValue_Config.Fields.COMP_QUE;
}
//-------------------------------------------------------------------------------//
uint16_t ADS1115v2::ReadConversionReg()
{
    _RegValue_Conversion = GetRegister(RegAddress_Conversion);
    return _RegValue_Conversion;
}
//-------------------------------------------------------------------------------//
float ADS1115v2::ReadConversionRegFloat()
{
    uint8_t pga_val = GetConfig_PGA(false); // <= returns [0 - 7]
    _RegValue_Conversion = GetRegister(RegAddress_Conversion);
    return Convert16bit2sComplToFloat(_RegValue_Conversion, _fsr[pga_val]);
}
//-------------------------------------------------------------------------------//
float ADS1115v2::ReadConversionScalled()
{
    uint8_t mux_val = GetConfig_Mux(false); // <= returns [0 - 7]
    float Vchnl = ReadConversionRegFloat();
    Vchnl *= _calSlope[mux_val];
    Vchnl += _calOffset[mux_val];
    return Vchnl;
}
//-------------------------------------------------------------------------------//
void ADS1115v2::WriteConfigReg(uint16_t newRegValue)
{
    _RegValue_Config.Register = newRegValue;
    SetRegister(RegAddress_Config, _RegValue_Config.Register);
}
//-------------------------------------------------------------------------------//
uint16_t ADS1115v2::ReadConfigReg()
{
    _RegValue_Config.Register = GetRegister(RegAddress_Config);
    return _RegValue_Config.Register;
}
//-------------------------------------------------------------------------------//
void ADS1115v2::WriteCompLoThreshReg(uint16_t newRegValue)
{
    _RegValue_CompLoThresh = newRegValue;
    SetRegister(RegAddress_CompLoThresh, _RegValue_CompLoThresh);
}
//-------------------------------------------------------------------------------//
uint16_t ADS1115v2::ReadCompLoThreshReg()
{
    _RegValue_CompLoThresh = GetRegister(RegAddress_CompLoThresh);
    return _RegValue_CompLoThresh;
}
//-------------------------------------------------------------------------------//
void ADS1115v2::WriteCompHiThreshReg(uint16_t newRegValue)
{
    /*
    9.6.4 Lo_thresh (P[1:0] = 2h) [reset = 8000h] and Hi_thresh (P[1:0] = 3h) [reset = 7FFFh] Registers
    The upper and lower threshold values used by the comparator are stored in two 16-bit registers in two's
    complement format. The comparator is implemented as a digital comparator; therefore, the values in these
    registers must be updated whenever the PGA settings are changed.
    The conversion-ready function of the ALERT/RDY pin is enabled by setting the Hi_thresh register MSB to 1 and
    the Lo_thresh register MSB to 0. To use the comparator function of the ALERT/RDY pin, the Hi_thresh register
    value must always be greater than the Lo_thresh register value. The threshold register formats are shown in
    Figure 37. When set to RDY mode, the ALERT/RDY pin outputs the OS bit when in single-shot mode, and
    provides a continuous-conversion ready pulse when in continuous-conversion mode.
    */
    
    _RegValue_CompHiThresh = newRegValue;
    SetRegister(RegAddress_CompHiThresh, _RegValue_CompHiThresh);
}
//-------------------------------------------------------------------------------//
uint16_t ADS1115v2::ReadCompHiThreshReg()
{
    _RegValue_CompHiThresh = GetRegister(RegAddress_CompHiThresh);
    return _RegValue_CompHiThresh;
}
//-------------------------------------------------------------------------------//
void ADS1115v2::SetRegister(uint8_t reg, uint16_t newVal) 
{
    ByteField16 thevalues;
    thevalues.valUint16 = newVal;
    Wire.beginTransmission(_deviceAddress); 
    Wire.write(reg);
    Wire.write(thevalues.bytes[1]);
    Wire.write(thevalues.bytes[0]);
    Wire.endTransmission();
} 
//-------------------------------------------------------------------------------//
uint16_t ADS1115v2::GetRegister(uint8_t reg)   
{
    Wire.beginTransmission(_deviceAddress); 
    Wire.write(reg); 
    Wire.endTransmission(); 
    ByteField16 thevalues;
    Wire.requestFrom(_deviceAddress, 2); 
    
    if(Wire.available() <= 2)
    {  
        thevalues.bytes[1] = Wire.read(); 
        thevalues.bytes[0] = Wire.read();
    }
    
    return thevalues.valUint16;
}   
//-------------------------------------------------------------------------------//
void ADS1115v2::SetCalSlope(uint8_t chnl, float newSlope)
{
    chnl &= 7;
    _calSlope[chnl] = newSlope;
}
//-------------------------------------------------------------------------------//
float ADS1115v2::GetCalSlope(uint8_t chnl)
{
    chnl &= 7;
    return _calSlope[chnl];
}
//-------------------------------------------------------------------------------//
void ADS1115v2::SetCalOffset(uint8_t chnl, float newOffset)
{
    chnl &= 7;
    _calOffset[chnl] = newOffset;
}
//-------------------------------------------------------------------------------//
float ADS1115v2::GetCalOffset(uint8_t chnl)
{
    chnl &= 7;
    return _calOffset[chnl];
}
//-------------------------------------------------------------------------------//
void ADS1115v2::SetChannelPGA(uint8_t chnl, uint8_t newPGA)
{
    /*
    Programmable gain amplifier configuration
    11  Config.PGA[2]  
    10  Config.PGA[1]  
        9  Config.PGA[0]  
                    
    000 : FSR = ±6.144 V (1)
    001 : FSR = ±4.096 V (1)
    010 : FSR = ±2.048 V (default)
    011 : FSR = ±1.024 V
    100 : FSR = ±0.512 V
    101 : FSR = ±0.256 V
    110 : FSR = ±0.256 V
    111 : FSR = ±0.256 V
    */

    chnl &= 7;  // NOTE:  chnl = [0-3] are the Differential mux channels.
                //        chnl = [4-7] are the Signle-Ended mux channels.
    newPGA &= 7;

    _gain[chnl] = newPGA;
    //Serial.println("SetChannelPGA[" + String(chnl, DEC) + "] = " + String(newPGA, DEC) );// <= For Debug Only!
}
//-------------------------------------------------------------------------------//
uint8_t ADS1115v2::GetChannelPGA(uint8_t chnl)
{
    chnl &= 7;
    return _gain[chnl];
}
//-------------------------------------------------------------------------------//
void ADS1115v2::SetChannelRate(uint8_t chnl, uint8_t newRate)
{
    /*
    Data rate
    Bit 7   DR[2]   
    Bit 6   DR[1]       
    Bit 5   DR[0] 
        000 :   8 SPS      
        001 :  16 SPS
        010 :  32 SPS
        011 :  64 SPS
        100 : 128 SPS (default)
        101 : 250 SPS
        110 : 475 SPS
        111 : 860 SPS

    1111110000000000
    5432109876543210
    0000000000000111 =     7dec, 0x0007
    0000000011100000 =   224dec, 0x00e0
    1111111100011111 = 65311dec, 0xF1FF
    */
    chnl &= 7;// NOTE:  chnl = [0-3] are the Differential mux channels.
            //        chnl = [4-7] are the Signle-Ended mux channels.
    _rate[chnl] = (newRate & 7); 
}
//-------------------------------------------------------------------------------//
uint8_t ADS1115v2::GetChannelRate(uint8_t chnl)
{
    chnl &= 7;
    return _rate[chnl];
}
//-------------------------------------------------------------------------------//
void ADS1115v2::EndWire()
{
    Wire.endTransmission(); // Not sure if this is needed!
    Wire.end();
    TWCR &= ~0x04;          // <= NOT SURE ABOUT THIS!
}
//-------------------------------------------------------------------------------//
float ADS1115v2::Convert16bit2sComplToFloat(uint16_t regValue, float ABS_FSR)
{
    float volts = 0.0f;
    uint16_t adcVolts = 0;

    if(regValue >= 32768)
    {
        adcVolts = (~regValue); 
        adcVolts++;
        volts = -ABS_FSR;
        volts *= (float)adcVolts / 32768.0f;
    }
    else
    {
        adcVolts = regValue;
        volts = ABS_FSR; 
        volts *= (float)adcVolts / 32768.0f;
    }
    return volts;
}
//-------------------------------------------------------------------------------//
uint16_t ADS1115v2::ConvertFloatTo2sCompl16Bit(float inpVal, float ABS_FSR)
{
    uint16_t regValue = 0;
    float tempFloat = 0;

    if(inpVal < 0.0f)
    {
        tempFloat = 32768.0f;
        tempFloat *=  (-inpVal/ABS_FSR);
        regValue = (uint16_t)tempFloat;
        regValue = ~regValue;
        regValue++;
    }
    else
    {
        tempFloat = 32768.0f;
        tempFloat *= (inpVal/ABS_FSR);
        regValue = (uint16_t)round(tempFloat);
    }
    return regValue;
}
//-------------------------------------------------------------------------------//
void ADS1115v2::EnableConversionReady()
{
    
    /*
    The conversion-ready function of the ALERT/RDY pin is enabled by setting the Hi_thresh register MSB to 1 and
    the Lo_thresh register MSB to 0. To use the comparator function of the ALERT/RDY pin, the Hi_thresh register
    value must always be greater than the Lo_thresh register value. The threshold register formats are shown in
    Figure 37. 
    When set to RDY mode, the ALERT/RDY pin outputs the OS bit when in single-shot mode, and
    provides a continuous-conversion ready pulse when in continuous-conversion mode.

    Congiguration Reg Bit3: COMP_POL R/W 0h
    Comparator polarity (ADS1114 and ADS1115 only)
    This bit controls the polarity of the ALERT/RDY pin. This bit serves no function on
    the ADS1113.
    0 : Active low (default)
    1 : Active high

    Congiguration Reg 1:0 COMP_QUE[1:0] R/W 3h
    Comparator queue and disable (ADS1114 and ADS1115 only)
    These bits perform two functions. When set to 11, the comparator is disabled and
    the ALERT/RDY pin is set to a high-impedance state. When set to any other
    value, the ALERT/RDY pin and the comparator function are enabled, and the set
    value determines the number of successive conversions exceeding the upper or
    lower threshold required before asserting the ALERT/RDY pin. These bits serve
    no function on the ADS1113.
    00 : Assert after one conversion
    01 : Assert after two conversions
    10 : Assert after four conversions
    11 : Disable comparator and set ALERT/RDY pin to high-impedance (default)

    */
    // set ALERT PIN to signal Conversion Ready.
    WriteCompHiThreshReg(_RegValue_CompHiThresh | 0x8000);
    WriteCompLoThreshReg(_RegValue_CompLoThresh & 0x7FFF);
    SetConfig_COMP_POL(1, false);
    SetConfig_COMP_QUE(0, true);
}
//-------------------------------------------------------------------------------//
void ADS1115v2::DisableConversionReady()
{
    WriteCompHiThreshReg(_RegValue_CompHiThresh & 0x7FFF);
    WriteCompLoThreshReg(_RegValue_CompLoThresh | 0x8000);
    SetConfig_COMP_QUE(3, true);
}
//-------------------------------------------------------------------------------//