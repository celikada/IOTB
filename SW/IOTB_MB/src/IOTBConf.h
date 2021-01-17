// IOTBConf.h

#ifndef _IOTBCONF_h
#define _IOTBCONF_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
#else
	#include "WProgram.h"
#endif

//EEPROM 
#include <EEPROM.h>


struct IOTBConfOBJ {
	uint8_t Status;
	uint8_t Version;//Seri başlangıcı örn: V00.00
	/*
	*İlgili seride kaç numaralı kart olduğu bu aynı zamanda broker'a publis ederken kullanılacak.
	*örn: client.publish("KMO0001/humidity", buffer);
	*/
	//int SerialText;//Seri başlangıcı örn: KMO
	/*
	*İlgili seride kaç numaralı kart olduğu bu aynı zamanda broker'a publis ederken,
	*ip ve mac adresinde kullanılacak.
	*örn: client.publish("KMO0001/humidity", buffer);
	*IP son parçada(IP blok 4. kısım): 192.168.3.1
	*/
	// İlgili seride kaç numaralı kart olduğu bu aynı zamanda broker'a publis ederken
	uint8_t SerialNum;
	
	uint8_t IP1;//IP blok 1. kısım(Server ve IOTB için)
	uint8_t IP2;//IP blok 2. kısım(Server ve IOTB için)
	uint8_t IP3;//IP blok 3. kısım(Server ve IOTB için)
	uint8_t IP4;//IP blok 3. kısım(Server ve IOTB için)

	uint8_t MASK1;//IP blok 1. kısım(Server ve IOTB için)
	uint8_t MASK2;//IP blok 2. kısım(Server ve IOTB için)
	uint8_t MASK3;//IP blok 3. kısım(Server ve IOTB için)
	uint8_t MASK4;//IP blok 3. kısım(Server ve IOTB için)

	uint8_t GW1;//IP blok 1. kısım(Server ve IOTB için)
	uint8_t GW2;//IP blok 2. kısım(Server ve IOTB için)
	uint8_t GW3;//IP blok 3. kısım(Server ve IOTB için)
	uint8_t GW4;//IP blok 3. kısım(Server ve IOTB için)

	uint8_t DNS1;//DNS blok 1. kısım(Server ve IOTB için)
	uint8_t DNS2;//DNS blok 2. kısım(Server ve IOTB için)
	uint8_t DNS3;//DNS blok 3. kısım(Server ve IOTB için)
	uint8_t DNS4;//DNS blok 3. kısım(Server ve IOTB için)

	uint8_t IPS1;//DNS blok 1. kısım(Server ve IOTB için)
	uint8_t IPS2;//DNS blok 2. kısım(Server ve IOTB için)
	uint8_t IPS3;//DNS blok 3. kısım(Server ve IOTB için)
	uint8_t IPS4;//DNS blok 3. kısım(Server ve IOTB için)

	uint8_t CommPortH;//Server Comminication Port Hight Byte
	uint8_t CommPortL;//Server Comminication Port Low Byte

	uint8_t VolatileOut;//Output Volatility (1 Volatile)
};




class IOTBConfClass
{
private:
	//IOTBConfClass();
	
	
 protected:


 public:
 	uint8_t Reset = 0; //1 reset
 	unsigned int GetPort();
	struct IOTBConfOBJ IOTBConf;
	void init();
	void read();
	void save();
	 
	//  String GetSerialID();
	//  String GetTagID(char* tag);
	static IOTBConfClass* getInstance() 
	{
		static IOTBConfClass hw;
		return &hw;
	};
};

extern IOTBConfClass IOTBConf;

#endif

