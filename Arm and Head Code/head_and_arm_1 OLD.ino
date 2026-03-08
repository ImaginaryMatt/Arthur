
Head Code needs to:
0) click button 
1) recieve signal from PS4 to wifi
2) active stepper
3) rotate a certain number of degree to turn head
4) delay
5) either
  5i) stay in same position
  5ii) rotate back to original position
6) end of function

Arm Code needs to:
0) Click button
1) recieve signal from PS4 to wifi
2) activate servo motor
3) rotate servo motor 180 degrees upwards
4) delay
5) rotate servo motor 180 degrees back down to original position
6) end of function


// include continuous servo, wifi, stepper library
                         // wifi library
#include <Stepper.h>                         // create stepper object to control a stepper
#include <ESP32Servo360.h>                   // For Parallax 360 servos with feedback

// servo motor pinModes
int pos = 0;                                // variable to store the servo position

// stepper motor pinModes
const int stepsPerRevolution = 2048;        // initialize stepper motor's number of steps per revolution
Stepper myStepper(stepsPerRevolution, 7, 5, 6, 4);  // pins connected to stepper motor driver to control the stepper motor (arbitrary)


void setup() {

// serial monitor
  Serial.begin(9600);                       // set up serial monitor

// stepper motor set up
  myStepper.setSpeed(60);                   // set the stepper speed at 60 rpm:

// continous servo motor
  servo.attach(4, 16); // Control pin (4), signal pin (16). arbitrary for now
  servo.setSpeed(15); // Set turns per minute (RPM), 140 max.
}

// FUNCTIONS

// servo motor function, rotating clockwise
void StartWaveServo () {
    servo.spin();                           // servo rotating clockwise at 15 RPM
    Serial.println(pos);                    // serial monitor writes position servo is at
    Serial.println("Servo motor rotating clockwise");   // serial monitor will write if servo motor moved clockwise
    delay(100);                             // wait 0.1 seconds
}

// servo motor function, stop rotation immediately
void StopWave () {
    servo.stop()
    Serial.println("Servo motor stopped");   // serial monitor will write if servo motor stopped
    delay(100);                             // wait 0.1 seconds
}

// servo motor function, rotate counterclockwise
void EndWaveServo () {
    servo.spin(-15);                        // servo rotating counterclockwise at 15 RPM
    Serial.println(pos);                    // serial monitor writes position servo is at
    Serial.println("Servo motor rotating counterclockwise");   // serial monitor will write if servo motor moved clockwise
    delay(100);                             // wait 0.1 seconds
}

void HeadRotateClockwise() {
  // step one revolution in one direction:
    Serial.println("clockwise");            // print that stepper will rotate CW
    myStepper.step(stepsPerRevolution);     // stepper will rotate CW for value of steps per revolution
    delay(100);                             // wait 0.1 seconds before switching to CCW
    Serial.println("stepper motor worked"); // serial print that stepper motor worked!
}

void HeadRotateCounterclockwise() {
  // step one revolution in the other direction:
    Serial.println("counterclockwise");     // print that stepper will rotate CCW
    myStepper.step(-stepsPerRevolution);    // stepper will rotate CCW for value of steps per revolution
    delay(100);                             // wait 0.1 seconds before switching to CCW
    Serial.println("stepper motor worked"); // serial print that stepper motor worked!
}

void HeadNeutral(); {
  Serial.println("back to neutral position");     // print that stepper will go to neutral position
  ?????????????????????????

}

void loop() {

// if Matt is holding in arm button
// code about measuring certain holding input, then function starts
StartWaveServo();

//else if
// if other button is pressed to lower arm
EndWaveServo();

//else if
// matt is not holding in any arm buttons
StopWave();
//should i code in a reset for the arm? so when nothing is being pressed it automatically lowers?

// Matt is holding head to rotate clockwise
// if this button is reading "pressed"...
    HeadRotateClockwise();
//else if 
// if the clockwise button is being held, 
    HeadRotateCounterClockwise();
//else if
// if nothing is being held in, reset head position
  HeadNeutral();

}
