#include "initialize.h"

//Static variables
static int LOAD_STATUS = 12345;
static BOOL LOADED = FALSE;
static UCHAR RESPONSE_BUFFER[MAX_RESPONSE_SIZE];
static UCHAR CHANNEL_BUFFER[MAX_CHANNEL_SIZE];
static UCHAR TRANSMIT_BUFFER[ANT_STANDARD_DATA_PAYLOAD_SIZE];

//Can store a maximum of MAX_DEVICES (50) ANT+ devices.
DEVICE * devices[MAX_DEVICES];
int index;

//////////////////////////////////////////////////////////////
// Initialize()
// 
// Initializes the object by setting array all NULL values.
//
//////////////////////////////////////////////////////////////
Initialize::Initialize() {
	cleanDevices();
}

//////////////////////////////////////////////////////////////
// flipStatus()
// 
// Flips the current status of the ANT+ device.
// -- Turns off if on.
// -- Turns on if off.
//
//////////////////////////////////////////////////////////////
char * Initialize::flipStatus() {
	if (LOADED) {
		stopANT();
	}
	else {
		startANT();
	}
	return printStatus();
}


//////////////////////////////////////////////////////////////
// getStatus()
// 
// Returns LOAD_STATUS 
// -- Main purpose for public access to LOAD_STATUS.
//
//////////////////////////////////////////////////////////////
int Initialize::getStatus() {
	return LOAD_STATUS;
}

//////////////////////////////////////////////////////////////
// getIndex()
// 
// Returns index 
// -- Main purpose for public access to index.
//
//////////////////////////////////////////////////////////////
int Initialize::getIndex() {
	return index;
}

//////////////////////////////////////////////////////////////
// printStatus()
// 
// Returns state of LOAD_STATUS in String form.
// -- Main purpose for printing to button.
//
//////////////////////////////////////////////////////////////
char * Initialize::printStatus() {
	switch (LOAD_STATUS) {
		case 0: return "Successfully started the USB Device (0x00) and ANT_DLL.dll"; break;
		case 1: return "Successfully Unloaded the ANT+ USB Device"; break;
		case 2: return "Failed to Load/Cannot Find ANT_DLL.dll"; break;
		case 3: return "Failed to start the ANT+ USB Device (0x00)"; break;
		default: return "LOAD_STATUS ERROR."; break;
	}
}

//////////////////////////////////////////////////////////////
// cleanDevices()
// 
// Cleans the devices array to be all NULL.
//
//////////////////////////////////////////////////////////////
void Initialize::cleanDevices() {
	for (int i = 0; i < MAX_DEVICES; i++) {
		devices[i] = NULL;
	}
	index = 0;
}

//////////////////////////////////////////////////////////////
// printDevices()
// 
// Returns the String of the device.
// -- At location i in devices[].
//
//////////////////////////////////////////////////////////////
char * Initialize::printDevices(int i) {
	char buff[100];
	sprintf_s(buff, "\nDeviceID: %d | DeviceType: %d (%s)\n", devices[i]->num, devices[i]->type, Definitions::deviceToName(devices[i]->type));
	OutputDebugString(buff);
	return buff;
}

//////////////////////////////////////////////////////////////
// searchDevices()
// 
// A method to check if the device has already been found.
// -- Very inefficiently but quick to code.
// -- Doesn't need to be efficient for the small program.
//
//////////////////////////////////////////////////////////////
BOOL Initialize::searchDevices(USHORT deviceNumber) {
	for (int i = 0; i < index; i++) {
		if (devices[i]->num == deviceNumber) {
			return TRUE;
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////
// startANT()
// 
// Start the ANT+ device to start scanning for devices.
//
//////////////////////////////////////////////////////////////
void Initialize::startANT() {
	//Runs the ANT_Load() function from the antlib class.
	//If it fails to load the DLL, returns with error.
	if (!ANT_Load()) {
		LOAD_STATUS = 2;
		return;
	}

	//Initializes the ANT USB at 0x00.
	//If it fails, will update LOAD_STATUS and return an error.
	if (ANT_Init(((UCHAR)USB_NUMBER), ((UCHAR)BAUDRATE))) {
		//Assigns the Response and ChannelEvent buffers.
		//Must set both or it won't start, but we only need ChannelEventFunction realistically.
		ANT_AssignResponseFunction(responseCallback, RESPONSE_BUFFER);
		ANT_AssignChannelEventFunction(ANTCHANNEL, channelCallback, CHANNEL_BUFFER);
		//Restart the ANT+ USB slave device.
		restartANT();
		OutputDebugString("Restarted.\n");
		//Assigns the ANT+ supplied developer key to network slot 0.
		UCHAR key[8] = NETWORK_KEY;
		ANT_SetNetworkKey(NETWORK_NUM, key);
		OutputDebugString("Network key set.\n");
		//Assigns the channel to the ANT+ slave device.
		ANT_AssignChannel(ANTCHANNEL, 0, NETWORK_NUM);
		OutputDebugString("Assigned channel.\n");
		//Sets the channel ID on the ANT+ slave device.
		ANT_SetChannelId(ANTCHANNEL, DEVICENUM, DEVICETYPE, TRANSTYPE);
		OutputDebugString("Set channel ID.\n");
		//Sets the radio frequency to the ANT+ frequency.
		ANT_SetChannelRFFreq(ANTCHANNEL, RADIOFREQ);
		OutputDebugString("Set radio frequency.\n");
		//Opens the channel to listen for master devices.
		ANT_OpenChannel(ANTCHANNEL);
		OutputDebugString("Opened channel.\n");
		//Flips the LOADED boolean, changes LOAD_STATUS and returns.
		ANT_RxExtMesgsEnable(TRUE);
		OutputDebugString("Extended messages enabled.\n");
		LOADED = TRUE;
		LOAD_STATUS = 0;
	} 
	else {
		LOAD_STATUS = 3;
	}
}

//////////////////////////////////////////////////////////////
// stopANT()
// 
// Stop the ANT+ device currently scanning for devices.
//
//////////////////////////////////////////////////////////////
void Initialize::stopANT() {
	//Unassigns the channel.
	ANT_UnAssignChannel(ANTCHANNEL);
	//Closes the open ANT+ device.
	ANT_Close();
	//Updates LOAD_STATUS  and LOADED.
	LOAD_STATUS = 1;
	LOADED = FALSE;
}

//////////////////////////////////////////////////////////////
// restartANT()
// 
// Stop the ANT+ device currently scanning for devices.
//
//////////////////////////////////////////////////////////////
void Initialize::restartANT() {
	ANT_ResetSystem();
	//Naps/delays to allow proper re-linking of device.
	ANT_Nap(1000);
}

//////////////////////////////////////////////////////////////
// channelCallback(UCHAR ucChannel_, UCHAR ucEvent_)
// 
// - For receiving the data sent from ANT+ master devices.
// --- Lots of potential events, but we only need two.
//
//////////////////////////////////////////////////////////////
BOOL Initialize::channelCallback(UCHAR ucChannel_, UCHAR ucEvent_) {
	switch (ucEvent_) {
		//Case for when we receive actual data from the ANT+ master device.
		//Tested using a geocaching ANT+ device.
		case EVENT_RX_FLAG_BROADCAST:
		{
			UCHAR flag = CHANNEL_BUFFER[9];
			if (flag  & ANT_EXT_MESG_BITFIELD_DEVICE_ID) {
				//Saves number, type and transmission type of the ANT+ master device.
				USHORT deviceNumber = CHANNEL_BUFFER[10] | (CHANNEL_BUFFER[11] << 8);
				UCHAR deviceType = CHANNEL_BUFFER[12];
				UCHAR transType = CHANNEL_BUFFER[13];
				//Checks if it is already in the array.
				if (searchDevices(deviceNumber)) {
					OutputDebugString("Device found, but already in database.\n");
				}
				else {
					//Buffer for the packets.
					char buff[50];
					sprintf_s(buff, "[%02x],[%02x],[%02x],[%02x],[%02x],[%02x],[%02x],[%02x]\n",
						CHANNEL_BUFFER[1],
						CHANNEL_BUFFER[2],
						CHANNEL_BUFFER[3],
						CHANNEL_BUFFER[4],
						CHANNEL_BUFFER[5],
						CHANNEL_BUFFER[6],
						CHANNEL_BUFFER[7],
						CHANNEL_BUFFER[8]);
					//Outputs the raw packets to the debug window.
					OutputDebugString(buff);
					//Creates a new DEVICE to add to the array.
					DEVICE *d = new DEVICE;
					d->num = deviceNumber;
					d->type = deviceType;
					d->trans = transType;
					//Adds it to the array, index goes up by one.
					devices[index] = d;
					index++;
				}
				
			}
			break;
		}
		//Case for when receiving of packets fails.
		case EVENT_RX_FAIL:
		{
			OutputDebugString("EVENT_RX_FAIL.\n");
			break;
		}
		//Default case for when the ANT+ device can no longer find the master devices.
		default:
		{
			OutputDebugString("NOTHING FOUND.\n");
		}
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////
// responseCallback(UCHAR ucChannel_, UCHAR ucMessageId_)
// 
// - Would be used for when the ANT+ USB device needs to respond.
// --- Since we are just scanning for devices, can be empty.
// --- Needed for proper initialization of ANT+ device, though.
//
//////////////////////////////////////////////////////////////
BOOL Initialize::responseCallback(UCHAR ucChannel_, UCHAR ucMessageId_) {
	return TRUE;
}