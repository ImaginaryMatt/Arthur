// ESP32 UDP Controller

#include <WiFi.h>
#include <WiFiUdp.h>
#include <Stepper.h>
//#include <ESP32Servo360.h>
#include <ESP32Servo.h>

// Set Wi-Fi parameters here
const char* ssid = "ArthurNet";
const char* password = "TheCakeIsALie";

WiFiUDP udp;
const int udpPort = 1234;

// Drivetrain is dc Motor, arms are servo, head is bipolar stepper
// Change pin numbers as needed
// order of numbers are 49,8,43,12,24,14,5,6,7,22,21,23,
const int ENA = 5, IN1 = 6, IN2 = 7; //PWM, left drive motor
const int ENB = 23, IN3 = 22, IN4 = 21; //PWM, right drive motor
const int HeadENA = 49, HeadENB = 14; // Enable Head
const int IN5 = 8, IN6 = 43; // Stepper 1
const int IN7 = 12, IN8 = 24; // Stepper 2

const int headMotorSteps = 123;

const int driveMotorMaxRPM = 12;
const int headMotorMaxRPM = 12;

const int servoMin = 0, servoMax = 100; // Defines servo parameters TODO: add actual values

// Creating Servo Objects
/*
ESP32Servo360  servo_Arm_L;
ESP32Servo360  servo_Arm_R;
ESP32Servo360  servo_claw_grab;
ESP32Servo360  servo_claw_lift
*/
//Drive systems pre-setup
// Arbitrary pin values have been assigned. These values should be adjusted to match the wiring of the installed Arduino
int motor1pin1 = IN1;
int motor1pin2 = IN2;

int motor2pin1 = IN3;
int motor2pin2 = IN4;

int motorLeftPWM = ENA;
int motorRightPWM = ENB;

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
/*
void setupPWM(int pin, int channel) {
  ledcSetup(channel, 1000, 8);  // 1 kHz, 8-bit
  ledcAttachPin(pin, channel);
}
*/
void setup() {
  Serial.begin(115200);
  
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(IN5, OUTPUT); pinMode(IN6, OUTPUT);
  pinMode(IN7, OUTPUT); pinMode(IN8, OUTPUT);
  
  setupPWM(ENA, 0);
  setupPWM(ENB, 1);
  setupPWM(HeadENA, 0);
  setupPWM(HeadENB, 1);

  Stepper leftDrive = Stepper(driveMotorSteps, IN1, IN2);
  Stepper rightDrive = Stepper(driveMotorSteps, IN3, IN4);
  Stepper head = Stepper(headMotorSteps, IN7, IN8);
  servo_Arm_L.attach(IN5);
  servo_Arm_R.attach(IN6);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("Connected!");
  udp.begin(udpPort);
}

void loop() {
  char buffer[64];
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(buffer, sizeof(buffer));
    buffer[len] = '\0';

    int motor_l = 0, motor_r = 0, joy_l = 0, joy_r = 0, l = 0, r = 0;
    int b0 = 0, b1 = 0, b2 = 0, b3 = 0, b4 = 0, b5 = 0, b6 = 0, b7 = 0, b8 = 0, b9 = 0, b10 = 0, b11 = 0, b12 = 0, b13 = 0, b14 = 0, b15 = 0;
    sscanf(buffer, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", &motor_l, &motor_r, &joy_l, &joy_r, &b0, &b1, &b2, &b3, &b4, &b5, &b6, &b7, &b8, &b9, &b10, &b11, &b12, &b13, &b14, &b15);

    controlDrivetrain(joy_l, joy_r, l_DZ_LB, l_DZ_UB, r_DZ_LB, r_DZ_UB, speedLimit);
    controlHead(head,joy_x);
    controlArms(servo_L,servo_R,b0,b1); //a, b activate left, right arm servo
    //controlMotor(left, ENA, IN1, IN2, 0);
    //controlMotor(right, ENB, IN3, IN4, 1);
  }
}

void controlDrivetrain(int leftJoystick, int rightJoystick, int leftDeadzoneLowerBound, int leftDeadzoneUpperBound, int rightDeadzoneLowerBound, int rightDeadzoneUpperBound, int speedLimit) {

//  // This takes in the direct joystick values from the sent packet
//  int leftJoystick = 0;
//  int rightJoystick = 0;
//
//  // Also sent in the packet will be deadzone bounds for the left and right stick. These could be combined into a singualr set of bounds, but I left them seperate
//  // for maximum custimizability
//  int leftDeadzoneLowerBound = 470;
//  int leftDeadzoneUpperBound = 550;
//
//  int rightDeadzoneLowerBound = 470;
//  int rightDeadzoneUpperBound = 550;
//
//  // This is the final variable sent for motor control from the driver station. This value should allow us to adjust the maximum speed of the robot remotely
//  int speedLimit = 255;

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

    // Below is an older version of the code that has the interpretation of the joystick data done on the driver side to minimize the number of needed values
    // If this code is instated over the current version, please update the function input variables to be (int leftSpeed, int leftDIR, int rightSpeed, int rightDIR)
    
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

void controlHead(Stepper head, int speed) {
  speed *= headMotorMaxRPM;
  speed /= 100;
  head.setSpeed(speed);
}

void controlArms(Servo left, Servo right, int l_raise, int r_raise) {
  if (l_raise > 0) {
    left.write(180);
  } else {
    left.write(0);
  }
  if (r_raise > 0) {
    right.write(180);
  } else {
    right.write(0);
  }
}

void controlMotor(int speed, int pwmPin, int in1, int in2, int channel) {
  bool dir = speed >= 0;
  int pwm = min(abs(speed), 100) * 2.55;

  digitalWrite(in1, dir);
  digitalWrite(in2, !dir);
  ledcWrite(channel, pwm);
}
