// This is an arduino ino file that sets up a tcp server and takes commands.
// The commands are to move motors connected to the arduino.

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <ESP32Servo.h>
#include <Stepper.h>
#include <Arduino.h>

const char *ssid = "ArthurNet";
const char *password = "TheCakeIsALie";

// Start Wifi
WiFiServer server(80);

// Define motors TODO: Change pin numbers as needed
const int motorLeftEnablePin = 10;  // PWM pin for left motor
const int motorRightEnablePin = 11; // PWM pin for right motor
const int motorLeftPin1 = 2;
const int motorLeftPin2 = 16;
const int motorRightPin1 = 15;
const int motorRightPin2 = 17;
const int motorHeadPin1 = 44;
const int motorHeadPin2 = 43;
const int motorHeadPin3 = 3;
const int motorHeadPin4 = 38;
const int motorArmLeftPin = 4; // Placeholder pin number
const int motorArmRightPin = 5; // Placeholder pin number
const int motorExtendedPin = 6; // Placeholder pin number
const int motorClawPin = 8; // Placeholder pin number

const int speedLimit = 255; // Limit on max speed of robot drive train
const int headSpeed = 30; // Speed for head stepper in rpm
const int stepsPerCommand = 10; //Number of steps per command sent. Needs to be set based on refresh rate
int  stepCount = 0; // Internal counter for number of steps

//Calculation values for arm and grabber speeds
const int gearTeeth = 20; //Number of teeth on the driver gear
const int rackTeeth = 28; //Number of teeth we need to travel on the rack for full extension
const float rotationsPerExtension = rackTeeth/gearTeeth;
const float extensionTime = 3; //seconds
const int rackAndPinionSpeed = (rotationsPerExtension/extensionTime) * 60;

const float armMaxRotation = 0.5; //Half a rotation
const float rotationTime = 0.5; //Half a second
const int armSpeed = (armMaxRotation/rotationTime) * 60;

const int fingerTeeth = 18; //Number of teeth we need to travel on the grabber finger for full extension
const float rotationsPerGrasp = fingerTeeth/gearTeeth;
const float graspTime = 1; //Seconds
const int graspSpeed = (rotationsPerGrasp/graspTime) * 60;

// Microsecond values for continuous servo control
const int armCW = map(armSpeed,0,140,1480,1280);
const int armCCW = map(armSpeed,0,140,1520,1720);
const int extendCW = map(rackAndPinionSpeed,0,140,1480,1280);
const int extendCCW = map(rackAndPinionSpeed,0,140,1520,1720);
const int graspCW = map(graspSpeed,0,140,1480,1280);
const int graspCCW = map(graspSpeed,0,140,1520,1720);
const int STOP = 1500;

const String killItKillIt = "81";

// initializing software kill switch
bool killSwitch = false;

Stepper head(200, motorHeadPin1, motorHeadPin2, motorHeadPin3, motorHeadPin4); // Example stepper motor on pins 12 and 24
 // Creating Servo Objects
  Servo leftArm;
  Servo rightArm;
  Servo Extended;
  Servo Claw;

void setup() {
  Serial.begin(115200);

   pinMode(motorLeftPin1, OUTPUT);
  pinMode(motorLeftPin2, OUTPUT);
  pinMode(motorRightPin1, OUTPUT);
  pinMode(motorRightPin2, OUTPUT);
  //pinMode(motorHeadEnablePin1, OUTPUT);
 // pinMode(motorHeadEnablePin2, OUTPUT);
  pinMode(motorHeadPin1, OUTPUT);
  pinMode(motorHeadPin2, OUTPUT);
  pinMode(motorHeadPin3, OUTPUT);
  pinMode(motorHeadPin4, OUTPUT);
  pinMode(motorArmLeftPin, OUTPUT);
  pinMode(motorArmRightPin, OUTPUT);
  pinMode(motorExtendedPin, OUTPUT);
  pinMode(motorClawPin, OUTPUT);
/*  // Set PWM Properties
  ledcAttach(motorLeftEnablePin, 1000, 8); // Channel 0, 1 kHz, 8-bit
  ledcAttach(motorRightEnablePin, 1000, 8); // Channel 1, 1 kHz, 8-bit
  // Attach PWM pins
  ledcAttach(motorLeftEnablePin, 0);   // Attach motorLeftEnablePin to channel 0
  ledcAttach(motorRightEnablePin, 1); // Attach motorRightEnablePin to channel 1
  motorLeft.attach(9);   // Attach motorLeft to pin 9
  motorRight.attach(10); // Attach motorRight to pin 10
*/

  head.setSpeed(headSpeed); //Setting stepper speed
 
  // Attach servos to their respective pins
  leftArm.attach(motorArmLeftPin);
  rightArm.attach(motorArmRightPin);
  Extended.attach(motorExtendedPin);
  Claw.attach(motorClawPin);

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

// claw functions
void clawOut(Servo Extended) {
// starts rotating servo CCW to open door and release claw 
Extended.writeMicroseconds(extendCCW);
delay(extensionTime*1000);
// stops claw
Extended.writeMicroseconds(STOP);
}

void clawIn(Servo Extended) {
// starts rotating servo CW to close door and place claw back inside
Extended.writeMicroseconds(extendCW);
delay(extensionTime*1000);
// stops claw
Extended.writeMicroseconds(STOP);
}

void clawGrab(Servo Claw) {
// starts rotating servo CW to grab ball
Claw.writeMicroseconds(graspCW);
delay(graspTime * 1000);
// stops claw
Claw.write(STOP);
}

void clawRelease(Servo Claw) {
// starts rotating servo CCW to release ball
Claw.writeMicroseconds(graspCCW);
delay(graspTime * 10);
// stops claw
Claw.writeMicroseconds(STOP);
}
 
// Process incoming commands to control motors
void processCommand(String command) {
  int motorNum = command.charAt(0) - '0';   // First character is motor number
  int value = command.substring(1).toInt(); // Rest is the angle
  int pwmValue = 0;
  switch (motorNum)
  {
  case 1: // Left Motor
    if (killSwitch == false)
    {
      // Reads pwm value for left motor
      if (value >= 256)
      {
        pwmValue = map(value, 256, 510, 0, speedLimit);
        digitalWrite(motorLeftPin1, 0);
        digitalWrite(motorLeftPin2, 1);
        analogWrite(motorLeftEnablePin, pwmValue);
      }
      else if (value < 256)
      {
        pwmValue = map(value, 255, 0, 0, speedLimit);
        digitalWrite(motorLeftPin1, 1);
        digitalWrite(motorLeftPin2, 0);
        analogWrite(motorLeftEnablePin,pwmValue);
      }
      else
      {
        digitalWrite(motorLeftPin1, 0);
        digitalWrite(motorLeftPin2, 0);
        analogWrite(motorLeftEnablePin, 0);
      }
    }
  break;

  case 2: // Right Motor
    if (killSwitch == false)
    {
      if (value >= 256)
      {
        pwmValue = map(value, 256, 510, 0, speedLimit);
        digitalWrite(motorRightPin1, 0);
        digitalWrite(motorRightPin2, 1);
        analogWrite(motorRightEnablePin, pwmValue);
      }
      else if (value < 256)
      {
        pwmValue = map(value, 255, 0, 0, speedLimit);
        digitalWrite(motorRightPin1, 1);
        digitalWrite(motorRightPin2, 0);
        analogWrite(motorRightEnablePin,pwmValue);
      }
      else
      {
        digitalWrite(motorRightPin1, 0);
        digitalWrite(motorRightPin2, 0);
        analogWrite(motorRightEnablePin, 0);
      }
    }
  break;

  case 3: // Head Stepper Motor
  if (killSwitch == false);
  {
    if (value == 1)
    {
      head.step(stepsPerCommand);
      stepCount = stepCount + stepsPerCommand;
    }
    else if (value == 2)
    {
      head.step(-stepsPerCommand);
      stepCount = stepCount - stepsPerCommand;
    }
    else if (value == 3)
    {
      head.step(-stepCount);
      stepCount = 0;
    }
    else if (value == 4) // Move based on joystick input
    {
    
    }
    else
    {
      head.step(0);
    }
  }
  break;

  case 4: // Left Arm Motor
    if (killSwitch == false)
    {
      if (value == 1) //Corresponds to press of L1
      {
        leftArm.writeMicroseconds(armCW);
      }
      else if (value == 2) //Corresponds to press of L2
      {
        leftArm.writeMicroseconds(armCCW);
      }
      else // Halts motor when no button is pressed
      {
        leftArm.writeMicroseconds(STOP);
      }
    }
  break;

  case 5: // Right Arm Motor
    if (killSwitch == false)
    {
      if (value == 1) //Corresponds to press of R1
      {
        rightArm.writeMicroseconds(armCCW);
      }
      else if (value == 2) //Corresponds to press of R2
      {
        rightArm.writeMicroseconds(armCW);
      }
      else //Halts motor when no button is pressed
      {
        rightArm.writeMicroseconds(STOP);
      }
    }
  break;

  case 6: // Extended Motor Deploy and Retract
    if (killSwitch == false)
    {
      if (value == 1) 
      {
        // clawOut(Extended); // mechanism deploys
        Extended.writeMicroseconds(armCCW);
      } 
      else if (value == 2)
      {
        // clawIn(Extended); //Mechanism retracts
        Extended.writeMicroseconds(armCW);
      }
      else
      {
        Extended.writeMicroseconds(STOP);
      }
    }
  break;

  case 7: // Claw Motor Grab and Release
    if (killSwitch == false)
    {
      if (value == 1) 
      {
        // clawGrab(Claw); // Close claw
        Claw.writeMicroseconds(armCW);
      }
      else if (value == 2) 
      {
        // clawRelease(Claw); // Open claw
        Claw.writeMicroseconds(armCCW);
      }
      else 
      {
        Claw.writeMicroseconds(STOP); // Stops claw from moving
      }
    }
  break;

  /*case 8:             // Emergency Stop Motor
    if (value == 1)
    {
      // Disabling left drive motor
      digitalWrite(motorLeftPin1, 0); //Setting both direction pins to 0 so that the rotation direction is null
      digitalWrite(motorLeftPin2, 0);
      analogWrite(motorLeftEnablePin, 0); //Setting motor speed to 0

      //Disabling right drive motor
      digitalWrite(motorRightPin1, 0); //Setting both direction pins to 0 so that rotation direction is null
      digitalWrite(motorRightPin2, 0);
      analogWrite(motorRightEnablePin, 0); //Setting motor speed to 0

      //Disabling head motor by seting all directional control pins to low. This SHOULD stop movement
      digitalWrite(motorHeadPin1, 0);
      digitalWrite(motorHeadPin2, 0);
      digitalWrite(motorHeadPin3, 0);
      digitalWrite(motorHeadPin4, 0);

      //Disabling arm motors
      leftArm.writeMicroseconds(STOP); //Sending halt command to left arm servo
      rightArm.writeMicroseconds(STOP); //Sending halt command to right arm servo

      //Disabling grabber servos
      Extended.writeMicroseconds(STOP); //Sending halt command to grabber extension servo
      Claw.writeMicroseconds(STOP); //Sending halt command to grabber claw servo

      //Setting internal kill switch value to true
      killSwitch = true;
    }
    else if (value == 2)
    {
      //resetting internal kill switch value to false
      killSwitch = false;
    }*/
   
  default:
    Serial.println("Invalid motor number");
  }
}
 
void loop() {
  WiFiClient client = server.accept();
  if (client)
  {
    Serial.println("New Client Connected");
    String command = "";
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        if (c == '\n')
        {
          Serial.print("Received Command: ");
          Serial.println(command);
          processCommand(command);
          command = "";
        }
        else
        {
          command += c;
        }
      }
    }
    //processCommand(killItKillIt); //sending kill switch command eiher for shutdown or in the event of a disconnect
    client.stop();
    Serial.println("Client Disconnected");
  }
}