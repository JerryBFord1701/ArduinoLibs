/*
    DATE: 04/24/2022
    TIME: 13:10
   TITLE: INA219 Driver (INA219 HAL)
  AUTHOR: Jerome B. Ford
    NOTE: Still under development. as of 4/24/2022

 * REF:
 * https://www.ti.com/lit/ds/symlink/ina219.pdf?HQS=dis-dk-null-digikeymode-dsf-pf-null-wwe&ts=1650824530374&ref_url=https%253A%252F%252Fwww.ti.com%252Fgeneral%252Fdocs%252Fsuppproductinfo.tsp%253FdistId%253D10%2526gotoUrl%253Dhttps%253A%252F%252Fwww.ti.com%252Flit%252Fgpn%252Fina219
 * 
  Adafruit's INA219
  INA219 
   
      8.6 Register Maps
      8.6.1 Register Information
      The INA219 uses a bank of registers for holding configuration settings, measurement results, maximum/minimum
      limits, and status information. Table 2 summarizes the INA219 registers; Functional Block Diagram shows
      registers.
      Register contents are updated 4 μs after completion of the write command. Therefore, a 4-μs delay is required
      between completion of a write to a given register and a subsequent read of that register (without changing the
      pointer) when using SCL frequencies in excess of 1 MHz.
      
      Table 2. Summary of Register Set
      
      Pointer |
      Address |
      HEX     | REGISTER NAME | POWER-ON RESET  | DIR | FUNCTION
      --------+---------------+-----------------+-----+-----------------------------------------
      0x00    | Configuration | 0x399F          | R/W | All-register reset, settings for bus voltage range, PGA Gain, ADC resolution/averaging.
      0x01    | Shunt voltage | ---             | R   | Shunt voltage measurement data.
      0x02    | Bus voltage   | ---             | R   | Bus voltage measurement data.
      0x03    | Power         | 0x0000          | R   | Power measurement data.     
      0x04    | Current       | 0x0000          | R   | Contains the value of the current flowing hrough the shunt resistor.
      0x05    | Calibration   | 0x0000          | R/W | Sets full-scale range and LSB of current and power measurements. Overall system calibration.   
   
    8.6.2.1 Configuration Register (address = 00h) [reset = 399Fh]
    Figure 19. Configuration Register
    15  14  13    12  11  10    9     8     7     6     5     4     3     2     1     0
    RST —   BRNG  PG1 PG0 BADC3 BADC2 BADC1 BADC0 SADC3 SADC2 SADC1 SADC0 MODE2 MODE1 MODE0

*/

#ifndef INA219v2_h
#define INA219v2_h

#include "Wire.h"
#include "Arduino.h"

class INA219v2
{
  public:
    
    // NOTE: Currently these enums are not being used in this class. 4/24/2022 JBF
    enum INA219_REGISTERS_ADD
    {
      Configuration = 0x00,
       ShuntVoltage = 0x01,
         BusVoltage = 0x02,
              Power = 0x03,
            Current = 0x04,
        Calibration = 0x05,
    };
    
    enum SLAVE_ADDRESS_INA219
    {
      GND_GND_0x40h = 0x40, // <= Default!
      GND_VSP_0x41h = 0x41,
      GND_SDA_0x42h = 0x42,
      GND_SCL_0x43h = 0x43,
      VSP_GND_0x44h = 0x44,
      VSP_VSP_0x45h = 0x45,
      VSP_SDA_0x46h = 0x46,
      VSP_SCL_0x47h = 0x47,
      SDA_GND_0x48h = 0x48,
      SDA_VSP_0x49h = 0x49,
      SDA_SDA_0x4Ah = 0x4A,
      SDA_SCL_0x4Bh = 0X4B,
      SCL_GND_0x4Ch = 0X4C,
      SCL_VSP_0x4Dh = 0X4D,
      SCL_SDA_0x4Eh = 0X4E,
      SCL_SCL_0x4Fh = 0X4F,
    };

    // bit [13]
    enum CONFIG_REG_BUS_VOLTAGE_RANGE
    {
      FSR_16V = 0,
      FSR_32V = 1,
    };

    // bits [11 & 12]
    enum CONFIG_REG_SHUNT_VOLTAGE_PGA
    {
      GAIN_1_40mV = 0,
      GAIN_2_80mV = 1,
      GAIN_4_160mV = 2,
      GAIN_8_320mV = 4,
    };

    // BADC: Bus ADC Resolution/Averaging, Bits [7-10]
    //       These bits adjust the Bus ADC resolution (9-, 10-, 11-, or 12-bit) or set 
    //        the number of samples used when averaging results for the Bus Voltage Register (02h).

    // SADC: SADC Shunt ADC Resolution/Averaging, Bits [3-6]
    //        These bits adjust the Shunt ADC resolution (9-, 10-, 11-, or 12-bit) or set 
    //        the number of samples used when averaging results for the Shunt Voltage Register (01h).
    //        BADC (Bus) and SADC (Shunt) ADC resolution/averaging and conversion time settings are shown in Table 5. 

    enum CONFIG_REG_RESOLUTION_AVERAGING
    {
      RESOLUTION_09bits_tConv84us_0 = 0,
      RESOLUTION_10bits_tConv148us_1 = 1,
      RESOLUTION_11bits_tConv276us_2 = 2,
      RESOLUTION_12bits_tConv532us_3 = 3,
      RESOLUTION_12bits_tConv532us_8 = 4,
      SAMPLES_002_tConv_1060us_9 = 5,
      SAMPLES_004_tConv_2130us_A = 6,
      SAMPLES_008_tConv_4260us_B = 7,
      SAMPLES_016_tConv_8510us_C = 8,
      SAMPLES_032_tConv_17020us_D = 9,
      SAMPLES_064_tConv_34050us_E = 10,
      SAMPLES_128_tConv_34050us_F = 10,
    };

    // MODE: Operating Mode, Bits [0-2]
    //      Bits 0–2 Selects continuous, triggered, or power-down mode of operation. 
    //      These bits default to continuous shunt and bus measurement mode.
    //      The mode settings are shown in Table 6.
    enum CONFIG_REG_MODE
    {
      MODE_0_PowerDown = 0,
      MODE_1_ShuntVoltageTriggered = 1,
      MODE_2_BusVoltageTriggered = 2,
      MODE_3_ShuntAndBusTriggered = 3,
      MODE_4_ADC_Disabled = 4,
      MODE_5_ShuntVoltageContinuous = 5,
      MODE_6_BusVoltageContinuous = 6,
      MODE_7_ShuntAndBusContinuous = 7,
    };
 
  //  0----------------------------------------------------------------------------//
  INA219v2();
  //  1----------------------------------------------------------------------------//
  INA219v2(SLAVE_ADDRESS_INA219 newValue);
  //  2----------------------------------------------------------------------------//
  ~INA219v2();
  //  3----------------------------------------------------------------------------//
  void  InitzRegisterValues();
  //  4----------------------------------------------------------------------------//
  void  SetDeviceAddress(SLAVE_ADDRESS_INA219 newValue);
  //  5----------------------------------------------------------------------------//
  SLAVE_ADDRESS_INA219 GetDeviceAddress();
  //  6----------------------------------------------------------------------------//
  void  EndWire();
  //  7----------------------------------------------------------------------------//
  void SetRegister(uint8_t reg, uint16_t newVal);
  //  8----------------------------------------------------------------------------//
  uint16_t GetRegister(uint8_t reg);
  //  9----------------------------------------------------------------------------//
  void SetConfigurationReg0(uint16_t newVal);
  // 10----------------------------------------------------------------------------//
  uint16_t GetConfigurationReg0();
  // 11----------------------------------------------------------------------------//
  uint16_t GetShuntVoltageReg1();
  // 12----------------------------------------------------------------------------//
  uint16_t GetBusVoltageReg2();
  // 13----------------------------------------------------------------------------//
  uint16_t GetPowerReg3();
  // 14----------------------------------------------------------------------------//
  uint16_t GetCurrentReg4();
  // 15----------------------------------------------------------------------------//
  void SetCalibrationReg5(uint16_t newVal);
  // 16----------------------------------------------------------------------------//
  uint16_t GetCalibrationReg5();
  // 17----------------------------------------------------------------------------//
  void SetConfiguration_MODE(uint8_t newVal);
  // 18----------------------------------------------------------------------------//
  uint8_t GetConfiguration_MODE();
  // 19----------------------------------------------------------------------------//
  void SetConfiguration_SADC(uint8_t newVal);
  // 20----------------------------------------------------------------------------//
  uint8_t GetConfiguration_SADC();
  // 21----------------------------------------------------------------------------//
  void SetConfiguration_BADC(uint8_t newVal);
  // 22----------------------------------------------------------------------------//
  uint8_t GetConfiguration_BADC();
  // 23----------------------------------------------------------------------------//
  void SetConfiguration_PGA(uint8_t newVal);
  // 24----------------------------------------------------------------------------//
  uint8_t GetConfiguration_PGA();
  // 25----------------------------------------------------------------------------//
  void SetConfiguration_BRNG(uint8_t newVal);
  // 26----------------------------------------------------------------------------//
  uint8_t GetConfiguration_BRNG();
  // 27----------------------------------------------------------------------------//
  void SetConfiguration_RST(uint8_t newVal);
  // 28----------------------------------------------------------------------------//
  uint8_t GetConfiguration_RST();
  // 29----------------------------------------------------------------------------//
  void SetRshunt(float newVal);
  // 30----------------------------------------------------------------------------//
  float GetRshunt();
  // 31----------------------------------------------------------------------------//
  float Read_Shunt_mV();
  // 32----------------------------------------------------------------------------//
  float Read_Vload();
  // 33----------------------------------------------------------------------------//
  float CalculateLoadCurrent_mA();
  // 34----------------------------------------------------------------------------//
  float CalculateLoadPower_mW();
  //  1000-------------------------------------------------------------------------//
  String About();
  //  1001-------------------------------------------------------------------------//
  private:
    SLAVE_ADDRESS_INA219 _deviceAddress = GND_GND_0x40h;
   
    // Bit fiels of theconfig register. (Total of 16bits)
    struct CFG_REG_INA219
    {
      uint8_t Mode:3;
      uint8_t SADC:4;
      uint8_t BADC:4;
      uint8_t PGA:2;
      uint8_t BRNG:1;
      uint8_t NOP:1;
      uint8_t RST:1;
    };
    
    // Union of the 16bit register + the struct of bitfields. Total size is only 2 bytes (16 bits).
    union INA219CFGREG
    {
      uint16_t Register;      // Direct menipulation of register's 16bits
      CFG_REG_INA219 fields;  // Direct menipulation of register's individule fields. (without bit menipulation code)
    };

 
    struct _INA219_REG_VALUES
    {
      //uint16_t Configuration;   // Version 1, used bit menipulation, ie Masking bits, Shifting Left&Righ, AND-ing & OR-ing.
      
      INA219CFGREG Configuration; // Version 2, Uses a compound structure {Union + Struct}. This makes it posible to get rid of all the bit menipulation code. 
                                  // Not sure if this make it faster, but it does make it cleaner code! 
      uint16_t ShuntVoltage;
      uint16_t BusVoltage;
      uint16_t Power;
      uint16_t Current;
      uint16_t Calibration;

      float Rshunt;
      float Vload;
      float Vshunt_mV;
      float Ishunt_mA;
      float Pload_mW; 
    };
  
    _INA219_REG_VALUES _registersValues;
      
    union ByteField16
    {
      float valFloat;
      uint16_t valUint16;
      int16_t valSint16;
      uint8_t bytes[4];
    };

};
#endif