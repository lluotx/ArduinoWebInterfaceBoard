/*
 cg8800.ino
 Created by Larry Luo
 Arduino Uno
 */

#include <SoftwareSerial.h>

int alarm =0;
int alarm1 =0;
int code = 0;
int timer = 6;
int State = 0;         // current state 
int lastState = 0;     // previous state 
char inByte;
char retry; 
SoftwareSerial portOne(A0,A1);

void setup()
{
  Serial.begin(9600);
  portOne.begin(9600);
  DDRD = 0;
  DDRB = 0;
  pinMode(3, INPUT);
  pinMode(6, INPUT);
}

void loop()
{
  State = digitalRead(2);
  if (State != lastState)                          //Check Alarm status
  {
    if (State == HIGH) 
    { 
      retry =0;
      alarm = 0;
      alarm1 = 0;
      code  &= 0x40;
      if (digitalRead(3) == HIGH) //Water Sensor
        retry++;
      delay(280);
      if (digitalRead(3) == HIGH)  retry++;
      delay(280);
      if (digitalRead(3) == HIGH)  retry++;
      if(retry>0)
      {
        code |= 1;
      }
      if((retry>0)&&(retry<3))
        alarm1 |=1;

      retry =0;
      if (digitalRead(6) == HIGH) //Indoor IR
        retry++;
      delay(280);
      if (digitalRead(6) == HIGH)  retry++;
      delay(280);
      if (digitalRead(6) == HIGH)  retry++;
      if(retry>0)
      {
        code |= 0x08;
      }
      if((retry>0)&&(retry<3))
        alarm1 |=8;

      retry =0;
      if (digitalRead(4) == HIGH) //Back Door  IR
        retry++;
      delay(280);
      if (digitalRead(4) == HIGH)  retry++;
      delay(280);
      if (digitalRead(4) == HIGH)  retry++;
      if(retry>0)
      {
        code |= 2;
      }
      if((retry>0)&&(retry<3))
        alarm1 |=2;

      retry =0;
      if (digitalRead(5) == HIGH) //Back Door Switch
        retry++;
      delay(280);
      if (digitalRead(5) == HIGH)  retry++;
      delay(280);
      if (digitalRead(5) == HIGH)  retry++;
      if(retry>0)
      {
        code |= 4;
      }
      if((retry>0)&&(retry<3))
        alarm1 |=4;

      retry =0;
      if (digitalRead(7) == HIGH)  //Front Door IR
        retry++;
      delay(280);
      if (digitalRead(7) == HIGH)  retry++;
      delay(280);
      if (digitalRead(7) == HIGH)  retry++;
      if(retry>0)
      {
        code |= 0x10;
      }
      if((retry>0)&&(retry<3))
        alarm1 |=0x10;
      
      retry =0;
      if (digitalRead(9) == HIGH)  //Fire Alarm
        retry++;
      delay(280);
      if (digitalRead(9) == HIGH)  retry++;
      delay(280);
      if (digitalRead(9) == HIGH)  retry++;
      if(retry>0)
      {
        code |= 0x20;
      } 
      if((retry>0)&&(retry<3))
        alarm1 |=0x20;
      
      if(alarm1==1)
          alarm = 1;
      else if(alarm1==2)
           alarm = 2;
      else if(alarm1==4)
          alarm = 2;
      else if(alarm1==8)
          alarm = 1;
      else if(alarm1==0x10)
          alarm = 8;
      else if(alarm1==0x20)
          alarm = 0x0f;
 
      Serial.write(alarm);

      //Serial.print(alarm1,HEX);	  //Debug purpose
      //delay(1);
      //Serial.write(0);
    }
    else
    {
      //After 1 minute CG8800 state change from High to Low
      Serial.write(0);
    }
  }
  lastState = State;

  if(portOne.available() > 0)         //Receive Uno Shield query output current LED value
  {
    //Serial.write(portOne.read());
    if(portOne.read()==0x55)
    { 
      retry =0;
      alarm = 0;
      alarm1 = 0;
      code  &= 0x40;
      if (digitalRead(3) == HIGH) //Water Sensor
        retry++;
      delay(280);
      if (digitalRead(3) == HIGH)  retry++;
      delay(280);
      if (digitalRead(3) == HIGH)  retry++;
      if(retry>0)
      {
        alarm |= 1;
        code |= 1;
      }
      if((retry>0)&&(retry<3))
        alarm1 |=1;

      retry =0;
      if (digitalRead(6) == HIGH) //Indoor IR
        retry++;
      delay(280);
      if (digitalRead(6) == HIGH)  retry++;
      delay(280);
      if (digitalRead(6) == HIGH)  retry++;
      if(retry>0)
      {
        alarm |= 0x1;
        code |= 0x08;
      }
      if((retry>0)&&(retry<3))
        alarm1 |=8;

      retry =0;
      if (digitalRead(4) == HIGH) //Back Door  IR
        retry++;
      delay(280);
      if (digitalRead(4) == HIGH)  retry++;
      delay(280);
      if (digitalRead(4) == HIGH)  retry++;
      if(retry>0)
      {
        alarm |= 0x02;
        code |= 2;
      }
      if((retry>0)&&(retry<3))
        alarm1 |=2;

      retry =0;
      if (digitalRead(5) == HIGH) //Back Door Switch
        retry++;
      delay(280);
      if (digitalRead(5) == HIGH)  retry++;
      delay(280);
      if (digitalRead(5) == HIGH)  retry++;
      if(retry>0)
      {
        alarm |= 0x02;
        code |= 4;
      }
      if((retry>0)&&(retry<3))
        alarm1 |=4;

      retry =0;
      if (digitalRead(7) == HIGH)  //Front Door IR
        retry++;
      delay(280);
      if (digitalRead(7) == HIGH)  retry++;
      delay(280);
      if (digitalRead(7) == HIGH)  retry++;
      if(retry>0)
      {
        alarm |= 0x8;
        code |= 0x10;
      }
      if((retry>0)&&(retry<3))
        alarm1 |=0x10;
      
      retry =0;
      if (digitalRead(9) == HIGH)  //Fire Alarm
        retry++;
      delay(280);
      if (digitalRead(9) == HIGH)  retry++;
      delay(280);
      if (digitalRead(9) == HIGH)  retry++;
      if(retry>0)
      {
        alarm |= 0xf;
        code |= 0x20;
      } 
      if((retry>0)&&(retry<3))
        alarm1 |=0x20;

      portOne.write(code);
      portOne.write(alarm1);

      //code = 0;
      //Serial.print(code,HEX);    //Debug
    }
  }    
  delay(100);
}

// END OF FILE

