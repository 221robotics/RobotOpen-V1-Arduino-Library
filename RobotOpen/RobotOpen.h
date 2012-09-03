/*
  RobotOpen.h - Library implementation of the RobotOpen Hardware found at www.RobotOpen.biz
  Created by Eric Barch, October 16, 2011.
*/

#ifndef RobotOpen_h
#define RobotOpen_h

#include "Arduino.h"

// Solenoid Constants
#define ON 0xFF
#define OFF 0x00

// Controller Aliases
#define ANALOG_LEFTX 0x00
#define ANALOG_LEFTY 0x01
#define ANALOG_RIGHTX 0x02
#define ANALOG_RIGHTY 0x03
#define LEFT_ANALOG_BTN 0x04
#define RIGHT_ANALOG_BTN 0x05
#define DPAD 0x06
#define BTN1 0x07
#define BTN2 0x08
#define BTN3 0x09
#define BTN4 0x0A
#define BTN5 0x0B
#define BTN6 0x0C
#define BTN7 0x0D
#define BTN8 0x0E
#define BTN9 0x0F
#define BTN10 0x10

// D-PAD Constants
#define UP 0x3F
#define UP_LEFT 0x1F
#define UP_RIGHT 0x5F
#define DOWN 0xBF
#define DOWN_LEFT 0xDF
#define DOWN_RIGHT 0x9F
#define LEFT 0xFF
#define RIGHT 0x7F

// Joystick modifiers
#define INVERT 1
#define NORMAL 0

// Sidecar Mapping
#define ANALOG0 0
#define ANALOG1 1
#define ANALOG2 2
#define ANALOG3 3
#define ANALOG4 4
#define ANALOG5 5
#define SIDECAR_DIGITAL1 7
#define SIDECAR_DIGITAL2 6
#define SIDECAR_DIGITAL3 5
#define SIDECAR_DIGITAL4 4
#define SIDECAR_DIGITAL5 3
#define SIDECAR_DIGITAL6 2
#define SIDECAR_DIGITAL7 9
#define SIDECAR_DIGITAL8 8
#define SIDECAR_PWM1 1
#define SIDECAR_PWM2 2
#define SIDECAR_PWM3 3
#define SIDECAR_PWM4 4
#define SIDECAR_PWM5 5
#define SIDECAR_PWM6 6
#define SIDECAR_PWM7 7
#define SIDECAR_PWM8 8
#define SIDECAR_PWM9 9
#define SIDECAR_PWM10 10
#define SOLENOID_1 1
#define SOLENOID_2 2
#define SOLENOID_3 3
#define SOLENOID_4 4
#define SOLENOID_5 5
#define SOLENOID_6 6
#define SOLENOID_7 7
#define SOLENOID_8 8
#define SOLENOID_9 9
#define SOLENOID_10 10


typedef struct {
   unsigned int length;
   unsigned int payloadIndex;
} bundleInfo;


class RobotOpenClass {
public:
    // Fire up the RobotOpen object and get things running
    static void begin();
    
    // Check for fresh data
    static void pollDS();
    
    // There is a feedback packet ready to send
    static void outgoingDS();

	// Sets a PWM output that is fed to the PWM generator chip
	static void setPWM(int pwmChannel, int value);
	
	// Sets a Solenoid output that is fed to the Solenoid generator chip
	static void setSolenoid(int solenoidChannel, boolean value);
    
    // Tells us if the robot is enabled
    static boolean enabled();

    // Used to publish data back to the dashboard
    static void publishAnalog(int pin, unsigned char bundleID);
    static void publishDigital(int pin, unsigned char bundleID);
    static void publishByte(unsigned char byteRead, unsigned char bundleID);
    static void publishInt(unsigned int valueRead, unsigned char bundleID);
    static void publishLong(long valueRead, unsigned char bundleID);
    
	// Get the number of components in a particular bundle
    static int getBundleSize(unsigned char bundleID);
	
    // Used to fetch a component from a data packet
    static int getComponent(unsigned char bundleID, int componentIndex);

private:
    // Periodic Task Functions
    static void handleData();
    static void parsePacket();
    static void swapValidPacket();
    
    // Dumps data back to the DS
    static void publishDS();
    
    // Used to fetch needed info of bundle
    static bundleInfo getBundleInfo(unsigned char bundleID);
    
    // CRC16 checksum function
    static unsigned int calc_crc16(unsigned char *buf, unsigned short len);
};

extern RobotOpenClass RobotOpen;

class USBJoystick
{
  public:
    USBJoystick(unsigned char);
    int getIndex(int index);
    unsigned int makePWM(int index, char mode);
    boolean getBtn(int index, char mode);
    boolean getDpad(unsigned char compare, char mode);
    
  private:
    unsigned char _bundleID;
};

#endif