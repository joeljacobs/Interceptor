#include <SPI.h>
#include <mcp_can.h>
 
//ADA
const int PIN_CS = 10;
const int GAIN_1 = 0x1;
const int GAIN_2 = 0x0;
unsigned int pedal0 = 0;
unsigned int pedal1 = 0;

//CAN
const int SPI_CS_PIN = 9;
unsigned char flagRecv = 0;
unsigned char len = 3;
unsigned char buf[8];
unsigned long interval=100; // the time we need to wait
unsigned long previousMillis=0; // millis() returns an unsigned long.
unsigned char stmp[5] = {0, 0, 0, 0, 0};
MCP_CAN CAN(SPI_CS_PIN); 

//SERIAL INPUT (UNUSED)
char receivedChar;
 
void setup()
{
  Serial.begin(115200);
 //SPI (ADA & CAN)
  SPI.begin();  
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  
 //ADA
  pinMode(PIN_CS, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  PORTB &= 0xfE; //latch LDAC by taking pin 8 low
  
 //CAN
    while (CAN_OK != CAN.begin(CAN_1000KBPS))              // init can bus : baudrate = 1000                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            00k
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
        delay(100);
    }
    Serial.println("CAN BUS Shield init ok!");

  attachInterrupt(2, MCP2515_ISR, FALLING); // start interrupt for can
  CAN.init_Mask(0, 0, 0x3ff);                         // there are 2 mask in mcp2515, you need to set both of them
  CAN.init_Mask(1, 0, 0x3ff);
  CAN.init_Filt(0, 0, 0x200); //0x200
}

//ADA Output
void setOutput(byte channel, byte gain, byte shutdown, unsigned int val)
{
  byte lowByte = val & 0xff; //sends lower 8 bits of 12-bit value (& oxff means make everything a 1 except what val has as a zero - to clear?)
  byte highByte = ((val >> 8) & 0xff) | channel << 7 | gain << 5 | shutdown << 4;
   
  PORTB &= 0xfb; //PORTB means bankB on the arduino. this sends a 1 to bankB pin 3, which is pin 10 - joel thinks this is backwards - sends a 0
  SPI.transfer(highByte);
  SPI.transfer(lowByte);
  PORTB |= 0x4; //this sends a 0 to bankB pin3 which is pin 10. - Joel thinks this is backwards - sends a 1
}

//CAN INTERRUPT FUNCTION
void MCP2515_ISR()
{
    flagRecv = 1;
}

//SERIAL INPUT (unused)
void recvOneChar() {
    if (Serial.available() > 0) {
        receivedChar = Serial.read();
    }
}

//MAIN LOOP
void loop()
{
  //ADC ---------------------------------------------------------------
  pedal0 = analogRead(A0) * 4 *1.24;
  pedal1 = analogRead(A1) * 4 *1.24;
  
  if (pedal0 > 3199) //3700
  {
    pedal0 = 3199;
  }
  if (pedal1 > 3999) //4500
  {
    pedal1 = 3999;
  }
  setOutput(0, GAIN_2, 1, pedal0);
  setOutput(1, GAIN_2, 1, pedal1);

  //PORTB &= 0xfE; //latch LDAC by taking pin 8 low
  //PORTB |= 0x1; //unlatch LCAC by taking pin 8 high

  //CAN ----------------------------------------------------------------
  unsigned long currentMillis = millis(); // grab current time
  //check if "interval" time has passed (1000 milliseconds)
  if ((unsigned long)(currentMillis - previousMillis) >= interval) {
    
    // send data:  id = 0x10, standrad frame, data len = 8, stmp: data buf
    stmp[0] = ((pedal0 >> 8) & 0xff);
    stmp[1] = pedal0 & 0xff;
    stmp[2] = ((pedal1 >> 8) & 0xff);
    stmp[3] = pedal1 & 0xff;
    //stmp[4] = 0b00101001; //counter starts two bits in and is 2 bits, cksum is next 4 bits
    stmp[4] = 0b01010101;
    CAN.sendMsgBuf(0x201, 0, 5, stmp);
    
    previousMillis = millis();
  }

  if(flagRecv)                   // check if get data
      {

        flagRecv = 0;                // clear flag
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

        Serial.println("\r\n------------------------------------------------------------------");
        Serial.print("Get Data From id: ");
        Serial.println(CAN.getCanId());
        for(int i = 0; i<len; i++)    // print the data
        {
            Serial.print("0x");
            Serial.print(buf[i], HEX);
            Serial.print("\t");
        }
        Serial.println();

      }


}
