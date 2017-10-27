/* ** add your names **
 * Authors: Nathan Welch, Colleen Kitzmann
 */

#include <Servo.h>
#include <XBOXRECV.h>

USB Usb;
XBOXRECV Xbox(&Usb);

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
int pos_SLR_X= 180; //starting position for shoulderLR x-axis
int pos_SLR_Y = 180; //starting position for shoulderLR y-axis
int pos_SUD_X = 180;  //starting position for shoulderUD x-axis
int pos_SUD_Y = 180;  //starting position for shoulderUD y-axis
int pos_ELBOW_Y = 180;

void setup() {
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  Serial.print(F("\r\nXbox Wireless Receiver Library Started"));

  // put your setup code here, to run once:
  shoulderLR.attach(SHOULDER_LR_PIN);
  shoulderLR.attach(SHOULDER_UD_PIN);
  elbow.attach(ELBOW_PIN);
  claw.attach(CLAW_PIN);  
}

void loop() {
  Usb.Task();
  if (Xbox.XboxReceiverConnected) {

    //if left joystick x-axis is triggered   (shoulderLR)
    int i = 0;
    if(Xbox.getAnalogHat(LeftHatX, i) > 7500) {
      Serial.print("left stick x-axis +\n");
      pos_SLR_X += moveSpeed;  //make it move a little
      if(pos_SLR_X > UPPER_BOUND)  //check to make sure the bounds are correct
        pos_SLR_X = UPPER_BOUND;
      shoulderLR.write(pos_SLR_X); //sets servo position
    }
    if(Xbox.getAnalogHat(LeftHatX, i) < -7500) {
      Serial.print("left stick x-axis -\n");
      pos_SLR_X -= moveSpeed;  //make it move a little
      if(pos_SLR_X < LOWER_BOUND) //check to make sure the bounds are correct
        pos_SLR_X = LOWER_BOUND; 
      shoulderLR.write(pos_SLR_X); //sets servo position
    }
    
    //if left joystick y-axis is triggered   (shoulderUD)
    if(Xbox.getAnalogHat(LeftHatY, i) > 7500) {
      Serial.print("left stick y-axis +\n");
      pos_SUD_Y += moveSpeed;  //make it move a little
      if(pos_SUD_Y > UPPER_BOUND)  //check to make sure the bounds are correct
        pos_SUD_Y = UPPER_BOUND;
      shoulderUD.write(pos_SUD_Y); //sets servo position
    }
    if(Xbox.getAnalogHat(LeftHatY, i) < -7500) {
      Serial.print("left stick y-axis -\n");
      pos_SUD_Y -= moveSpeed;  //make it move a little
      if(pos_SUD_Y < LOWER_BOUND) //check to make sure the bounds are correct
        pos_SUD_Y = LOWER_BOUND; 
      shoulderUD.write(pos_SUD_Y); //sets servo position
    }
    
    //if right joystick y-axis is triggered  (elbow)  
    if(Xbox.getAnalogHat(RightHatY, i) > 7500) {
      Serial.print("right stick y-axis +\n");
      pos_ELBOW_Y += moveSpeed;  //make it move a little
      if(pos_ELBOW_Y > UPPER_BOUND)  //check to make sure the bounds are correct
        pos_ELBOW_Y = UPPER_BOUND;
      elbow.write(pos_ELBOW_Y); //sets servo position
    }
    if(Xbox.getAnalogHat(RightHatY, i) < -7500) {
      Serial.print("right stick y-axis -\n");
      pos_ELBOW_Y -= moveSpeed;  //make it move a little
      if(pos_ELBOW_Y < LOWER_BOUND) //check to make sure the bounds are correct
        pos_ELBOW_Y = LOWER_BOUND; 
      elbow.write(pos_ELBOW_Y); //sets servo position
    }

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

