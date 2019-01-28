#include "definitions.h"

//////////////////////////////////////////////////////////////
// deviceToName()
// 
// Converts the UCHAR to it's String equivalent.
// -- All were extracted from documents on ANT+ site.
//
//////////////////////////////////////////////////////////////
char * Definitions::deviceToName(UCHAR device) {
	switch (device) {
		default: return "ERROR DEVICE CODE"; break;
		case 11: return "Bicycle Power"; break;
		case 15: return "Multi-Sport"; break;
		case 16: return "Control"; break;
		case 17: return "Fitness"; break;
		case 18: return "Blood Pressure"; break;
		case 19: return "Geocache"; break;
		case 20: return "Electric Vehicle"; break;
		case 25: return "Environment"; break;
		case 26: return "Racquet"; break;
		case 31: return "Muscle Oxygen"; break;
		case 34: return "Shifting"; break;
		case 41: return "Tracker"; break;
		case 115: return "Device Dropseat"; break;
		case 116: return "Suspension"; break;
		case 119: return "Weight Scale"; break;
		case 120: return "Heart Rate"; break;
		case 123: return "Bicycle Speed"; break;
		case 124: return "Stride Speed"; break;
	}
}