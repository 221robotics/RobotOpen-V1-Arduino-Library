/* Swerve demo using a 221 Robotic Systems Mobile Platform */

#include <SPI.h>
#include <Ethernet.h>
#include <RobotOpen.h>


/* I/O Setup */
USBJoystick usb1('0');  // Assign the logitech USBJoystick object to bundle 0


/* Swerve Constants */
#define frontMIN 10
#define frontMAX 1010
#define rearMIN 10
#define rearMAX 1010
#define PCONSTANT 4
#define TURNING_CUTOFF 20


/* POT Tracking Vars */
int frontPOT = 512;
int rearPOT = 512;
int lastFrontPOT = 512;
int lastRearPOT = 512;
int frontPotTurns = 0;
int rearPotTurns = 0;


void setup()
{
  /* Initiate comms */
  RobotOpen.begin();
}


/* This is your primary robot loop - all of your code
 * should live here that allows the robot to operate
 */
void enabled() {
  frontPOT += frontPotTurns * 1023;
  rearPOT += rearPotTurns * 1023;
    
  int frontError = frontPOT - map(usb1.makePWM(ANALOG_RIGHTX, NORMAL), 0, 255, frontMIN, frontMAX);  //Find error for front wheels
  int rearError = rearPOT - map(usb1.makePWM(ANALOG_RIGHTX, NORMAL), 0, 255, frontMIN, frontMAX);   //Find error for rear wheels
      
  int rawFrontError = frontError;
  int rawRearError = rearError; 
                
  frontError *= PCONSTANT;
  rearError *= PCONSTANT;
    
  if (frontError > 511)
    frontError = 511;
  else if (frontError < -512)
    frontError = -512;
  if (rearError > 511)
    rearError = 511;
  else if (rearError < -512)
    rearError = -512;    
    
  // Steering motors
  if (rawFrontError > TURNING_CUTOFF || rawFrontError + TURNING_CUTOFF < 0)
    RobotOpen.setPWM(SIDECAR_PWM3, map(frontError, -512, 511, 0, 255));
  else
    RobotOpen.setPWM(SIDECAR_PWM3, 127);
        
  if (rawRearError > TURNING_CUTOFF || rawRearError + TURNING_CUTOFF < 0)
    RobotOpen.setPWM(SIDECAR_PWM4, map(rearError, -512, 511, 0, 255));
  else
    RobotOpen.setPWM(SIDECAR_PWM4, 127);
          
  // Drive motors
  RobotOpen.setPWM(SIDECAR_PWM1, usb1.makePWM(ANALOG_LEFTY, NORMAL));
  RobotOpen.setPWM(SIDECAR_PWM2, usb1.makePWM(ANALOG_LEFTY, INVERT));
}


/* This is called while the robot is disabled
 * You must make sure to set all of your outputs
 * to safe/disable values here
 */
void disabled() {
  // PWMs are automatically disabled
}


/* This loop ALWAYS runs - only place code here that can run during a disabled state
 * This is also a good spot to put driver station publish code
 * You can use either publishAnalog, publishDigital, publishByte, publishShort, or publishLong
 * Specify a bundle ID with a single character (a-z, A-Z, 0-9) - Just make sure not to use the same twice!
 */
void timedtasks() {
  frontPOT = analogRead(ANALOG1);
  rearPOT = analogRead(ANALOG2);

  if ((frontPOT - lastFrontPOT) > 200)
    frontPotTurns -= 1;
  if ((frontPOT - lastFrontPOT) < -200)
    frontPotTurns += 1;
      
  if ((rearPOT - lastRearPOT) > 200)
    rearPotTurns -= 1;
  if ((rearPOT - lastRearPOT) < -200)
    rearPotTurns += 1;
      
  lastFrontPOT = frontPOT;
  lastRearPOT = rearPOT;
  
  RobotOpen.publishAnalog(ANALOG0, 'A');   // Bundle A
  RobotOpen.publishAnalog(ANALOG1, 'B');   // Bundle B
  RobotOpen.publishAnalog(ANALOG2, 'C');   // Bundle C
  RobotOpen.publishAnalog(ANALOG3, 'D');   // Bundle D
  RobotOpen.publishAnalog(ANALOG4, 'E');   // Bundle E
  RobotOpen.publishAnalog(ANALOG5, 'F');   // Bundle F
}


/* This is the main program loop that keeps comms operational
 * There's no need to touch anything here!!!
 */
void loop() {
  RobotOpen.pollDS();
  if (RobotOpen.enabled())
    enabled();
  else
    disabled();
  timedtasks();
  RobotOpen.outgoingDS();
}