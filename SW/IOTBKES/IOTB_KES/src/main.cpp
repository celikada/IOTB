#include <Arduino.h>

#include <DHT.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define sendinterval 300000

String inString = "";    // string to hold input
unsigned long lastSendToESP = 0;

//NATIVE SENSOR INPUT
constexpr auto  NIT1 = 8; // DHT Sensor
constexpr auto  NIT2 = 9; //DS18S20
constexpr auto  NIT3 = A0;//soil moisto

//INPUTS
constexpr auto PI1 = 0;//Door1 ;
constexpr auto PI2 = 1; //Door2;
constexpr auto PI3 = 2; //leakSensor
constexpr auto PI4 = 3; //
constexpr auto PI5 = 4;//
constexpr auto PI6 = 5; //
constexpr auto PI7 = 6; //
constexpr auto PI8 = 7; //

//OUTPUTS
constexpr auto PQ1 = 8 ;
constexpr auto PQ2 = 9 ;
constexpr auto PQ3 = 10; 
constexpr auto PQ4 = 11;
constexpr auto PQ5 = 12;
constexpr auto PQ6 = 13;
constexpr auto PQ7 = 14;
constexpr auto PQ8 = 15;

//components&sensors
DHT dht(NIT1, DHT22);
SoftwareSerial espLine(3, 4); // RX, TX
Adafruit_MCP23017 mcp;
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(NIT2);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

//GlobalVariable
float DHTTempVal = 0;
float DHTHumVal = 0;
int THMVal = 0;
uint8_t I1Val = 0;
uint8_t I2Val = 0;
uint8_t I3Val = 0;
uint8_t I4Val = 0;
uint8_t I5Val = 0;
uint8_t I6Val = 0;
uint8_t I7Val = 0;
uint8_t I8Val = 0;
int soilMoistureValue = 0;
int soilTempValue = 0;

//soil moisto cal const
const int AirValue = 520;   
const int WaterValue = 260;  
int intervals = (AirValue - WaterValue)/3;  
//

void chkEspLine();

void initDHT();
void mcpInit();

void readDHT();
void ReadCardTHM();
void ReadDigiInputs();
void ReadDigiOutputs();
void readMoisto();
void readSoilTemp();

void SendToEsp();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  espLine.begin(9600);
  sensors.begin();
	initDHT();
  mcpInit();
  lastSendToESP = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  /*mcp.digitalWrite(PQ1,HIGH);
	mcp.digitalWrite(PQ2,HIGH);
	mcp.digitalWrite(PQ3,HIGH);
	mcp.digitalWrite(PQ4,HIGH);
	mcp.digitalWrite(PQ5,HIGH);
	mcp.digitalWrite(PQ6,HIGH);
	mcp.digitalWrite(PQ7,HIGH);
	mcp.digitalWrite(PQ8,HIGH);
	delay(1000);

	mcp.digitalWrite(PQ1,LOW);
	mcp.digitalWrite(PQ2,LOW);
	mcp.digitalWrite(PQ3,LOW);
	mcp.digitalWrite(PQ4,LOW);
	mcp.digitalWrite(PQ5,LOW);
	mcp.digitalWrite(PQ6,LOW);
	mcp.digitalWrite(PQ7,LOW);
	mcp.digitalWrite(PQ8,LOW);
	
	delay(1000);
*/
  chkEspLine();

	readDHT();
  ReadCardTHM();
  ReadDigiInputs();
  ReadDigiOutputs();
  readMoisto();
  readSoilTemp();

  long countdown = (sendinterval + lastSendToESP - millis())/1000;
  Serial.print("Countdown ");
  Serial.println(countdown);
  Serial.println("------------------------------");
  SendToEsp();
  delay(2000);
}

void chkEspLine()
{
  while (espLine.available() > 0) {
    int inChar = espLine.read();
    // convert the incoming byte to a char and add it to the string:
    inString += (char)inChar;
    
    // if you get a newline, print the string, then the string's value:
    if (inChar == '\n') {
      Serial.print("ESP LINE :");
      Serial.println(inString);
      // clear the string for new input:
      inString = "";
    }
  }
}

//------------------------------------------------------------------------
void initDHT()
{
  dht.begin();
}

void mcpInit()
{
	mcp.begin(1);      // Default device address 0
    //mcp.begin(0x21);      // Default device address 0

	mcp.pinMode(PI1, INPUT);
	mcp.pinMode(PI2, INPUT);
	mcp.pinMode(PI3, INPUT);
	mcp.pinMode(PI4, INPUT);
	mcp.pinMode(PI5, INPUT);
	mcp.pinMode(PI6, INPUT);
	mcp.pinMode(PI7, INPUT);
	mcp.pinMode(PI8, INPUT);

	mcp.pinMode(PQ1, OUTPUT);     // LED output
	mcp.pinMode(PQ2, OUTPUT);     // LED output
	mcp.pinMode(PQ3, OUTPUT);     // LED output
	mcp.pinMode(PQ4, OUTPUT);     // LED output
	mcp.pinMode(PQ5, OUTPUT);     // LED output
	mcp.pinMode(PQ6, OUTPUT);     // LED output
	mcp.pinMode(PQ7, OUTPUT);     // LED output
	mcp.pinMode(PQ8, OUTPUT);     // LED output
}

//------------------------------------------------------------------------
void readDHT()
{
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = DHTHumVal = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = DHTTempVal = dht.readTemperature();

  Serial.print(F("Humidity:         "));
  Serial.print(h);
  Serial.println(F("% "));
  Serial.print(F("Temperature:      "));
  Serial.print(t);
  Serial.println(F("°C "));

}
void ReadCardTHM()
{
	int Vo;
	float R1 = 10000;
	float logR2, R2, T;
	float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;


	Vo = analogRead(A7);
  	R2 = R1 * (1023.0 / (float)Vo - 1.0);
  	logR2 = log(R2);
  	T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  	T = T - 273.15;

	THMVal = T;
  Serial.print(F("IOTB Temperature: "));
  Serial.print(T);
  Serial.println(F("°C "));
}

void ReadDigiInputs()
{
	I1Val = mcp.digitalRead(PI1);
	I2Val = mcp.digitalRead(PI2);
	I3Val = mcp.digitalRead(PI3);
	I4Val = mcp.digitalRead(PI4);
	I5Val = mcp.digitalRead(PI5);
	I6Val = mcp.digitalRead(PI6);
	I7Val = mcp.digitalRead(PI7);
	I8Val = mcp.digitalRead(PI8);

  Serial.print("INPUT PORTS:  [");
  Serial.print(I1Val);
  Serial.print(" ,");
  Serial.print(I2Val);
  Serial.print(" ,");
  Serial.print(I3Val);
  Serial.print(" ,");
  Serial.print(I4Val);
  Serial.print(" ,");
  Serial.print(I5Val);
  Serial.print(" ,");
  Serial.print(I6Val);
  Serial.print(" ,");
  Serial.print(I7Val);
  Serial.print(" ,");
  Serial.print(I8Val);
  Serial.println(" ]");
}

void ReadDigiOutputs()
{
  Serial.print("OUTPUT PORTS: [");
  Serial.print(mcp.digitalRead(PQ1));
  Serial.print(" ,");
  Serial.print(mcp.digitalRead(PQ2));
  Serial.print(" ,");
  Serial.print(mcp.digitalRead(PQ3));
  Serial.print(" ,");
  Serial.print(mcp.digitalRead(PQ4));
  Serial.print(" ,");
  Serial.print(mcp.digitalRead(PQ5));
  Serial.print(" ,");
  Serial.print(mcp.digitalRead(PQ6));
  Serial.print(" ,");
  Serial.print(mcp.digitalRead(PQ7));
  Serial.print(" ,");
  Serial.print(mcp.digitalRead(PQ8));
  Serial.println(" ]");
}

void readMoisto()
{
	soilMoistureValue = analogRead(NIT3);  //put Sensor insert into soil
  Serial.print("Soil Moisture Value: ");
  Serial.println(soilMoistureValue);  
  
	if(soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + intervals)) 
	{
		 Serial.println("Very Wet"); 
	} 
	else if(soilMoistureValue > (WaterValue + intervals) && soilMoistureValue < (AirValue - intervals))
	{
  		Serial.println("Wet");
	}
	else if(soilMoistureValue < AirValue && soilMoistureValue > (AirValue - intervals))
	{
  		Serial.println("Dry");
	}
	delay(100);
}

void readSoilTemp()
{
  sensors.requestTemperatures();
	float celsius = sensors.getTempCByIndex(0);
  soilTempValue = celsius;
  Serial.print("Soil Temperature is: ");
	Serial.println(sensors.getTempCByIndex(0)); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
}



//------------------------------------------------------------------------
/*
Field 1 ambiance_temp
Field 2 ambiance_hum
Field 3 soil_temp
Field 4 soil_hum
Field 5 iotb_temp
Field 6 pump_relay
Field 7 water_level
*/

bool SendTSFrame(int frame, int value)
{
  long msg = value*10+frame;
  Serial.print("SendTSFrame : ");
  Serial.println(msg);
  espLine.println(msg);
  chkEspLine();
  return true;
}

void SendToEsp()
{
  if (millis() > lastSendToESP + 300000) {
    lastSendToESP = millis();
    
    SendTSFrame(1, DHTTempVal);
    /**/
    delay(2000);
    SendTSFrame(2, DHTHumVal);
    delay(2000);
    SendTSFrame(3, soilTempValue);
    delay(2000);
    SendTSFrame(4, soilMoistureValue);
    delay(2000);
    SendTSFrame(5, THMVal);
    delay(2000);
    SendTSFrame(6, mcp.digitalRead(PQ1));
    delay(2000);
    SendTSFrame(7, I1Val);
    
  }
  
}

