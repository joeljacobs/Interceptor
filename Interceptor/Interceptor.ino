#include <SPI.h>
#include <mcp_can.h>
 
const int PIN_CS = 10;
const int GAIN_1 = 0x1;
const int GAIN_2 = 0x0;
const int SPI_CS_PIN = 9;

MCP_CAN CAN(SPI_CS_PIN); 
 
void setup()
{
  pinMode(PIN_CS, OUTPUT);
  SPI.begin();  
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  pinMode(A0, INPUT);
  Serial.begin(115200);
  
      while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
        delay(100);
    }
    Serial.println("CAN BUS Shield init ok!");
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
 
unsigned char stmp[8] = {0, 0, 0, 0, 0, 0, 0, 0};
void loop()
{
  int pedal = analogRead(A0) * 4 *1.24;
  Serial.print(pedal);
  setOutput(0, GAIN_2, 1, pedal);
  setOutput(1, GAIN_2, 1, pedal);
/*  
 //high-res triangular wave
 for (int i=0; i < 4096; i+=32)   
 {
  setOutput(0, GAIN_2, 1, i);
 }
*/
    // send data:  id = 0x00, standrad frame, data len = 8, stmp: data buf
    stmp[7] = stmp[7]+1;
    if(stmp[7] == 100)
    {
        stmp[7] = 0;
        stmp[6] = stmp[6] + 1;
        
        if(stmp[6] == 100)
        {
            stmp[6] = 0;
            stmp[5] = stmp[6] + 1;
        }
    }
    
    CAN.sendMsgBuf(0x00, 0, 8, stmp);
    Serial.println(stmp[0] + stmp[1] + stmp[2] + stmp[3] + stmp [4] + stmp[5] + stmp[6] + stmp[7]);
    delay(100);                       // send data per 100ms

}
