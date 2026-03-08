
// All values before setup are values that will need to be adjusted and/or tuned
// Arbitrary pin values have been assigned. These values should be adjusted to match the wiring of the installed Arduino
int motor1pin1 = 1;
int motor1pin2 = 2;

int motor2pin1 = 3;
int motor2pin2 = 4;

int motorLeftPWM = 9;
int motorRightPWM = 10;

// States will need to be assigned based on the outputs from the test code
int leftFWDState = 1;
int leftBWDState = 2;
int rightFWDState = 3;
int rightBWDState = 4;

//Initializing the motor speed variables
int leftSpeed = 0;
int rightSpeed = 0;

//This is a value that will need to be tuned to adjust the threshhold for which the motors actually turn on so as to prevent buzzing
int buzzThreshhold = 0;

void setup() {
  // Defining all of the pinModes for motor control
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);

  pinMode(motorLeftPWM, OUTPUT);
  pinMode(motorRightPWM, OUTPUT);
}

void controlDrivetrain(int leftJoystick, int rightJoystick, int leftDeadzoneLowerBound, int leftDeadzoneUpperBound, int rightDeadzoneLowerBound, int rightDeadzoneUpperBound, int speedLimit) {
  // put your main code here, to run repeatedly:

//  // First you read in joystick values
//  //These values will be found by the python script and read in from the control computer
//  int leftSpeed = 0;
//  int leftDIR = 0;
//  
//  int rightSpeed = 0;
//  int rightDIR = 0;
//
//  //Based on the direction variables recieved from the python code, the correct motor states are applied
//  if (leftDIR = -1) {
//    //Call the motorState function explained more below
//    motorState(leftBWDState);
//  }
//  else if (leftDIR = 1) {
//    motorState(leftFWDState);
//  }
//
//  // The same code used for the left joystick is then run for the right
//  if (rightDIR = -1) {
//    motorState(rightBWDState);
//  }
//
//  else if (rightDIR = 1) {
//    motorState(rightFWDState);
//  }

  //  This is an alternative version of the control code that allows for deadzone to be controlled by the operator but does all interpretation client sidde

  // This takes in the direct joystick values from the sent packet
  int leftJoystick = 0;
  int rightJoystick = 0;

  // Also sent in the packet will be deadzone bounds for the left and right stick. These could be combined into a singualr set of bounds, but I left them seperate
  // for maximum custimizability
  int leftDeadzoneLowerBound = 470;
  int leftDeadzoneUpperBound = 550;

  int rightDeadzoneLowerBound = 470;
  int rightDeadzoneUpperBound = 550;

  // This is the final variable sent for motor control from the driver station. This value should allow us to adjust the maximum speed of the robot remotely
  int speedLimit = 255;

  // Joystick values are then compared to the deadzone bounds to determine what direction the robot should be moving in and calculate speed 
  if (leftJoystick < leftDeadzoneLowerBound) {
    motorState(leftBWDState);
    leftSpeed = map(leftJoystick,leftDeadzoneLowerBound,0,0,speedLimit);
  }

  else if (leftJoystick > leftDeadzoneUpperBound) {
    motorState(leftFWDState);
    leftSpeed = map(leftJoystick,leftDeadzoneUpperBound,1023,0,speedLimit);
  }
  
  else {
    leftSpeed = 0; //This statement is likely unnecessary, but I would reccomend leaving it in as a failsafe
  }

  // The same process is repeated concerning the right joystick
  if (rightJoystick < rightDeadzoneLowerBound) {
    motorState(rightBWDState);
    rightSpeed = map(rightJoystick,rightDeadzoneLowerBound,0,0,speedLimit);
  }

  else if (rightJoystick > rightDeadzoneUpperBound) {
    motorState(rightFWDState);
    rightSpeed = map(rightJoystick,rightDeadzoneUpperBound,1023,0,speedLimit);
  }

  else {
    rightSpeed = 0;
  }

  //Checking if motor speed exceeds threshhold for movement. This is because some motors will only generate torque when supplied with a certain amount of PWM, and if provided too little power will just buzz.
  if (leftSpeed < buzzThreshhold) {
    leftSpeed = 0;
  }
  
  if (rightSpeed < buzzThreshhold) {
    rightSpeed = 0;
  }
  // Converted joystick values are then written to speed inputs
  analogWrite(motorLeftPWM, leftSpeed);
  analogWrite(motorRightPWM, rightSpeed);
    
}

void motorState(int s){
  // This is a simple function that puts the motors in one of the 4 states present in the test code, with states 1 and 2 corrisponding to one motor and states 3 and 4 corrisponding to the other. 
  // Which state maps to which direction of which joystick is done with the variables before setup and is done based on the outputs of the test code.
  if (s = 1) {
    digitalWrite(motor1pin1, HIGH);
    digitalWrite(motor1pin2, LOW);
  }
  else if (s = 2) {
    digitalWrite(motor1pin1, LOW);
    digitalWrite(motor1pin2, HIGH);
  }
  else if (s = 3) {
    digitalWrite(motor2pin1, HIGH);
    digitalWrite(motor2pin2, LOW);
  }
  else if (s = 4) {
    digitalWrite(motor2pin1, LOW);
    digitalWrite(motor2pin2, HIGH);
  }
}
