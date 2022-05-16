/*
    DATE: 04/24/2022
    TIME: 15:10
   TITLE: INA219 Driver (INA219 HAL)
  AUTHOR: Jerome B. Ford
    NOTE: Still under development. as of 4/26/2022
          Not yet tested!
*/
#include "Arduino.h"
#include "Wire.h"
#include "INA219v2.h"
 
//  0----------------------------------------------------------------------------//
INA219v2::INA219v2()
{
    InitzRegisterValues();
    Wire.begin(); // Join i2c bus as master.
}
//  1----------------------------------------------------------------------------//
INA219v2::INA219v2(SLAVE_ADDRESS_INA219 newValue)
{
    InitzRegisterValues();
    _deviceAddress = newValue;
    Wire.begin(); // Join i2c bus as master.
}
//  2----------------------------------------------------------------------------//
INA219v2::~INA219v2()
{
    EndWire();
} 
//  3----------------------------------------------------------------------------//
void INA219v2::InitzRegisterValues()
{
    _deviceAddress = GND_GND_0x40h;

    //_registersValues.Configuration = 0x399F;
    _registersValues.Configuration.Register = 0x399F;

    _registersValues.ShuntVoltage = 0;
    _registersValues.BusVoltage = 0;
    _registersValues.Power = 0;
    _registersValues.Current = 0;
    _registersValues.Calibration = 0;
    
    _registersValues.Rshunt = 0.10;
    _registersValues.Vshunt_mV = 0;
    _registersValues.Ishunt_mA = 0; 
    _registersValues.Pload_mW = 0;
}
//  4----------------------------------------------------------------------------//
void INA219v2::SetDeviceAddress(SLAVE_ADDRESS_INA219 newValue)
{
    _deviceAddress = newValue;
}
//  5----------------------------------------------------------------------------//
INA219v2::SLAVE_ADDRESS_INA219 INA219v2::GetDeviceAddress()
{
    return _deviceAddress;
}
//  6----------------------------------------------------------------------------//
void INA219v2::EndWire()
{
    Wire.endTransmission(); // Not sure if this is needed!
    Wire.end();
    TWCR &= ~0x04;          // <= NOT SURE ABOUT THIS!
}
//  7----------------------------------------------------------------------------//
void INA219v2::SetRegister(uint8_t reg, uint16_t newVal) 
{
    ByteField16 thevalues;                  // Create a local variable of type ByteField16, named thevalues.
    thevalues.valUint16 = newVal;           // Copy the newVal into the thevalues.
    Wire.beginTransmission(_deviceAddress); // Address the device.
    Wire.write(reg);                        // Index the device's register.
    Wire.write(thevalues.bytes[1]);         // write high byte of the 16bit value.
    Wire.write(thevalues.bytes[0]);         // write low byte of the 16bit value.
    Wire.endTransmission();                 // End I2C communication to the device.
} 
//  8----------------------------------------------------------------------------//
uint16_t INA219v2::GetRegister(uint8_t reg)   
{
    Wire.beginTransmission(_deviceAddress); // Start by Addressing the I2C device.
    Wire.write(reg);                        // Index the device's register.
    Wire.endTransmission();                 // End I2C
    ByteField16 thevalues;                  // Create a local variable of type ByteField16, named thevalues.
    Wire.requestFrom(_deviceAddress, 2);    // Read in 2byts.
    
    if(Wire.available() <= 2)               // Was the 2byts recieved? 
    {  
        thevalues.bytes[1] = Wire.read();   // Copy the high byte into the thevalues.
        thevalues.bytes[0] = Wire.read();   // Copy the low byte into the thevalues.
    }
    
    return thevalues.valUint16;             // Return the 16bits of the two bytes.
}  
//  9----------------------------------------------------------------------------//
void INA219v2::SetConfigurationReg0(uint16_t newVal)
{
    _registersValues.Configuration.Register = newVal;
    SetRegister(INA219_REGISTERS_ADD::Configuration, _registersValues.Configuration.Register);
}
// 10----------------------------------------------------------------------------//
uint16_t INA219v2::GetConfigurationReg0()
{
    _registersValues.Configuration.Register = GetRegister(INA219_REGISTERS_ADD::Configuration);
    return _registersValues.Configuration.Register;
}
// 11----------------------------------------------------------------------------//
uint16_t INA219v2::GetShuntVoltageReg1()
{
    _registersValues.ShuntVoltage = GetRegister(INA219_REGISTERS_ADD::ShuntVoltage);
    return _registersValues.ShuntVoltage;
}
// 12----------------------------------------------------------------------------//
uint16_t INA219v2::GetBusVoltageReg2()
{    
    _registersValues.BusVoltage = GetRegister(INA219_REGISTERS_ADD::BusVoltage);
    return _registersValues.BusVoltage;
}
// 13----------------------------------------------------------------------------//
uint16_t INA219v2::GetPowerReg3()
{    
    _registersValues.Power = GetRegister(INA219_REGISTERS_ADD::Power);
    return _registersValues.Power;
} 
// 14----------------------------------------------------------------------------//
uint16_t INA219v2::GetCurrentReg4()
{
    _registersValues.Current = GetRegister(INA219_REGISTERS_ADD::Current);
    return _registersValues.Current;
}
// 15----------------------------------------------------------------------------//
void INA219v2::SetCalibrationReg5(uint16_t newVal)
{    
    _registersValues.Calibration = newVal;
    SetRegister(INA219_REGISTERS_ADD::Calibration, _registersValues.Calibration);
}
// 16----------------------------------------------------------------------------//
uint16_t INA219v2::GetCalibrationReg5()
{
    /*
        8.5 Programming
        An important aspect of the INA219 device is that it measure current or power if it is programmed based on the
        system. The device measures both the differential voltage applied between the IN+ and IN- input pins and the
        voltage at IN- pin. In order for the device to report both current and power values, the user must program the
        resolution of the Current Register (04h) and the value of the shunt resistor (RSHUNT) present in the application to
        develop the differential voltage applied between the input pins. Both the Current_LSB and shunt resistor value
        are used in the calculation of the Calibration Register value that the device uses to calculate the corresponding
        current and power values based on the measured shunt and bus voltages.
        After programming the Calibration Register, the Current Register (04h) and Power Register (03h) update
        accordingly based on the corresponding shunt voltage and bus voltage measurements. Until the Calibration
        Register is programmed, the Current Register (04h) and Power Register (03h) remain at zero.
        8.5.1 Programming the Calibration Register
        The Calibration Register is calculated based on Equation 1. This equation includes the term Current_LSB, which
        is the programmed value for the LSB for the Current Register (04h). The user uses this value to convert the
        value in the Current Register (04h) to the actual current in amperes. The highest resolution for the Current
        Register (04h) can be obtained by using the smallest allowable Current_LSB based on the maximum expected
        current as shown in Equation 2. While this value yields the highest resolution, it is common to select a value for
        the Current_LSB to the nearest round number above this value to simplify the conversion of the Current Register
        (04h) and Power Register (03h) to amperes and watts respectively. The RSHUNT term is the value of the external
        shunt used to develop the differential voltage across the input pins. The Power Register (03h) is internally set to
        be 20 times the programmed Current_LSB see Equation 3.
        where
        • 0.04096 is an internal fixed value used to ensure scaling is maintained properly (1)
        (2)
        (3)
        Shunt voltage is calculated by multiplying the Shunt Voltage Register contents with the Shunt Voltage LSB of 10
        μV.
        The Bus Voltage register bits are not right-aligned. In order to compute the value of the Bus Voltage, Bus Voltage
        Register contents must be shifted right by three bits. This shift puts the BD0 bit in the LSB position so that the
        contents can be multiplied by the Bus Voltage LSB of 4-mV to compute the bus voltage measured by the device.
        After programming the Calibration Register, the value expected in the Current Register (04h) can be calculated
        by multiplying the Shunt Voltage register contents by the Calibration Register and then dividing by 4096 as
        shown in Equation 4. To obtain a value in amperes the Current register value is multiplied by the programmed
        Current_LSB.
        (4)
        The value expected in the Power register (03h) can be calculated by multiplying the Current register value by the
        Bus Voltage register value and then dividing by 5000 as shown in Equation 5. Power Register content is
        multiplied by Power LSB which is 20 times the Current_LSB for a power value in watts.
        (5)
        12 Submit Documentation Feedback Copyright © 2008–2015, Texas Instruments Incorporated
        Product Folder Links: INA219
        Corrected_Full_Scale_Cal = trunc
        Cal MeasShuntCurrent
        INA219_Current
        ´
        INA219
        www.ti.com SBOS448G –AUGUST 2008–REVISED DECEMBER 2015
        Programming (continued)
        8.5.2 Programming the Power Measurement Engine
        8.5.2.1 Calibration Register and Scaling
        The Calibration Register enables the user to scale the Current Register (04h) and Power Register (03h) to the
        most useful value for a given application. For example, set the Calibration Register such that the largest possible
        number is generated in the Current Register (04h) or Power Register (03h) at the expected full-scale point. This
        approach yields the highest resolution using the previously calculated minimum Current_LSB in the equation for
        the Calibration Register. The Calibration Register can also be selected to provide values in the Current Register
        (04h) and Power Register (03h) that either provide direct decimal equivalents of the values being measured, or
        yield a round LSB value for each corresponding register. After these choices have been made, the Calibration
        Register also offers possibilities for end user system-level calibration. After determining the exact current by
        using an external ammeter, the value of the Calibration Register can then be adjusted based on the measured
        current result of the INA219 to cancel the total system error as shown in Equation 6.
        (6)
        8.5.3 Simple Current Shunt Monitor Usage (No Programming Necessary)
        The INA219 can be used without any programming if it is only necessary to read a shunt voltage drop and bus
        voltage with the default 12-bit resolution, 320-mV shunt full-scale range (PGA = /8), 32-V bus full-scale range,
        and continuous conversion of shunt and bus voltage.
        Without programming, current is measured by reading the shunt voltage. The Current register and Power register
        are only available if the Calibration register contains a programmed value.
        8.5.4 Default Settings
        The default power-up states of the registers are shown in the Register Details section of this data sheet. These
        registers are volatile, and if programmed to other than default values, must be re-programmed at every device
        power-up. Detailed information on programming the Calibration register specifically is given in the section,
        Programming the Calibration Register.
        (1) 
    */
    _registersValues.Calibration = GetRegister(INA219_REGISTERS_ADD::Calibration );
    return _registersValues.Calibration;
}
// 17----------------------------------------------------------------------------//
void INA219v2::SetConfiguration_MODE(uint8_t newVal)
{
    /*
        MODE: Operating Mode
        Bits 0–2 Selects continuous, triggered, or power-down mode of operation. 
        These bits default to continuous shunt and bus measurement mode.
        The mode settings are shown in Table 6.
        
        MODE3   MODE2   MODE1   MODE
        0       0       0       Power-down
        0       0       1       Shunt voltage, triggered
        0       1       0       Bus voltage, triggered
        0       1       1       Shunt and bus, triggered
        1       0       0       ADC off (disabled)
        1       0       1       Shunt voltage, continuous
        1       1       0       Bus voltage, continuous
        1       1       1       VShunt and VBus, continuous
    
    */
    _registersValues.Configuration.fields.Mode = newVal;
    SetConfigurationReg0(_registersValues.Configuration.Register);
}
// 18----------------------------------------------------------------------------//
uint8_t INA219v2::GetConfiguration_MODE()
{
    GetConfigurationReg0();
    return _registersValues.Configuration.fields.Mode;
}
// 19----------------------------------------------------------------------------//
void INA219v2::SetConfiguration_SADC(uint8_t newVal)
{
    /*
        SADC: SADC Shunt ADC Resolution/Averaging
        Bits 3–6 These bits adjust the Shunt ADC resolution (9-, 10-, 11-, or 12-bit) or set the number of 
        samples used when averaging results for the Shunt Voltage Register (01h).
        BADC (Bus) and SADC (Shunt) ADC resolution/averaging and conversion time settings are shown in Table 5.
    */
    _registersValues.Configuration.fields.SADC = newVal;
    SetConfigurationReg0(_registersValues.Configuration.Register);
}   
// 20----------------------------------------------------------------------------//
uint8_t INA219v2::GetConfiguration_SADC()
{
    GetConfigurationReg0();
    return _registersValues.Configuration.fields.SADC;
}
// 21----------------------------------------------------------------------------//
void INA219v2::SetConfiguration_BADC(uint8_t newVal)
{
    /*
        BADC: BADC Bus ADC Resolution/Averaging
        Bits 7–10 These bits adjust the Bus ADC resolution (9-, 10-, 11-, or 12-bit) or set the number of 
        samples used when averaging results for the Bus Voltage Register (02h).
    */
    _registersValues.Configuration.fields.BADC = newVal;
    SetConfigurationReg0(_registersValues.Configuration.Register);
} 
// 22----------------------------------------------------------------------------//
uint8_t INA219v2::GetConfiguration_BADC()
{
    GetConfigurationReg0();
    return _registersValues.Configuration.fields.BADC;
}
// 23----------------------------------------------------------------------------//
void INA219v2::SetConfiguration_PGA(uint8_t newVal)
{
    /*
        PG: PGA (Shunt Voltage Only)
        Bits 11, 12 Sets PGA gain and range. Note that the PGA defaults to ÷8 (320mV range). Table 4 shows the gain and range for
        the various product gain settings.
        PG1 PG0 GAIN    Range
        0   0   1       ±40 mV
        0   1   2       ±80 mV
        1   0   4       ±160 mV
        1   1   8       ±320 mV
    */
    _registersValues.Configuration.fields.PGA = newVal;
    SetConfigurationReg0(_registersValues.Configuration.Register);

} 
// 24----------------------------------------------------------------------------//
uint8_t INA219v2::GetConfiguration_PGA()
{
    GetConfigurationReg0();
    return _registersValues.Configuration.fields.PGA;
}
// 25----------------------------------------------------------------------------//
void INA219v2::SetConfiguration_BRNG(uint8_t newVal)
{ 
    /*
        BRNG: Bus Voltage Range
        Bit 13 0 = 16V FSR
        1 = 32V FSR (default value)
    */
    _registersValues.Configuration.fields.BRNG = newVal;
    SetConfigurationReg0(_registersValues.Configuration.Register);
}
// 26----------------------------------------------------------------------------//
uint8_t INA219v2::GetConfiguration_BRNG()
{
    GetConfigurationReg0();
    return _registersValues.Configuration.fields.BRNG;
}
// 27----------------------------------------------------------------------------//
void INA219v2::SetConfiguration_RST(uint8_t newVal)
{ 
    /*
        RST: Reset Bit
        Bit 15 Setting this bit to '1' generates a system reset that is the same as power-on reset. Resets all registers to default
        values; this bit self-clears.
    */
    _registersValues.Configuration.fields.RST = newVal;
    SetConfigurationReg0(_registersValues.Configuration.Register);
}
// 28----------------------------------------------------------------------------//
uint8_t INA219v2::GetConfiguration_RST()
{   
    GetConfigurationReg0();
    return _registersValues.Configuration.fields.RST;
}
// 29----------------------------------------------------------------------------//
void INA219v2::SetRshunt(float newVal)
{
    // units = Ohms
    if(newVal > 0.0000005)
    {
        _registersValues.Rshunt = newVal;
    }
}
// 30----------------------------------------------------------------------------//
float INA219v2::GetRshunt()
{
    return _registersValues.Rshunt;
}
// 31----------------------------------------------------------------------------//
float INA219v2::Read_Shunt_mV()
{
    /*
        PG: PGA (Shunt Voltage Only)
        Bits 11, 12 Sets PGA gain and range. Note that the PGA defaults to ÷8 (320mV range). Table 4 shows the gain and range for
        the various product gain settings.
        
        i   PG1 PG0 GAIN    Range
        0   0   0   1       ±40 mV
        1   0   1   2       ±80 mV
        2   1   0   4       ±160 mV
        3   1   1   8       ±320 mV
    */
     
    uint16_t rawVolts = GetShuntVoltageReg1(); 
    
    if((_registersValues.ShuntVoltage & (1 << 15)) == 0)
    {
        _registersValues.Vshunt_mV = ((float)_registersValues.ShuntVoltage/100.0);
    }
    else
    {
        rawVolts = ~_registersValues.ShuntVoltage;// 2's Compl.
        rawVolts++;
        _registersValues.Vshunt_mV = -((float)rawVolts/100.0); 
    }
    return _registersValues.Vshunt_mV;
}
// 32----------------------------------------------------------------------------//
float INA219v2::Read_Vload()
{
    _registersValues.Vload = (GetBusVoltageReg2() >> 3) * 0.004;
    return _registersValues.Vload;
}
// 33----------------------------------------------------------------------------//
float INA219v2::CalculateLoadCurrent_mA()
{
    _registersValues.Ishunt_mA = (_registersValues.Vshunt_mV / _registersValues.Rshunt);
    return _registersValues.Ishunt_mA;
}
// 34----------------------------------------------------------------------------//
float INA219v2::CalculateLoadPower_mW()
{
    _registersValues.Pload_mW = (_registersValues.Ishunt_mA * _registersValues.Vload);
    return _registersValues.Pload_mW;
}
//  1000-------------------------------------------------------------------------//
String INA219v2::About()
{
    return "INA219 DC Power Monitor \n\tVersion: 1.2022.0424.i\n\tAuthor: Jerome B. Ford";
}
//  1001-------------------------------------------------------------------------//

