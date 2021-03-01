/*
 ESP8266 --> ThingSpeak Channel
 
 This sketch sends the value of Analog Input (A0) to a ThingSpeak channel
 using the ThingSpeak API (https://www.mathworks.com/help/thingspeak).
 
 Requirements:
 
   * ESP8266 Wi-Fi Device
   * Arduino 1.8.8+ IDE
   * Additional Boards URL: http://arduino.esp8266.com/stable/package_esp8266com_index.json
   * Library: esp8266 by ESP8266 Community
   * Library: ThingSpeak by MathWorks
 
 ThingSpeak Setup:
 
   * Sign Up for New User Account - https://thingspeak.com/users/sign_up
   * Create a new Channel by selecting Channels, My Channels, and then New Channel
   * Enable one field
   * Enter SECRET_CH_ID in "secrets.h"
   * Enter SECRET_WRITE_APIKEY in "secrets.h"
 Setup Wi-Fi:
  * Enter SECRET_SSID in "secrets.h"
  * Enter SECRET_PASS in "secrets.h"
  
 Tutorial: http://nothans.com/measure-wi-fi-signal-levels-with-the-esp8266-and-thingspeak
   
 Created: Feb 3, 2017 by Hans Scharler (http://nothans.com)
*/

#include "ThingSpeak.h"
#include "secrets.h"
#include "espcom.h"



unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

#include <ESP8266WiFi.h>

char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key index number (needed only for WEP)
WiFiClient  client;

int state = VALUE_READED;

void SendToServer(unsigned int field, int value);

int ReadInt16(Stream* stream);
void WriteInt16(Stream* stream, int i);

void setup() {
  Serial.begin(115200);
  delay(100);

  WiFi.mode(WIFI_STA);

  ThingSpeak.begin(client);
}
String inString = "";    // string to hold input
long inStringLong = 0;
void loop() {

  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Attempting");
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("Connected");
  }
  if (Serial.available() > 0) {
    int inChar = Serial.read();
    if (isDigit(inChar)) {
      // convert the incoming byte to a char and add it to the string:
      inString += (char)inChar;
    }
    // if you get a newline, print the string, then the string's value:
    if (inChar == '\n') {
      Serial.print("Value:");
      inStringLong = inString.toInt();
      Serial.println(inStringLong);
      Serial.print("String: ");
      Serial.println(inString);
      // clear the string for new input:
      inString = "";


      unsigned int field = inStringLong % 10; // for incoming serial data
      int value = inStringLong / 10; // for incoming serial data 
      SendToServer(field, value);
    }

    

    /*
    unsigned int field = 0; // for incoming serial data
    int value = 0; // for incoming serial data
    // read the incoming byte:
    switch (state)
    {
    case VALUE_READED:
      field = ReadInt16(&Serial);
      WriteInt16(&Serial, field);
      Serial.write(FIELD_READED);
      state = FIELD_READED;
      break;
    case FIELD_READED:
      value = ReadInt16(&Serial);
      WriteInt16(&Serial, value);
      Serial.write(VALUE_READED);
      state = VALUE_READED;
      SendToServer(field, value);
      break;
    default:
      break;
    }
    */
  }
}

void SendToServer(unsigned int field, int value)
{
    // Write value to Field 1 of a ThingSpeak Channel
  int httpCode = ThingSpeak.writeField(myChannelNumber, field, value, myWriteAPIKey);

  if (httpCode == 200) {
    Serial.println("Write_successful");
  }
  else {
    //Serial.println("Problem writing to channel. HTTP error code " + String(httpCode));
    Serial.print("HTTP_error ");
    Serial.println(httpCode);
  }

  // Wait 20 seconds to update the channel again
  delay(20000);
}

int ReadInt16(Stream* stream)
{
    stream->readBytes(readInt16Buff ,2);
    int i = readInt16Buff[1] | readInt16Buff[0] << 8;
    return i;
}

void WriteInt16(Stream* stream, int i)
{
    stream->write(i>>8);
    stream->write(i);
}