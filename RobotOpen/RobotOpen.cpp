/*
  RobotOpen.cpp - Library implementation of the RobotOpen Hardware found at www.RobotOpen.biz
  Created by Eric Barch, October 16, 2011.
*/

#include "Arduino.h"
#include "RobotOpen.h"
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <avr/pgmspace.h>

/* DEVICE IDs - Driver Stations */
#define OFFICIAL_JAVA_DS 0x01
#define OFFICIAL_ANDROID_APP 0x02
#define OFFICIAL_IPHONE_APP 0x03
            
/* DEVICE IDs - Robot Controllers */
#define ROBOTOPEN_V1 0xFF
        
/* MESSAGE TYPES */
#define CONTROL_PACKET 0x01
#define FEEDBACK_PACKET 0x02
#define QUERY 0x03

/* APPLICATION PARAMETERS */
#define PROTOCOL_VER 0x02
#define DEVICE_ID 0xFF
#define FIRMWARE_VER 0x01

/* CRC lookup table */
PROGMEM static short crctab[] =
{
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

//Set the MAC address and static IP for the TCP/IP stack
static byte mac[] = { 0xF2, 0x7D, 0xBB, 0x86, 0x45, 0x7C };
static byte ip[]  = { 192, 168, 1, 22 };
#define PORT 22211 //Port of incoming UDP data (default)

// Store the data here
static unsigned char _packetBuffer[256];  // Byte array for incoming data
static unsigned char _validPacket[256];    // Byte array of valid data
static unsigned char _outgoingPacket[256];  // Data to publish to DS is stored into this array
static unsigned char _pwmSerialData[14];
static unsigned char _relaySerialData[14];
static unsigned char *_packetBufferAccessor;
static unsigned char *_validPacketAccessor;
static unsigned int _packetBufferSize = 0;
static unsigned int _validPacketSize = 0;
static unsigned int _outgoingPacketSize = 6;

// Robot specific stuff
static boolean _enabled = false;      // Tells us if the robot is enabled or disabled
static boolean _waitingPacket = false;	// There is a driverstation packet in the queue
static unsigned long _lastUpdate = 0;  // Keeps track of the last time (ms) we received data

// General vars
static IPAddress _remoteIp;        // holds received packet's originating IP
static unsigned int _remotePort = 0; // holds received packet's originating port

// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

RobotOpenClass RobotOpen;

void RobotOpenClass::begin() {
    // Setup the initial pointers to the arrays
    _packetBufferAccessor = _packetBuffer;
    _validPacketAccessor = _validPacket;
	_pwmSerialData[0] = 0xFF;
	_pwmSerialData[1] = 0x00;
	_relaySerialData[0] = 0xFF;
	_relaySerialData[1] = 0x00;
	for (int i = 2; i <= 11; i++) {
		_pwmSerialData[i] = 127;
		_relaySerialData[i] = 0;
	}
    // Start Ethernet, UDP, and Serial
    Ethernet.begin(mac,ip);
    Udp.begin(PORT);
    Serial.begin(115200);	// This is used to talk to the coprocessor on the RobotOpen shield
}

void RobotOpenClass::pollDS() {
    // Process any data sitting in the buffer
    handleData();
  
    //Only allow robot to be enabled if we've received data in the last 250ms and robot is set to enabled
    if (((millis() - _lastUpdate) <= 250) && (millis() > 500)) {  //Robot is disabled for first 500ms of runtime
        _enabled = true;
	}
    else {
        _enabled = false;
    }
}

// This function's purpose is to receive data and prepare it for parsing
void RobotOpenClass::handleData() {
	_packetBufferSize = Udp.parsePacket();
    
    // If there's data, read the packet in
    if (_packetBufferSize > 0) {
		_remotePort = Udp.remotePort();
		_remoteIp = Udp.remoteIP();
		Udp.read(_packetBufferAccessor, 256);
		parsePacket();	// Data is all set, time to parse through it
    }
}

void RobotOpenClass::parsePacket() {
    /* ---PACKET FORMAT---
     * Message Type (1 byte)
     * Protocol Version (1 byte)
     * Device ID (1 byte)
     * ...Bundle Payloads... (1 length byte + 1 identifier byte + payload)
     * CRC16 Checksum (2 bytes)
     */
    
    unsigned int crc16_recv = (_packetBufferAccessor[_packetBufferSize - 2] << 8) | _packetBufferAccessor[_packetBufferSize - 1];
    if (_packetBufferAccessor[1] == PROTOCOL_VER && calc_crc16(_packetBufferAccessor, _packetBufferSize - 2) == crc16_recv) {
		// VALID PACKET
        if (_packetBufferAccessor[0] == CONTROL_PACKET && _packetBufferSize > 5) {
            swapValidPacket();	// This swaps the buffer and the "active" packet data pointers
			// Update the PWM generator values over serial
			unsigned int crc16_pwm = calc_crc16(_pwmSerialData, 12);
	        _pwmSerialData[12] = crc16_pwm >> 8;
	        _pwmSerialData[13] = (unsigned char)(crc16_pwm & 0xFF);
			for (int i = 0; i <= 13; i++)
				Serial.write(_pwmSerialData[i]);
			// Update the Relay values over serial
			unsigned int crc16_relay = calc_crc16(_relaySerialData, 12) + 1;
	        _relaySerialData[12] = crc16_relay >> 8;
	        _relaySerialData[13] = (unsigned char)(crc16_relay & 0xFF);
			for (int i = 0; i <= 13; i++)
				Serial.write(_relaySerialData[i]);
			// Make sure the system stays active
            _lastUpdate = millis();
        }
        publishDS();
    }
}

void RobotOpenClass::setPWM(int pwmChannel, int value) {
	if (pwmChannel > 0 && pwmChannel <= 10) {
		if (value > 255)
			value = 255;
		else if (value < 0)
			value = 0;
		_pwmSerialData[pwmChannel+1] = (unsigned char)value;
	}
}

void RobotOpenClass::setRelay(int relayChannel, boolean value) {
    unsigned char relayValue;
	if (relayChannel > 0 && relayChannel <= 10) {
		if (value == HIGH)
			relayValue = 0xFF;
        else
            relayValue = 0x00;
		_relaySerialData[relayChannel+1] = relayValue;
	}
}

void RobotOpenClass::swapValidPacket() {
    // Swap the pointers for the packet buffer and valid packet so that the validated packet is now active
    unsigned char *swap = _validPacketAccessor;
    _validPacketAccessor = _packetBufferAccessor;
    _packetBufferAccessor = swap;
    // Update the packet size value
    _validPacketSize = _packetBufferSize;
}

void RobotOpenClass::outgoingDS() {
    _waitingPacket = true;
}

void RobotOpenClass::publishDS() {
    /* ---PACKET FORMAT---
     * Message Type (1 byte)
     * Protocol Version (1 byte)
     * Device ID (1 byte)
     * Firmware Version (1 byte)
     * Robot State (1 byte)
     * Uptime (minutes) (1 byte)
     * ...Bundle Payloads... (1 length byte + 1 identifier byte + payload)
     * CRC16 Checksum (2 bytes)
     */
    
    unsigned char robotState = 0;
    if (_enabled == true)
        robotState = 0xFF;

	unsigned int uptime = (millis()/1000/60);
	if (uptime > 255)
		uptime = 255;
    
    // Build the packet and send it back to the DS
    if (_remotePort != 0) {
        _outgoingPacket[0] = FEEDBACK_PACKET;
        _outgoingPacket[1] = PROTOCOL_VER;
        _outgoingPacket[2] = ROBOTOPEN_V1;
        _outgoingPacket[3] = FIRMWARE_VER;
        _outgoingPacket[4] = robotState;
        _outgoingPacket[5] = (unsigned char)(uptime & 0xFF); // uptime in minutes (maxes out at 255)
        unsigned int crc16_recv = calc_crc16(_outgoingPacket, _outgoingPacketSize);
        _outgoingPacket[_outgoingPacketSize] = crc16_recv >> 8;
        _outgoingPacketSize++;
        _outgoingPacket[_outgoingPacketSize] = (unsigned char)(crc16_recv & 0xFF);
        _outgoingPacketSize++;
        Udp.beginPacket(_remoteIp, _remotePort);
		Udp.write(_outgoingPacket, _outgoingPacketSize);
		Udp.endPacket();
        _outgoingPacketSize = 6;
        _waitingPacket = false;
    }
}

unsigned int RobotOpenClass::calc_crc16(unsigned char *buf, unsigned short len) {
    unsigned short crc = 0;
    unsigned short i;
    for (i=0; i<len; i++)
        crc = ((crc >> 8) & 0xff) ^ pgm_read_word_near(crctab + (unsigned int)((crc ^ *buf++) & 0xff));
    return (crc);
}

bundleInfo RobotOpenClass::getBundleInfo(unsigned char bundleID) {
    unsigned int currentBundle = 0;
    unsigned int currentIndex = 3;
    while (currentIndex < _validPacketSize - 2) {
        if ((unsigned char)_validPacketAccessor[currentIndex + 1] == bundleID) {	// we have a bundle identifier match
            bundleInfo foundInfo;
            foundInfo.length = (unsigned int)_validPacketAccessor[currentIndex] - 1;
            foundInfo.payloadIndex = currentIndex + 2;
            return foundInfo;
        }
        
        currentBundle++;
        currentIndex += (unsigned int)_validPacketAccessor[currentIndex] + 1;
    }
    bundleInfo error = {0,0};
    return error;
}

int RobotOpenClass::getComponent(unsigned char bundleID, int componentIndex) {
    bundleInfo currentBundle = getBundleInfo(bundleID);
    if (currentBundle.length == 0 || componentIndex >= currentBundle.length || componentIndex < 0)	// error
        return -1;
    else {
        return _validPacketAccessor[currentBundle.payloadIndex + componentIndex];
    }
}

int RobotOpenClass::getBundleSize(unsigned char bundleID) {
    bundleInfo currentBundle = getBundleInfo(bundleID);
    if (currentBundle.length == 0)	// error
        return -1;
    else {
        return currentBundle.length - 1; // bundleID byte doesn't count
    }
}

void RobotOpenClass::publishAnalog(int pin, unsigned char bundleID) {
    if (_outgoingPacketSize < 251 && !_waitingPacket) {
		int valueRead = analogRead(pin);
        _outgoingPacket[_outgoingPacketSize] = 0x03;
        _outgoingPacketSize++;
		_outgoingPacket[_outgoingPacketSize] = bundleID;
        _outgoingPacketSize++;
        _outgoingPacket[_outgoingPacketSize] = highByte(valueRead);
        _outgoingPacketSize++;
        _outgoingPacket[_outgoingPacketSize] = lowByte(valueRead);
        _outgoingPacketSize++;
    }
}

void RobotOpenClass::publishDigital(int pin, unsigned char bundleID) {
    if (_outgoingPacketSize < 252 && !_waitingPacket) {
		pinMode(pin, INPUT);
		int valueRead = digitalRead(pin);
        unsigned char outputByte = 0;
        if (valueRead != 0)
            outputByte = 0xFF;
        _outgoingPacket[_outgoingPacketSize] = 0x02;
        _outgoingPacketSize++;
		_outgoingPacket[_outgoingPacketSize] = bundleID;
        _outgoingPacketSize++;
        _outgoingPacket[_outgoingPacketSize] = outputByte;
        _outgoingPacketSize++;
    }
}

void RobotOpenClass::publishByte(unsigned char byteRead, unsigned char bundleID) {
    if (_outgoingPacketSize < 252 && !_waitingPacket) {
        _outgoingPacket[_outgoingPacketSize] = 0x02;
        _outgoingPacketSize++;
		_outgoingPacket[_outgoingPacketSize] = bundleID;
        _outgoingPacketSize++;
        _outgoingPacket[_outgoingPacketSize] = byteRead;
        _outgoingPacketSize++;
    }
}

void RobotOpenClass::publishInt(unsigned int valueRead, unsigned char bundleID) {
    if (_outgoingPacketSize < 251 && !_waitingPacket) {
        _outgoingPacket[_outgoingPacketSize] = 0x03;
        _outgoingPacketSize++;
		_outgoingPacket[_outgoingPacketSize] = bundleID;
        _outgoingPacketSize++;
        _outgoingPacket[_outgoingPacketSize] = highByte(valueRead);
        _outgoingPacketSize++;
        _outgoingPacket[_outgoingPacketSize] = lowByte(valueRead);
        _outgoingPacketSize++;
    }
}

void RobotOpenClass::publishLong(long valueRead, unsigned char bundleID) {
    if (_outgoingPacketSize < 249 && !_waitingPacket) {
        _outgoingPacket[_outgoingPacketSize] = 0x05;
        _outgoingPacketSize++;
		_outgoingPacket[_outgoingPacketSize] = bundleID;
        _outgoingPacketSize++;
        unsigned char firstByte = (valueRead >> 24) & 0xFF;
        _outgoingPacket[_outgoingPacketSize] = firstByte;
        _outgoingPacketSize++;
        unsigned char secondByte = (valueRead >> 16) & 0xFF;
        _outgoingPacket[_outgoingPacketSize] = secondByte;
        _outgoingPacketSize++;
        unsigned char thirdByte = (valueRead >> 8) & 0xFF;
        _outgoingPacket[_outgoingPacketSize] = thirdByte;
        _outgoingPacketSize++;
        unsigned char fourthByte = valueRead & 0xFF;
        _outgoingPacket[_outgoingPacketSize] = fourthByte;
        _outgoingPacketSize++;
    }
}

boolean RobotOpenClass::enabled() {
    return _enabled;
}

USBJoystick::USBJoystick(unsigned char bID)
{
  _bundleID = bID;
}

int USBJoystick::getIndex(int index)
{
    return RobotOpen.getComponent(_bundleID, index);
}

unsigned int USBJoystick::makePWM(int index, char mode)
{
	int val = RobotOpen.getComponent(_bundleID, index);
	if (val == -1)	// there was an error - send PWM to neutral
		return 127;
    if (mode == INVERT)
		return (unsigned int)(255 - val);
    else
		return (unsigned int)val;
}

boolean USBJoystick::getBtn(int index, char mode) {
    if (RobotOpen.getComponent(_bundleID, index) == 0) {
        if (mode == INVERT)
        	return HIGH;
		else
			return LOW;
	}
    else {
        if (mode == INVERT)
        	return LOW;
		else
			return HIGH;
	}
}

boolean USBJoystick::getDpad(unsigned char compare, char mode) {
    // index 6 is always DPAD
    if (RobotOpen.getComponent(_bundleID, 6) == compare) {
		if (mode == INVERT)
        	return LOW;
		else
			return HIGH;
	}
    else {
        if (mode == INVERT)
        	return HIGH;
		else
			return LOW;
	}
}