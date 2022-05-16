/*
      Title:    DigitalOutPin 
    Started:    10/28/2019
    Version:    1.2019.1101
     Author:    Jerome B. Ford
*/

#ifndef DigitalOutPin_h
#define DigitalOutPin_h
#include "Arduino.h"
//==============================================//
class DigitalOutPin
{
  public: 
    static uint8_t Count;
    
    DigitalOutPin(int pin);
    ~DigitalOutPin();

    void begin(int thePinNumber);
    void on();
    void off();
    bool status();
    uint32_t GetCountHigh();
    void SetCountHigh(uint32_t newVal);

    int GetPinNumber();
    void SetPinNumber(int newVal);

  private:
    int _pin = -1;
    bool _isLow = LOW;
    uint32_t _countHi = 0;
};

#endif
