/* ** add your names **
 * Authors: Nathan Welch, Colleen Kitzmann
 */

#include <Servo.h>
#include <XBOXRECV.h>
#include <time.h>
#include <stdlib.h>

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


//led constants
//placeholder pin values for all LEDs
const unsigned char LED_PINS[] = {1,2,3,4,5,6,7,8,9};//The LED_PINS array; can add more arrays to seperate LEDs for different goal posts.
const unsigned char led_pin_array_size = sizeof(LED_PINS)/sizeof(LED_PINS[0]); //size of the LED_PINS array

//random lighting led variables
unsigned char gameStarted = 0;//(0 = start button has not been pressed, 1 = start has been pressed)
                              //used so random LEDs start only when start button has been pressed
unsigned char randomOn = 0;   //default option for random LED lighting (0 = all leds on, 1 = start with random lighting)
unsigned long current_time = 0, previous_time = 0, interval= 5000; //used to determine random LED changes
                                                                   //interval is initally set, it changes
                                                                   //randomly between 5-9 sec within loop

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

  //setting all LED's to output
  for(unsigned char i = 0; i < led_pin_array_size-1;i++){
    pinMode(LED_PINS[i], OUTPUT);
  }
  
  //time seed set to NULL - used for random LEDs
  srand(time(NULL));
}

void loop() {
  Usb.Task();  //"polls connected usb devices for updates to their status"

  //Random LED Lighting
  //randomOn is initally set above (@variable declerations) or toggled using 'BACK' button
  //gameStarted is set when start button has been pressed
  if (randomOn && gameStarted){
    current_time = millis();
    if (current_time - previous_time >= interval){//if time difference since last change is greater than interval set
      randomLEDs();                               //change LEDs individually to HIGH or LOW
      previous_time = current_time;               //record time for last change
      interval = rand() % 5000 + 5000;            //change the interval between 5-10 seconds
    }
  }
  
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
         //if start button gets triggered
        if (Xbox.getButtonClick(START, i)){
          gameStarted = 1;
          //if randomOn is set it initializes LEDs to random LED lighting else it turns on all LEDs
          (randomOn)? randomLEDs():turnOnAllLEDs();
        }

        //if back button is pressed it toggles between random LED lighting and having all LEDs on
        if (Xbox.getButtonClick(BACK, i)){
          if (randomOn){ //change to not have random LED lighting timing
            randomOn = 0;
            turnOnAllLEDs();
          }
          else{  //change to random LED lighting timing
            randomOn = 1;
            randomLEDs();
          }
        }

      }
    }
  }
}


/**
 * Turns on all LEDs listed in the LED_PINS array
 * @Param None, xbox start button has been pressed
 * @Return All LED specified should turn on.
 */
void turnOnAllLEDs(){
  for(unsigned char i = 0; i < led_pin_array_size-1;i++){
    digitalWrite(LED_PINS[i], HIGH);
  }
}

/**
 * Turns off all LEDs listed in the LED_PINS array (not implemented)
 * @Param None
 * @Return All LED specified should turn off.
 */
void turnOffAllLEDs(){
  for(unsigned char i = 0; i < led_pin_array_size-1;i++){
    digitalWrite(LED_PINS[i], LOW);
  }
}

/**
 * Random LEDs are turned on and others off
 * @Param randomOn has been set(1)
 * @Return All LED are changed to either HIGH or LOW randomly
 */
void randomLEDs(){
  unsigned char set_or_unset;
  for(unsigned char i = 0; i < led_pin_array_size-1;i++){
    set_or_unset = rand()%2; //0 or 1
    //if set_or_unset is 1 then that LED is set to HIGH otherwise set to LOW
    set_or_unset? digitalWrite(LED_PINS[i], HIGH):digitalWrite(LED_PINS[i], LOW);
  }
}
