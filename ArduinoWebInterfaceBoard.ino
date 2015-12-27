/*
 ArduinoWebInterfaceBoard.ino
 Created by Larry Luo
 Arduino Uno with Ethernet Shield
 */

#include <SPI.h>
#include <Ethernet.h>
#include <dht.h>
#include <SoftwareSerial.h>

dht DHT;
int copin = A2;
int coValue;
int alarm;
int timer = 6;
#define DHT22_PIN 5
char inByte;
char inByte1;
char sw = 0;

byte mac[] = { 0x00, 0xAD, 0xBE, 0xEF, 0xFE, 0x1D };   //physical mac address
byte ip[] = { 192, 168, 43, 178 };                     //ip in lan (that's what you need to use in your browser. ("192.168.1.178")
byte gateway[] = { 192, 168, 43, 1 };                  //internet access via router
byte subnet[] = { 255, 255, 255, 0 };                  //subnet mask
EthernetServer server(8080);                           //server port     
String readString;
SoftwareSerial portOne(A0,A1);                         //Create second serial port using A0, A1 pin

void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(115200);

    while (!Serial) {
    ;                                                   // wait for serial port to connect. If not then loop here.
    }

    portOne.begin(9600);
    portOne.write(0x55);
    pinMode(4, OUTPUT); 
    digitalWrite(4, HIGH);
    pinMode(3, OUTPUT); 
    digitalWrite(3, HIGH);
    pinMode(6, OUTPUT); 
    digitalWrite(6, HIGH);
    // start the Ethernet connection and the server:
    Ethernet.begin(mac, ip, gateway, subnet);
    server.begin();
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());
    Serial.println("DHT TEST PROGRAM ");
    Serial.print("LIBRARY VERSION: ");
    Serial.println(DHT_LIB_VERSION);
    Serial.println();
    Serial.println("CO Temp(F) Humid(%)"); 
    //  portOne.listen(); 
    delay(500); 

    if(portOne.available() >= 2) {
    inByte = portOne.read();
    inByte1 = portOne.read();
    //Serial.print(inByte,DEC);
    }   
  }


void loop() {
  if(--timer ==0)
  {
    portOne.write(0x55);
    int chk = DHT.read22(DHT22_PIN);

    switch (chk)
    {
    case DHTLIB_OK:
        //Serial.print("OK,\t");
        break;
    case DHTLIB_ERROR_CHECKSUM:
        Serial.print("Checksum error,\t");
        break;
    case DHTLIB_ERROR_TIMEOUT:
        Serial.print("Time out error,\t");
        break;
    default:
        Serial.print("Unknown error,\t");
        break;
    }

    coValue = analogRead(copin);
    // DISPLAY DATA
    Serial.print(coValue, 1);
    Serial.print(",");
    Serial.print(DHT.temperature*9/5+32, 1);
    Serial.print(",");
    Serial.print(DHT.humidity, 1);
    Serial.print(",");
    Serial.print(inByte,DEC);
    Serial.print(",");
    Serial.print(inByte1,DEC);
    Serial.println();
//  portOne.listen(); 
    timer = 599;              //599+1=600x100ms=60s
  }

  delay(100); 
      
  if(portOne.available() >= 2) {
    inByte = portOne.read();
    inByte1 = portOne.read();
  //Serial.print(inByte,DEC);
  }
  // Create a client connection
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {   
      if (client.available()) {
        char c = client.read();
     
        //read char by char HTTP request
        if (readString.length() < 100) {
          //store characters to string
          readString += c;
          //Serial.print(c);
         }

         //if HTTP request has ended
         if (c == '\n') {          
           //Serial.println(readString); //print to serial monitor for debuging
     
           client.println("HTTP/1.1 200 OK"); //send new page
           client.println("Content-Type: text/html");
           client.println();     
           client.println("<HTML>");
           client.println("<HEAD>");
           client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
           client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
           client.println("<link rel='stylesheet' type='text/css' href='http://randomnerdtutorials.com/ethernetcss.css' />");
           client.println("<TITLE>Home Security System</TITLE>");
           client.println("</HEAD>");
           client.println("<BODY>");
           client.println("<H1>Home Security System</H1>");
           client.println("<hr />");
           client.println("<br />");  
           client.println("<a href=\"/?button1on\"\">Enable Alarm SW1=1</a>");
           client.println("<a href=\"/?button1off\"\">Reset SW1=0</a><br />");   
           client.println("<br />");     
           client.println("<a href=\"/?button2on\"\">DisAlarm,SW=2</a>");
           client.println("<a href=\"/?button2off\"\">Reset SW2=0</a><br />");   
           client.println("<br />");     
           client.println("<a href=\"/?button3on\"\">Alarm Sound On,SW=4</a>");
           client.println("<a href=\"/?button3off\"\">Reset SW3=0</a><br />");   
           client.println("<br />");     
           client.println("Gas Quality=");
           client.println(coValue);
           client.println("Temp="); 
           client.println(DHT.temperature*9/5+32);
           client.println("Humidity="); 
           client.println(DHT.humidity); 
           client.println("Alarm="); 
           client.println(inByte, DEC); 
           client.println("Alarm New="); 
           client.println(inByte1, DEC); 
           client.println("<br />");     
           if((inByte&1)==1)
             client.println("Water Sensor, "); 
           if((inByte&2)==2)
             client.println("BackDoorIR, "); 
           if((inByte&4)==4)
             client.println("BackDoorOpen, "); 
           if((inByte&8)==8)
             client.println("IndoorIR, "); 
           if((inByte&0x10)==0x10)
             client.println("FrontDoor, "); 
           if((inByte&0x20)==0x20)
             client.println("FireAlarm, "); 
           client.println("<br />");     
           //controls the Arduino if you press the buttons
           if (readString.indexOf("?button1on") >0){
               digitalWrite(3, LOW);
               delay(400); 
               digitalWrite(3, HIGH);
               //sw |= 1;
           }
           if (readString.indexOf("?button1off") >0){
               digitalWrite(3, HIGH);
               sw &= ~1;
           }
           if (readString.indexOf("?button2on") >0){
               digitalWrite(4, LOW);                       // waits 15ms for the servo to reach the position 
               delay(400); 
               digitalWrite(4, HIGH);                      // waits 15ms for the servo to reach the position 
               //sw |= 2;
            }
           if (readString.indexOf("?button2off") >0){
               digitalWrite(4, HIGH);                      // waits 15ms for the servo to reach the position 
               sw &= ~2;
           }
           if (readString.indexOf("?button3on") >0){
               digitalWrite(6, LOW);                       // waits 15ms for the servo to reach the position 
               delay(400); 
               digitalWrite(6, HIGH);                      // waits 15ms for the servo to reach the position 
               //sw |= 4;
            }
           if (readString.indexOf("?button3off") >0){
               digitalWrite(6, HIGH);                      // waits 15ms for the servo to reach the position 
               sw &= ~4;
           }
            //clearing string for next read
           client.println("Switch=0x"); 
           client.println(sw, HEX); 
           client.println("<br />"); 
           client.println("</BODY>");
           client.println("</HTML>");
     
           delay(1);
           //stopping client
           client.stop();

            readString="";  
         }
       }
    }
  }
}
