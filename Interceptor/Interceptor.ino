#include <SPI.h>
 
const int PIN_CS = 10;
const int GAIN_1 = 0x1;
const int GAIN_2 = 0x0;
 
void setup()
{
  pinMode(PIN_CS, OUTPUT);
  SPI.begin();  
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  pinMode(A0, INPUT);
  Serial.begin(9600);
}
 
void setOutput(byte channel, byte gain, byte shutdown, unsigned int val)
{
  byte lowByte = val & 0xff; //sends lower 8 bits of 12-bit value (& oxff means make everything a 1 except what val has as a zero - to clear?)
  byte highByte = ((val >> 8) & 0xff) | channel << 7 | gain << 5 | shutdown << 4;
   
  PORTB &= 0xfb; //PORTB means bankB on the arduino. this sends a 1 to bankB pin 3, which is pin 10
  SPI.transfer(highByte);
  SPI.transfer(lowByte);
  PORTB |= 0x4; //this sends a 0 to bankB pin3 which is pin 10. 
}
 
void loop()
{
  int pedal = analogRead(A0) * 4 *1.24;
  Serial.println(pedal);
  setOutput(0, GAIN_2, 1, pedal);
  setOutput(1, GAIN_2, 1, pedal);
/*  
 //high-res triangular wave
 for (int i=0; i < 4096; i+=32)   
 {
  setOutput(0, GAIN_2, 1, i);
 }
*/
}
