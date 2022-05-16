/*
      Title:    DigitalOutPin 
    Started:    10/28/2019
    Version:    1.2019.1101
     Author:    Jerome B. Ford
*/

#include "Arduino.h"
#include "DigitalOutPin.h"

uint8_t DigitalOutPin::Count = 0;

/*-----------------------------------------------------------------------------------------*/
// GP CONSTRUCTOR.
DigitalOutPin::DigitalOutPin(int pin)
{
  // Set default pin to 13.
  _pin = pin;
    Count++;
}
/*-----------------------------------------------------------------------------------------*/
DigitalOutPin::~DigitalOutPin()
{
    Count--;
}
/*-----------------------------------------------------------------------------------------*/ 
void DigitalOutPin::begin(int thePinNumber)
{
    _pin = thePinNumber;
    pinMode(_pin, OUTPUT);
    off();
    _countHi = 0;
}
/*-----------------------------------------------------------------------------------------*/
void DigitalOutPin::on()
{
    digitalWrite(_pin, HIGH); 
    
    if (_isLow == LOW)
    {
        _isLow = HIGH;
        ++_countHi;
    }
}
/*-----------------------------------------------------------------------------------------*/
void DigitalOutPin::off()
{
  digitalWrite(_pin, LOW);
  _isLow = LOW;
}
/*-----------------------------------------------------------------------------------------*/
bool DigitalOutPin::status()
{
  return _isLow;
}
/*-----------------------------------------------------------------------------------------*/
uint32_t DigitalOutPin::GetCountHigh()
{
  return _countHi;
}
/*-----------------------------------------------------------------------------------------*/
void DigitalOutPin::SetCountHigh(uint32_t newVal)
{
  _countHi = newVal;
}
/*-----------------------------------------------------------------------------------------*/
int DigitalOutPin::GetPinNumber()
{
    return _pin;
}
/*-----------------------------------------------------------------------------------------*/
void DigitalOutPin::SetPinNumber(int newPin)
{
    _pin = newPin;
}
/*-----------------------------------------------------------------------------------------*/