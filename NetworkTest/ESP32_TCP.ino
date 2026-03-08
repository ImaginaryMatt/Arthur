// This is an arduino ino file that sets up a tcp server and takes commands.
// The commands are to move motors connected to the arduino.
 
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <Servo.h>
#include <Stepper.h>
#include <Arduino.h>
 
const char* ssid = "Routers of Rohan";
const char* password = "OneDoesNotSimplyLoginToMordor";

WiFiServer server(80);

// Define motors
int motorLeftEnablePin = 9;  // PWM pin for left motor
int motorRightEnablePin = 10; // PWM pin for right motor
int motorLeftPin1 = 11;
int motorLeftPin2 = 12;
int motorRightPin1 = 13;
int motorRightPin2 = 14;
int motorHeadEnablePin = 15;
int motorHeadEnablePin2 = 16;
int motorHeadPin1 = 15;
int motorHeadPin2 = 16;
int motorHeadPin3 = 17;
int motorHeadPin4 = 18;
int motorArmLeftPin = 19;
int motorArmRightPin = 20;
int motorExtendedPin = 21;
int motorClawPin = 22;

Stepper head(123, motorHeadPin1, motorHeadPin2, motorHeadPin3, motorHeadPin4); // Example stepper motor on pins 12 and 24
 
void setup() {
  Serial.begin(115200);
  motor1.attach(9);  // Attach motor1 to pin 9
  motor2.attach(10); // Attach motor2 to pin 10
  motor3.attach(11); // Attach motor3 to pin 11
 
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
 
  server.begin();
}
 
// Process incoming commands to control motors
void processCommand(String command) {
    int motorNum = command.charAt(0) - '0'; // First character is motor number
    int value = command.substring(2).toInt(); // Rest is the angle
    switch (motorNum) {
      case 1: // Left Motor
        motor1.write(value);
        Serial.print("Motor 1 set to ");
        Serial.println(value);
        break;
      case 2: // Right Motor
        motor2.write(value);
        Serial.print("Motor 2 set to ");
        Serial.println(value);
        break;
      case 3: // Head Motor
        motor3.write(value);
        Serial.print("Motor 3 set to ");
        Serial.println(value);
        break;
      case 4: // Left Arm Motor
        motor4.write(value); 
        Serial.print("Motor 4 set to ");
        Serial.println(value);
        break;
      case 5: // Right Arm Motor
        motor5.write(value);
        Serial.print("Motor 5 set to ");
        Serial.println(value);
        break;
      case 6: // Extended Motor
        motor6.write(value);
        Serial.print("Motor 6 set to ");
        Serial.println(value);
        break;
      case 7: // Claw Motor
        motor7.write(value);
        Serial.print("Motor 7 set to ");
        Serial.println(value);
        break;
        case 8: // Emergency Stop Motor
        motor1.write(90); // Stop all motors
        motor2.write(90); 
        motor3.write(90);
        motor4.write(90);
        motor5.write(90);
        motor6.write(90);
        motor7.write(90);
      default:
        Serial.println("Invalid motor number");
    }
}
 
void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client Connected");
    String command = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          Serial.print("Received Command: ");
          Serial.println(command);
          processCommand(command);
          command = "";
        } else {
          command += c;
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected");
  }
}