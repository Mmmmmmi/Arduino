//Use soft SoftwareSerial
#include <SoftwareSerial.h>
SoftwareSerial mySerial(4, 5);

#define my_Serial mySerial
//#define my_Serial Serial  //Define serial communication as Serial1 
String msg = ""; //Define a string of characters 

void setup()
{
  // Initialize Bluetooth communication baud rate  
  my_Serial.begin(9600);
  // Initialize serial monitor communication baud rate 
  Serial.begin(9600);
}
void loop()
{
  //Receives signal once and give feedback once to the other communication side. 
  if (my_Serial.available() > 0)  //If there is data input in serial port 
  {
    msg = my_Serial.readStringUntil('\n'); //All content before acquiring line break
    Serial.println(msg);                   //Display character string of msg in the serial monitor  
    my_Serial.println("bluetooth respond");  //Send data to the other side of Bluetooth communication 
  }
}
