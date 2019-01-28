#pragma once

#include "types.h"
#include "libant.h"
#include "antdefines.h"
#include "antmessage.h"
#include "definitions.h"

#include <stdio.h>
#include <string.h>


#define USB_NUMBER		(0)		//Always use USB device 0.
#define CHANNEL_TYPE	(1)		//Slave device.
#define BAUDRATE		(57600) //The BaudRate for a USB2 ANT+ stick.
#define RADIOFREQ		(57)	//The radiofrequency is RADIOFREQ + 2400 (2457mhz) which is reserved for ANT+ devices.
#define ANTCHANNEL		(0)		//The channel to listen to.
#define DEVICENUM		(0)		//Device number 0 i for wildcard searching.
#define DEVICETYPE		(0)		//Device type 19 (0x13) is for Geocaching devices. 0 *should* be for wildcard searching.
#define TRANSTYPE		(0)		//According to specsheet, must be set to 0.
#define NETWORK_KEY		{0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45}	//ANT+ supplied private network key for ANT+ devices.
#define NETWORK_NUM		(0)		//USB supports 8 network channels, assign key to network 0.
#define MESSAGE_TIMEOUT	(3000)	//3000ms, 3 seconds.

#define MAX_DEVICES		(50)	//Max amount of devices that the analyzer/scanner will find and display.
#define MAX_CHANNEL_SIZE	(MESG_MAX_SIZE_VALUE)
#define MAX_RESPONSE_SIZE	(MESG_MAX_SIZE_VALUE)

typedef struct Device {
	USHORT num;
	UCHAR type;
	UCHAR trans;
} DEVICE;

class Initialize {
public:
	Initialize();
	char * flipStatus();
	char * printStatus();
	char * printDevices(int i);
	void cleanDevices();
	int getStatus();
	int getIndex();
	
private:
	void startANT();
	void stopANT();
	void restartANT();
	static BOOL searchDevices(USHORT deviceNumber);
	static BOOL Initialize::channelCallback(UCHAR ucChannel_, UCHAR ucEvent_);
	static BOOL Initialize::responseCallback(UCHAR ucChannel_, UCHAR ucMessageId_);
};

