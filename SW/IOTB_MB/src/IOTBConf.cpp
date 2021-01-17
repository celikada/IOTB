// 
// 
// 

#include <IOTBConf.h>

void IOTBConfClass::read()
{
	EEPROM.get(0, IOTBConf);
}

void IOTBConfClass::init()
{
	IOTBConf.Status = 1;
	IOTBConf.Version = 0;
	
	IOTBConf.SerialNum = 1;
	
	IOTBConf.IP1 = 10;
	IOTBConf.IP2 = 0;
	IOTBConf.IP3 = 0;
	IOTBConf.IP4 = 100;
	
	IOTBConf.MASK1 = 255;
	IOTBConf.MASK2 = 255;
	IOTBConf.MASK3 = 255;
	IOTBConf.MASK4 = 0;
	
	IOTBConf.GW1 = 10;
	IOTBConf.GW2 = 0;
	IOTBConf.GW3 = 0;
	IOTBConf.GW4 = 1;
	
	IOTBConf.DNS1 = 208;
	IOTBConf.DNS2 = 67;
	IOTBConf.DNS3 = 220;
	IOTBConf.DNS4 = 220;
	
	IOTBConf.IPS1 = 10;
	IOTBConf.IPS2 = 0;
	IOTBConf.IPS3 = 0;
	IOTBConf.IPS4 = 99;
	
	//IOTBConf.CommPort = 1883;
	IOTBConf.CommPortH = 502>>8;
	IOTBConf.CommPortL = (uint8_t) 502;

	IOTBConf.VolatileOut = 0;

	save();
}

unsigned int IOTBConfClass::GetPort()
{
	unsigned int pnL = (int)IOTBConf.CommPortL & 0x000000ff;
	unsigned int pnH = (int)IOTBConf.CommPortH << 8 & 0x0000ff00;
	unsigned int portNum = pnL | pnH;
	return portNum;
}

void IOTBConfClass::save()
{
	EEPROM.put(0, IOTBConf);
	//resetFunc();  //call reset
}

// //sıkıntı olabilir............!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// String IOTBConfClass::GetSerialID()
// {
// 	char str[8];
// 	sprintf(str, "%s%04d\0", IOTBConf.SerialText, IOTBConf.SerialNum);
// 	String sym(str);
// 	return sym;
// }

// String IOTBConfClass::GetTagID(char* tag)
// {
// 	char str[20];
// 	sprintf(str, "%s/%s\0", GetSerialID().c_str(), tag);
// 	String sym(str);
// 	return sym;
// }


IOTBConfClass IOTBConf;

