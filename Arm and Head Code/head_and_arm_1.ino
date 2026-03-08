
// include continuous servo, wifi, stepper library
#include <Stepper.h>                         // create stepper object to control a stepper
#include <ESP32Servo360.h>                   // For Parallax 360 servos with feedback
ESP32Servo360 servo1;                          // create servo object to control a servo1
ESP32Servo360 servo2;                          // create servo object to control a servo2



// stepper motor pinModes
const int stepsPerRevolution = 205;                // initialize stepper motor's number of steps per revolution
const int rolePerMinute = 17;                       // Stepper motor's Adjustable range of 28BYJ-48 stepper is 0~17 rpm
Stepper myStepper(stepsPerRevolution, 7, 5, 6, 4);  // pins connected to stepper motor driver to control the stepper motor (arbitrary)

long headPosition = 0;  // track current step position of head (0 = neutral)

// servo motor pinModes
int pos_servo = 0;                                // variable to store the servo position
const int rest_servo = 0;                         // initial rest position of servo (arthur's arm is down, parallel to body

// do i need to add a variable to allow the stepper to turn on and off when not used??


int R1 = 0;                                       // controls right arm, lifts arm up (CW)
int R2 = 0;                                       // controls right arm, lowers arm (CCW)
int L1 = 0;                                       // controls left arm, lifts arm up (CCW)
int L2 = 0;                                       // controls left arm, lowers arm (CW)
int B1 = 0;                                       // controls head CCW
int B2 = 0;                                       // controls head CW


void setup() {

// defining variable pinModes for head and arm control

// serial monitor
  Serial.begin(9600);                       // set up serial monitor

// stepper motor set up
  myStepper.setSpeed(rolePerMinute);                   // set the stepper speed at 17 rpm

// continous servo motor
  servo1.attach(4, 16); // Control pin (4), signal pin (16). arbitrary for now
  servo2.attach(5, 17); // Control pin (5), signal pin (17). arbitrary for now
  servo1.setSpeed(30); // Set 30 turns per minute (RPM), 140 max.
  servo2.setSpeed(30); // Set 30 turns per minute (RPM), 140 max.
}

// FUNCTIONS

// rotating right arm up
void RightArmUp () {
    servo1.spin();                                       // servo rotating CW at 30 RPM
    Serial.println("Right arm up");                     // serial monitor 
    delay(100);                                         // wait 0.1 seconds
}

// right arm to stop rotation immediately
void RightArmStop () {
    servo1.stop()                               // servo motor to stop
    Serial.println("right arm stop");         // serial monitor 
    delay(100);                             // wait 0.1 seconds
}

// rotating right arm down
void RightArmDown () {
    servo1.spin(-30);                        // servo rotating CCW at 30 RPM
    Serial.println("right arm down");   // serial monitor 
    delay(100);                             // wait 0.1 seconds
}

// rotating left arm up
void LeftArmUp () {
    servo2.spin(-30);                                       // servo rotating CCW at 30 RPM
    Serial.println("left arm up");                     // serial monitor 
    delay(100);                                         // wait 0.1 seconds
}

// left arm to stop rotation immediately
void LeftArmStop () {
    servo2.stop()                               // servo motor to stop
    Serial.println("left arm stop");         // serial monitor 
    delay(100);                             // wait 0.1 seconds
}

// rotating left arm down
void LeftArmDown () {
    servo2.spin();                           // servo rotating CW at 30 RPM
    Serial.println("left arm down");       // serial monitor 
    delay(100);                             // wait 0.1 seconds
}

void RestPositionArms () {
servo1.rotateTo(0);                        // rotates servo to 0 degree position
servo1.wait();

servo2.rotateTo(0);                        // rotates servo to 0 degree position
servo2.wait();
}

void HeadRotateClockwise() {
    Serial.println("clockwise");            // print that stepper will rotate CW
    myStepper.step(stepsPerRevolution);     // stepper will rotate CW for value of steps per revolution
    delay(100);                             // wait 0.1 seconds before switching to CCW
    headPosition += stepsPerRevolution;      // update position tracker
    serial.println(headPosition);            // head position read
    Serial.println("stepper motor worked"); // serial print that stepper motor worked!
}

void HeadRotateCounterclockwise() {
  // step one revolution in the other direction:
    Serial.println("counterclockwise");     // print that stepper will rotate CCW
    myStepper.step(-stepsPerRevolution);    // stepper will rotate CCW for value of steps per revolution
    delay(100);                             // wait 0.1 seconds before switching to CCW
    headPosition += stepsPerRevolution;      // update position tracker
    serial.println(headPosition);            // head position read
    Serial.println("stepper motor worked"); // serial print that stepper motor worked!
}

void HeadStop() {
  myStepper.step(0);    // stepper will stop
  delay(100);
  Serial.println("stepper motor stopped"); // serial print that stepper motor worked
}

void HeadNeutral(); {
  Serial.println("Returning head to neutral (0°) position...");
  // Step in opposite direction until back to zero
  myStepper.step(-headPosition);   // move the exact opposite distance to return to 0
  headPosition = 0;                // reset position tracker
  delay(100);
  Serial.println("Head at neutral position.");
}


void controlArmsandHead(); {

// lifting right arm commands
if (R1 == 1) RightArmUp(); 
else if (R1 == 0) {RightArmStop();
else RightArmStop();

// lowering right arm commands
if (R2 == 1) RightArmDown();
else if (R2 == 0) RightArmStop();
else RightArmStop();

// lifting left arm commands
if (L1 == 1) LeftArmUp();
else if (L1 == 0) LeftArmStop();
else LeftArmStop();

// lowering left arm commands
if (L2 == 1) LeftArmDown();
else if (L2 == 0) LeftArmStop();
else LeftArmStop();

// resetting right arm back to default position
if (R2 == 1 && L2 == 1) RestPositionArms();

// rotate head CCW
if (B1 == 1) HeadRotateCounterclockwise();
else if (B1 == 0); HeadStop();
else HeadStop();

// rotate head clockwise
if (B2 == 1) HeadRotateClockwise();
else if (B2 == 0) HeadStop();
else HeadStop();

// reset head to rest position
if (B1 == 1 && B2 == 1) HeadNeutral();
}

void loop() {

// read ps4 inputs
controlArmsandHead();

delay(50);
}
