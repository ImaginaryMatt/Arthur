
// Arbitrary pin values have been assigned. These values should be adjusted to match the wiring of the installed Arduino
int motor1pin1 = 2;
int motor1pin2 = 16;
int motor1PWM = 10;

int motor2pin1 = 15;
int motor2pin2 = 17;
int motor2PWM = 11;

void setup() {
  Serial.begin(9600);
  
  // Defining all of the pinModes for motor control
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor1PWM, OUTPUT);
  
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);
  pinMode(motor2PWM, OUTPUT);

  //Setting an arbitrary motor speed
  analogWrite(motor1PWM, 125);
  analogWrite(motor2PWM, 125);
}

void loop() {
  // This code, starting with motor 1, will loop through each motor running in each direction for 5 seconds at a time with a 5 second pause between each motor's activation 
  // so that we can identify which direction of which motor should be mapped to each joystick

   // Motor 1 Direction 1
   digitalWrite(motor1pin1, HIGH);
   digitalWrite(motor1pin2, LOW);
   Serial.print("State 1: Motor 1 - Pin 1 High - Pin 2 Low");
   delay(5000);

   // Motor Off
   digitalWrite(motor1pin1, LOW);
   digitalWrite(motor1pin2, LOW);
   Serial.print("Motor 1 Off");
   delay(5000);

   //Motor 1 Direction 2
   digitalWrite(motor1pin1, LOW);
   digitalWrite(motor1pin2, HIGH);
   Serial.print("State 2: Motor 1 - Pin 1 Low - Pin 2 High");
   delay(5000);

   //Motor Off
   digitalWrite(motor1pin1, LOW);
   digitalWrite(motor1pin2, LOW);
   Serial.print("Motor 1 Off");
   delay(5000);

   // Motor 2 Direction 1
   digitalWrite(motor2pin1, HIGH);
   digitalWrite(motor2pin2, LOW);
   Serial.print("State 3: Motor 2 - Pin 1 High - Pin 2 Low");
   delay(5000);

   //Motor Off
   digitalWrite(motor2pin1, LOW);
   digitalWrite(motor2pin2, LOW);
   Serial.print("Motor 2 Off");
   delay(5000);

   // Motor 2 Direction 2
   digitalWrite(motor2pin1, LOW);
   digitalWrite(motor2pin2, HIGH);
   Serial.print("State 4: Motor 2 - Pin 1 Low - Pin 2 High");
   delay(5000);

   //Motor Off
   digitalWrite(motor2pin1, LOW);
   digitalWrite(motor2pin2, LOW);
   Serial.print("Motor 2 Off");
   delay(5000);
}
