
#include "Arduino.h"


//==============================================//
class DigitalPin
{
  public: 
  
  DigitalPin(int pin);
  
  void begin();
  void on();
  void off();
  bool status();
  long countHi = 0;

  private:
  int _pin = 13;
  bool _status;
  bool _isLow = LOW;
};
//==============================================// 

/*..............................................*/
// GP CONSTRUCTOR.
DigitalPin::DigitalPin(int pin)
{
  // Set default pin to 13.
  _pin = pin;
  //pinMode(_pin,OUTPUT);
  
}
/*..............................................*/ 
void DigitalPin::begin()
{
  pinMode(_pin,OUTPUT);
  countHi = 0;
}
/*..............................................*/
void DigitalPin::on()
{
  
	digitalWrite(_pin,HIGH); 
  
  _status  = 1;

  if (_isLow == LOW)
  {
	  _isLow = HIGH;
	  ++countHi;
  }

}
/*..............................................*/
void DigitalPin::off()
{
  digitalWrite(_pin,LOW);
  _status  = 0;
  _isLow = LOW;

}
/*..............................................*/
bool DigitalPin::status()
{
  return _status;
}
/*..............................................*/

