/*                                                                                                                                                                                                      
        Name:       IOTB Modbus                                                                                                                                                                      
        Created:    17/1/2021 2:17:06 PM                                                                                                                                                             
        Author:     Ozan Celikada                                                                                                                                                            
*/

#include <Wire.h>
#include <SPI.h>
#include <Ethernet2.h> 

#include <ArduinoRS485.h> // ArduinoModbus depends on the ArduinoRS485 library
#include <ArduinoModbus.h>

#include <IOTBConf.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_SHT31.h>
#include <SoftwareSerial.h>

int keyIndex = 0;                 // your network key Index number (needed only for WEP)

const int ledPin = LED_BUILTIN;

EthernetServer ethenet_server = EthernetServer(502);
ModbusTCPServer modbusTCPServer;

//Wifi to serial
SoftwareSerial espLine(4, 3); // RX, TX


//NATIVE SENSOR INPUT
constexpr auto  NIT1 = 8; // DHT Sensor
constexpr auto  NIT2 = 9;
constexpr auto  NIT3 = A0;

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

//Devices
Adafruit_SHT31 sht31 = Adafruit_SHT31();
Adafruit_MCP23017 mcp;

//Conf
IOTBConfClass* IOTBConfC = IOTBConfClass::getInstance();

void serialInit();
void confInit();
void ethernetInit();
void modbusInit();
void mcpInit();
void shtInit();

void readCoilRegsToOutputs();
void readInputsToInputReg();
void readHoldingRegsToConfig();
int readCardTHM();




void setup() {
	pinMode(PC13, OUTPUT);  
	digitalWrite(PC13, HIGH);   // turn the LED on (HIGH is the voltage level)                                                                                                                            
  	delay(500);              // wait for a second                                                                                                                                                         
  	digitalWrite(PC13, LOW);    // turn the LED off by making the voltage LOW                                                                                                                             
  	delay(500);              // wait for a second          
	digitalWrite(PC13, HIGH);   // turn the LED on (HIGH is the voltage level)                                                                                                                            
  	delay(500);              // wait for a second                                                                                                                                                         
  	digitalWrite(PC13, LOW);    // turn the LED off by making the voltage LOW                                                                                                                             
  	delay(500);              // wait for a second          
	digitalWrite(PC13, HIGH);   // turn the LED on (HIGH is the voltage level)                                                                                                                            
  	delay(500);              // wait for a second                                                                                                                                                         
  	digitalWrite(PC13, LOW);    // turn the LED off by making the voltage LOW                                                                                                                             
  	delay(500);              // wait for a second          




	//serialInit();//portlar yanlış olabilir çakılıyor
	confInit();
  	ethernetInit();
	modbusInit();
	mcpInit();
	shtInit();
	//volatilityInit();
}

void loop() {
  	// listen for incoming clients
  	EthernetClient client = ethenet_server.available();
  
  	if (client) {
    // a new client connected
    // let the Modbus TCP accept the connection 
    	modbusTCPServer.accept(client);

    	while (client.connected()) {
      	// poll for Modbus TCP requests, while client connected
      	modbusTCPServer.poll();
		//update IO and others
		readCoilRegsToOutputs();
		readInputsToInputReg();
		readHoldingRegsToConfig();
    }
  }
}

//------------------------------------------------------
void serialInit()
{
	Serial.begin(9600);
	espLine.begin(115200);
}

void shtInit()
{
	sht31.begin(0x44);
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

	// mcp.pullUp(PI1, HIGH);     // Puled high to ~100k
	// mcp.pullUp(PI2, HIGH);     // Puled high to ~100k
	// mcp.pullUp(PI3, HIGH);     // Puled high to ~100k
	// mcp.pullUp(PI4, HIGH);     // Puled high to ~100k
	// mcp.pullUp(PI5, HIGH);     // Puled high to ~100k
	// mcp.pullUp(PI6, HIGH);     // Puled high to ~100k
	// mcp.pullUp(PI7, HIGH);     // Puled high to ~100k
	// mcp.pullUp(PI8, HIGH);     // Puled high to ~100k

	mcp.pinMode(PQ1, OUTPUT);     // LED output
	mcp.pinMode(PQ2, OUTPUT);     // LED output
	mcp.pinMode(PQ3, OUTPUT);     // LED output
	mcp.pinMode(PQ4, OUTPUT);     // LED output
	mcp.pinMode(PQ5, OUTPUT);     // LED output
	mcp.pinMode(PQ6, OUTPUT);     // LED output
	mcp.pinMode(PQ7, OUTPUT);     // LED output
	mcp.pinMode(PQ8, OUTPUT);     // LED output

	// for (int i = 8; i < 16; i++)
	// {
	// 	mcp.digitalWrite(i, LOW);
	// }
}

void confInit()
{
	IOTBConfC->read();
	if(IOTBConfC->IOTBConf.Status != 1)
	{
		IOTBConfC->init();
	}
}

void ethernetInit()
{
  
	byte mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
	mac[5] = IOTBConfC->IOTBConf.SerialNum;

	IPAddress ip = IPAddress(IOTBConfC->IOTBConf.IP1, IOTBConfC->IOTBConf.IP2, IOTBConfC->IOTBConf.IP3, IOTBConfC->IOTBConf.IP4);
	IPAddress dns = IPAddress(IOTBConfC->IOTBConf.DNS1, IOTBConfC->IOTBConf.DNS2, IOTBConfC->IOTBConf.DNS3, IOTBConfC->IOTBConf.DNS4);
	IPAddress gateway = IPAddress(IOTBConfC->IOTBConf.GW1, IOTBConfC->IOTBConf.GW2, IOTBConfC->IOTBConf.GW3, IOTBConfC->IOTBConf.GW4);
	IPAddress subnet = IPAddress(IOTBConfC->IOTBConf.MASK1, IOTBConfC->IOTBConf.MASK2, IOTBConfC->IOTBConf.MASK3, IOTBConfC->IOTBConf.MASK4);

	//Ethernet
  	Ethernet.init(PA4);
	Ethernet.begin(mac, ip, dns, gateway, subnet);
	//analogReference(DEFAULT);

  	int portNum = IOTBConfC->GetPort();
  	if(portNum != 502)
	{
		
	}
  	ethenet_server.begin();
}

void modbusInit()
{
  	// start the Modbus TCP server
  	if (!modbusTCPServer.begin()) {
  	  //Serial.println("Failed to start Modbus TCP Server!");
  	  while (1);
  	}

	//Configuration
  	modbusTCPServer.configureHoldingRegisters(0x00, 27);
  	modbusTCPServer.configureInputRegisters(0x00, 14);
  	modbusTCPServer.configureCoils(0x00, 8);

	//Holding
  	modbusTCPServer.holdingRegisterWrite(0, IOTBConfC->IOTBConf.Status);
  	modbusTCPServer.holdingRegisterWrite(1, IOTBConfC->IOTBConf.Version);
  	modbusTCPServer.holdingRegisterWrite(2, IOTBConfC->IOTBConf.SerialNum); 

  	modbusTCPServer.holdingRegisterWrite(3, IOTBConfC->IOTBConf.IP1);
  	modbusTCPServer.holdingRegisterWrite(4, IOTBConfC->IOTBConf.IP2);
  	modbusTCPServer.holdingRegisterWrite(5, IOTBConfC->IOTBConf.IP3);
  	modbusTCPServer.holdingRegisterWrite(6, IOTBConfC->IOTBConf.IP4);
	
  	modbusTCPServer.holdingRegisterWrite(7, IOTBConfC->IOTBConf.MASK1);
  	modbusTCPServer.holdingRegisterWrite(8, IOTBConfC->IOTBConf.MASK2);
  	modbusTCPServer.holdingRegisterWrite(9, IOTBConfC->IOTBConf.MASK3);
  	modbusTCPServer.holdingRegisterWrite(10, IOTBConfC->IOTBConf.MASK4);
	
  	modbusTCPServer.holdingRegisterWrite(11, IOTBConfC->IOTBConf.GW1);
  	modbusTCPServer.holdingRegisterWrite(12, IOTBConfC->IOTBConf.GW2);
  	modbusTCPServer.holdingRegisterWrite(13, IOTBConfC->IOTBConf.GW3);
  	modbusTCPServer.holdingRegisterWrite(14, IOTBConfC->IOTBConf.GW4);
	
  	modbusTCPServer.holdingRegisterWrite(15, IOTBConfC->IOTBConf.DNS1);
  	modbusTCPServer.holdingRegisterWrite(16, IOTBConfC->IOTBConf.DNS2);
  	modbusTCPServer.holdingRegisterWrite(17, IOTBConfC->IOTBConf.DNS3);
  	modbusTCPServer.holdingRegisterWrite(18, IOTBConfC->IOTBConf.DNS4);
	
  	modbusTCPServer.holdingRegisterWrite(19, IOTBConfC->IOTBConf.IPS1);
  	modbusTCPServer.holdingRegisterWrite(20, IOTBConfC->IOTBConf.IPS2);
  	modbusTCPServer.holdingRegisterWrite(21, IOTBConfC->IOTBConf.IPS3);
  	modbusTCPServer.holdingRegisterWrite(22, IOTBConfC->IOTBConf.IPS4);
	
  	modbusTCPServer.holdingRegisterWrite(23, IOTBConfC->IOTBConf.CommPortH); 
  	modbusTCPServer.holdingRegisterWrite(24, IOTBConfC->IOTBConf.CommPortL); 
	
  	modbusTCPServer.holdingRegisterWrite(25, IOTBConfC->IOTBConf.VolatileOut);
	
  	modbusTCPServer.holdingRegisterWrite(26, IOTBConfC->Reset);
}

//----------------------------------------------------------------------
void readCoilRegsToOutputs()
{
	mcp.digitalWrite(PQ1, modbusTCPServer.coilRead(0x00));
	mcp.digitalWrite(PQ2, modbusTCPServer.coilRead(0x01));
	mcp.digitalWrite(PQ3, modbusTCPServer.coilRead(0x02));
	mcp.digitalWrite(PQ4, modbusTCPServer.coilRead(0x03));
	mcp.digitalWrite(PQ5, modbusTCPServer.coilRead(0x04));
	mcp.digitalWrite(PQ6, modbusTCPServer.coilRead(0x05));
	mcp.digitalWrite(PQ7, modbusTCPServer.coilRead(0x06));
	mcp.digitalWrite(PQ8, modbusTCPServer.coilRead(0x07));
}

void readInputsToInputReg()
{	
	u_int8_t coils = modbusTCPServer.coilRead(0x00) & 0x0001;
	coils |= ((modbusTCPServer.coilRead(0x01) & 0x0001) << 1);
	coils |= ((modbusTCPServer.coilRead(0x02) & 0x0001) << 2);
	coils |= ((modbusTCPServer.coilRead(0x03) & 0x0001) << 3);
	coils |= ((modbusTCPServer.coilRead(0x04) & 0x0001) << 4);
	coils |= ((modbusTCPServer.coilRead(0x05) & 0x0001) << 5);
	coils |= ((modbusTCPServer.coilRead(0x06) & 0x0001) << 6);
	coils |= ((modbusTCPServer.coilRead(0x07) & 0x0001) << 7);

	modbusTCPServer.inputRegisterWrite(0, mcp.digitalRead(PI1));
	modbusTCPServer.inputRegisterWrite(1, mcp.digitalRead(PI2));
	modbusTCPServer.inputRegisterWrite(2, mcp.digitalRead(PI3));
	modbusTCPServer.inputRegisterWrite(3, mcp.digitalRead(PI4));
	modbusTCPServer.inputRegisterWrite(4, mcp.digitalRead(PI5));
	modbusTCPServer.inputRegisterWrite(5, mcp.digitalRead(PI6));
	modbusTCPServer.inputRegisterWrite(6, mcp.digitalRead(PI7));
	modbusTCPServer.inputRegisterWrite(7, mcp.digitalRead(PI8));

	modbusTCPServer.inputRegisterWrite(8, coils);
	
	modbusTCPServer.inputRegisterWrite(9, sht31.readTemperature());
	modbusTCPServer.inputRegisterWrite(10, sht31.readHumidity());

	modbusTCPServer.inputRegisterWrite(11, readCardTHM());
	//other inputs***********
	modbusTCPServer.inputRegisterWrite(12, 0);
	modbusTCPServer.inputRegisterWrite(13, 0);

}

void readHoldingRegsToConfig()
{
	if(modbusTCPServer.holdingRegisterRead(26) == 1)
	{
		IOTBConfC->IOTBConf.Status		= modbusTCPServer.holdingRegisterRead(0);
		IOTBConfC->IOTBConf.Version 	= modbusTCPServer.holdingRegisterRead(1);
		IOTBConfC->IOTBConf.SerialNum  	= modbusTCPServer.holdingRegisterRead(2);
		IOTBConfC->IOTBConf.IP1 		= modbusTCPServer.holdingRegisterRead(3);
		IOTBConfC->IOTBConf.IP2 		= modbusTCPServer.holdingRegisterRead(4);
		IOTBConfC->IOTBConf.IP3 		= modbusTCPServer.holdingRegisterRead(5);
		IOTBConfC->IOTBConf.IP4 		= modbusTCPServer.holdingRegisterRead(6);
		IOTBConfC->IOTBConf.MASK1 		= modbusTCPServer.holdingRegisterRead(7);
		IOTBConfC->IOTBConf.MASK2 		= modbusTCPServer.holdingRegisterRead(8);
		IOTBConfC->IOTBConf.MASK3 		= modbusTCPServer.holdingRegisterRead(9);
		IOTBConfC->IOTBConf.MASK4 		= modbusTCPServer.holdingRegisterRead(10);
		IOTBConfC->IOTBConf.GW1 		= modbusTCPServer.holdingRegisterRead(11);
		IOTBConfC->IOTBConf.GW2 		= modbusTCPServer.holdingRegisterRead(12);
		IOTBConfC->IOTBConf.GW3 		= modbusTCPServer.holdingRegisterRead(13);
		IOTBConfC->IOTBConf.GW4 		= modbusTCPServer.holdingRegisterRead(14);
		IOTBConfC->IOTBConf.DNS1 		= modbusTCPServer.holdingRegisterRead(15);
		IOTBConfC->IOTBConf.DNS2 		= modbusTCPServer.holdingRegisterRead(16);
		IOTBConfC->IOTBConf.DNS3 		= modbusTCPServer.holdingRegisterRead(17);
		IOTBConfC->IOTBConf.DNS4 		= modbusTCPServer.holdingRegisterRead(18);
		IOTBConfC->IOTBConf.IPS1 		= modbusTCPServer.holdingRegisterRead(19);
		IOTBConfC->IOTBConf.IPS2 		= modbusTCPServer.holdingRegisterRead(20);
		IOTBConfC->IOTBConf.IPS3 		= modbusTCPServer.holdingRegisterRead(21);
		IOTBConfC->IOTBConf.IPS4 		= modbusTCPServer.holdingRegisterRead(22);
		IOTBConfC->IOTBConf.CommPortH 	= modbusTCPServer.holdingRegisterRead(23);
		IOTBConfC->IOTBConf.CommPortL 	= modbusTCPServer.holdingRegisterRead(24);
		IOTBConfC->IOTBConf.VolatileOut = modbusTCPServer.holdingRegisterRead(25);
		IOTBConfC->Reset				= 0;
		IOTBConfC->save();
		NVIC_SystemReset();
	}
}

int readCardTHM()
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
	return T;
}
