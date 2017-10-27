/* ** add your names **
 * Authors: Nathan Welch, Colleen Kitzmann
 */

#include <Servo.h>
#include <XBOXRECV.h>

USB Usb;
XBOXRECV Xbox(&Usb);

//I dont know if we actually need these
#ifdef dobogusinclude
#include <spi4teelnsy3.h>
#endif
#include <SPI.h>

Servo shoulderLR; //shoulder servo moving in left/right direction
Servo shoulderUD; //shoulder servo moving in up/down direction
Servo elbow; //servo controlling elbow movements
Servo claw;  //servo controlling claw movements, assume only one servo needed -> both react the same

//constants
#define UPPER_BOUND 255
#define LOWER_BOUND 0
//these are temp values
//this also assumes that there is just an open and closed position to the claw
//   and we aren't going to have 'valriable open/close positions'
#define POS_CLAW_CLOSE 2
#define POS_CLAW_OPEN 2

//these are all just placeholder values
#define SHOULDER_LR_PIN 1
#define SHOULDER_UD_PIN 2
#define ELBOW_PIN 3
#define CLAW_PIN 4

int moveSpeed = 2;
//thses are temp values
int pos_slr= 180; //starting position for shoulderLR x-axis
int pos_sud = 180;  //starting position for shoulderUD y-axis
//Creating these just so code compiles
int pos_elbow = 180;

void setup() {
  Serial.begin(115200);  //serial initialization
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  Serial.print(F("\r\nXbox Wireless Receiver Library Started"));

  shoulderLR.attach(SHOULDER_LR_PIN); //attaches a servo to a pin
  shoulderLR.attach(SHOULDER_UD_PIN);
  elbow.attach(ELBOW_PIN);
  claw.attach(CLAW_PIN);  
}

void loop() {
  Usb.Task();  //"polls connected usb devices for updates to their status"
  if (Xbox.XboxReceiverConnected) {
    for(uint8_t i = 0; i < 4; i++) //for each possible controller
    {
      if(Xbox.Xbox360Connected[i]) //if it is connected
      {
        if(Xbox.getButtonPress(L2, i) || Xbox.getButtonPress(R2, i))  //if a trigger is pressed
        {
          Serial.print("L2: ");
          Serial.print(Xbox.getButtonPress(L2, i));
          Serial.print("\tR2: ");
          Serial.println(Xbox.getButtonPress(R2, i));  //output some numbers to serial
          //and make them vibrate
          Xbox.setRumbleOn(Xbox.getButtonPress(L2, i), Xbox.getButtonPress(R2, i), i); 
        }
    
        //if left joystick x-axis is triggered  -> moves shoulderLR
        if(Xbox.getAnalogHat(LeftHatX, i) > 7500) {
          Serial.print("shoulder right\n");         
          pos_slr += moveSpeed;  //make it move a little
          if(pos_slr > UPPER_BOUND)  //check to make sure the bounds are correct
            pos_slr = UPPER_BOUND;
          shoulderLR.write(pos_slr); //sets servo position
        }
        if(Xbox.getAnalogHat(LeftHatX, i) < -7500) {
          Serial.print("shoulder left\n");  
          pos_slr -= moveSpeed;  //make it move a little
          if(pos_slr < LOWER_BOUND) //check to make sure the bounds are correct
            pos_slr = LOWER_BOUND; 
          shoulderLR.write(pos_slr); //sets servo position
        }
        
        //if left joystick y-axis is triggered   (shoulderUD)
        if(Xbox.getAnalogHat(LeftHatY, i) > 7500) {
          Serial.print("shoulder up\n");
          pos_sud += moveSpeed;  //make it move a little
          if(pos_sud > UPPER_BOUND)  //check to make sure the bounds are correct
            pos_sud = UPPER_BOUND;
          shoulderUD.write(pos_sud); //sets servo position
        }
        if(Xbox.getAnalogHat(LeftHatY, i) < -7500) {
          Serial.print("shoulder down\n");
          pos_sud -= moveSpeed;  //make it move a little
          if(pos_sud < LOWER_BOUND) //check to make sure the bounds are correct
            pos_sud = LOWER_BOUND; 
          shoulderUD.write(pos_sud); //sets servo position
        }
        
        //if right joystick y-axis is triggered  (elbow)  
        if(Xbox.getAnalogHat(RightHatY, i) > 7500) {
          Serial.print("elbow up\n");
          pos_elbow += moveSpeed;  //make it move a little
          if(pos_elbow > UPPER_BOUND)  //check to make sure the bounds are correct
            pos_elbow = UPPER_BOUND;
          elbow.write(pos_elbow); //sets servo position
        }
        if(Xbox.getAnalogHat(RightHatY, i) < -7500) {
          Serial.print("elbox down\n");
          pos_elbow -= moveSpeed;  //make it move a little
          if(pos_elbow < LOWER_BOUND) //check to make sure the bounds are correct
            pos_elbow = LOWER_BOUND; 
          elbow.write(pos_elbow); //sets servo position
        }
//      pos_ELBOW_X needs to be added

        
        //close the claw
        if(Xbox.getButtonClick(L2, i)) {
          Serial.print("claw close\n");
          claw.write(POS_CLAW_CLOSE); //sets servo position
        }
        //open the claw
        if(Xbox.getButtonClick(R2, i)) {
          Serial.print("claw open\n");
          claw.write(POS_CLAW_OPEN); //sets servo position
        }   
      }
    }
  }
}

