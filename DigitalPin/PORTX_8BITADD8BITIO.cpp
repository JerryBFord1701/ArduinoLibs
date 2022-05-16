/*
        TITLE: Digital PortX on a MCU that has 8bit Register Addressing and the ports are 8bits wide.
      VERSION: 1.2022.0121
       AUTHOR: Jerome B. Ford
  DESCRIPTION: Object to directly control ports registers on a uC. Developed on an Arduino UNO
   REFERENCES:
              1) https://www.avrfreaks.net/forum/pass-ports-and-their-registers-functions
              2) https://www.avrfreaks.net/forum/passing-port-argument
*/
#include "PORTX_8BITADD8BITIO.h"

//  0)------------------------------------------------------------------------------------//
PORTX_8BITADD8BITIO::PORTX_8BITADD8BITIO()
{
    //SetPortRef(volatile uint8_t *PORTx, volatile uint8_t *DDRx, volatile uint8_t *PINx);
    _thePORTx = NULL;
    _theDDRx = NULL;
    _thePINx = NULL;
}
//  1)------------------------------------------------------------------------------------//
PORTX_8BITADD8BITIO::PORTX_8BITADD8BITIO(volatile uint8_t *PORTx)
{
    _thePORTx = PORTx;
    _theDDRx = NULL;
    _thePINx = NULL;
}
//  2)------------------------------------------------------------------------------------//
PORTX_8BITADD8BITIO::PORTX_8BITADD8BITIO(volatile uint8_t *PORTx, volatile uint8_t *DDRx)
{
    _thePORTx = PORTx;
    _theDDRx = DDRx;
    _thePINx = NULL;
} 
//  3)------------------------------------------------------------------------------------//
PORTX_8BITADD8BITIO::PORTX_8BITADD8BITIO(volatile uint8_t *PORTx, volatile uint8_t *DDRx, volatile uint8_t *PINx)
{
    _thePORTx = PORTx;
    _theDDRx = DDRx;
    _thePINx = PINx;
} 
//  4)------------------------------------------------------------------------------------//
PORTX_8BITADD8BITIO::~PORTX_8BITADD8BITIO()
{
    _thePORTx = NULL;
    _theDDRx = NULL;
    _thePINx = NULL;
}
//  5)------------------------------------------------------------------------------------//
void PORTX_8BITADD8BITIO::SetBit0(uint8_t theState)
{
    SetBit(0, theState);
}
//  6)------------------------------------------------------------------------------------//
void PORTX_8BITADD8BITIO::SetBit1(uint8_t theState)
{
    SetBit(1, theState);
}
//  7)------------------------------------------------------------------------------------//
void PORTX_8BITADD8BITIO::SetBit2(uint8_t theState)
{
    SetBit(2, theState);
}
//  8)------------------------------------------------------------------------------------//
void PORTX_8BITADD8BITIO::SetBit3(uint8_t theState)
{
    SetBit(3, theState);
}
//  9)------------------------------------------------------------------------------------//
void PORTX_8BITADD8BITIO::SetBit4(uint8_t theState)
{
    SetBit(4, theState);
}
// 10)------------------------------------------------------------------------------------//
void PORTX_8BITADD8BITIO::SetBit5(uint8_t theState)
{
    SetBit(5, theState);
}
// 11)------------------------------------------------------------------------------------//
void PORTX_8BITADD8BITIO::SetBit6(uint8_t theState)
{
    SetBit(6, theState);
}
// 12)------------------------------------------------------------------------------------//
void PORTX_8BITADD8BITIO::SetBit7(uint8_t theState)
{
    SetBit(7, theState);
}
// 13)------------------------------------------------------------------------------------//
void PORTX_8BITADD8BITIO::SetBit(uint8_t theBit, uint8_t theState)
{
    if(_thePORTx != NULL)
    {
        if(theState == 0)
        {
            *_thePORTx &= ~(1 << (theBit & 7));
        }
        else
        {
            *_thePORTx |= (1 << (theBit & 7));
        }
    }
}
// 14)------------------------------------------------------------------------------------//
uint8_t PORTX_8BITADD8BITIO::GetBit0()
{
    return GetBit(0);
}
// 15)------------------------------------------------------------------------------------//
uint8_t PORTX_8BITADD8BITIO::GetBit1()
{
    return GetBit(1);
}
// 16)------------------------------------------------------------------------------------//
uint8_t PORTX_8BITADD8BITIO::GetBit2()
{
    return GetBit(2);
}
// 17)------------------------------------------------------------------------------------//
uint8_t PORTX_8BITADD8BITIO::GetBit3()
{
    return GetBit(3);
}
// 18)------------------------------------------------------------------------------------//
uint8_t PORTX_8BITADD8BITIO::GetBit4()
{
    return GetBit(4);
}
// 19)------------------------------------------------------------------------------------//
uint8_t PORTX_8BITADD8BITIO::GetBit5()
{
    return GetBit(5);
}
// 20)------------------------------------------------------------------------------------//
uint8_t PORTX_8BITADD8BITIO::GetBit6()
{
    return GetBit(6);
}
// 21)------------------------------------------------------------------------------------//
uint8_t PORTX_8BITADD8BITIO::GetBit7()
{
    return GetBit(7);
}
// 22)------------------------------------------------------------------------------------//
uint8_t PORTX_8BITADD8BITIO::GetBit(uint8_t theBit)
{
    if(_thePINx != NULL)
    {
        return (*_thePINx & (1 << (theBit & 7))) >> (theBit & 7);
    }
    else
    {
        return 0;
    }
}
// 23)------------------------------------------------------------------------------------//
void PORTX_8BITADD8BITIO::SetPort(uint8_t OutputBits)
{
    if(_thePORTx != NULL)
    {
        *_thePORTx = OutputBits;
    }
}
// 24)------------------------------------------------------------------------------------//
uint8_t PORTX_8BITADD8BITIO::GetPort()
{
    if(_thePINx != NULL)
    {
        return *_thePINx;
    }
    else
    {
        return 0;
    }
}
// 25)------------------------------------------------------------------------------------//
void PORTX_8BITADD8BITIO::SetDirection(uint8_t DirectionBits)
{
    if(_theDDRx != NULL)
    {
        *_theDDRx = DirectionBits;
    }
}
// 26)------------------------------------------------------------------------------------//
uint8_t PORTX_8BITADD8BITIO::GetDirection()
{
    if( _theDDRx != NULL)
    {
        return *_theDDRx;
    }
    else
    {
        return 0;
    }
}
// 27)------------------------------------------------------------------------------------//
void PORTX_8BITADD8BITIO::SetPortRef(volatile uint8_t *PORTx, volatile uint8_t *DDRx, volatile uint8_t *PINx)
{
    _thePORTx = PORTx;
    _theDDRx = DDRx;
    _thePINx = PINx;
}
// 28)------------------------------------------------------------------------------------//
void PORTX_8BITADD8BITIO::SetConfiguration(uint8_t DirectionBits, uint8_t BitStates, uint8_t PullUpBits)
{
    *_theDDRx = DirectionBits;  // Use this to tell whether the Pin is an INPUT = 0 or OUTPUT = 1
    *_thePORTx = BitStates | PullUpBits;  // Port pin registers whether the pin is a HIGH or a LOW
} 
//----------------------------------------------------------------------------------------//


