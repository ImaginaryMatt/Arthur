#include <WiFi.h>
#include <WiFiUdp.h>
#include <Stepper.h>



// Set Wi-Fi parameters here
// int Location = 2;
// switch (Location) {
//   case 1:
    // const char* ssid = "ArthurNet";
    // const char* password = "TheCakeIsALie";
  //   break;
  // case 2:
    const char* ssid = "Routers of Rohan";
    const char* password = "OneDoesNotSimplyLoginToMordor";
    // break;
    // }

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

void setup() {
   Serial.begin(115200);
  Serial.println("Shit!");
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(IN5, OUTPUT); pinMode(IN6, OUTPUT);
  pinMode(IN7, OUTPUT); pinMode(IN8, OUTPUT);
  
  pinMode (21, HIGH);
  //setupPWM(ENA, 0);
  //setupPWM(ENB, 1);

  //Stepper leftDrive = Stepper(headMotorSteps, IN1, IN2);
  //Stepper rightDrive = Stepper(driveMotorSteps, IN3, IN4);
  Stepper head = Stepper(headMotorSteps, IN7, IN8);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("Connected!");
  udp.begin(udpPort);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Fuck!");
  /*
char buffer[64];
  int packetSize = udp.parsePacket();
  Serial.println(packetSize);
  if (packetSize) {
    int len = udp.read(buffer, sizeof(buffer));
    buffer[len] = '\0';
    //commandArray[22];

    int motor_l = 0, motor_r = 0, joy_l = 0, joy_r = 0, l = 0, r = 0;
    int b0 = 0, b1 = 0, b2 = 0, b3 = 0, b4 = 0, b5 = 0, b6 = 0, b7 = 0, b8 = 0, b9 = 0, b10 = 0, b11 = 0, b12 = 0, b13 = 0, b14 = 0, b15 = 0;
    Serial.println(sscanf(buffer, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", &motor_l, &motor_r, &joy_l, &joy_r, &b0, &b1, &b2, &b3, &b4, &b5, &b6, &b7, &b8, &b9, &b10, &b11, &b12, &b13, &b14, &b15));
    Serial.println(motor_l);
    //controlHead(head, b0, b1);
  }
*/
  char buffer[256];

int packetSize = udp.parsePacket();
Serial.print("Packet size: ");
Serial.println(packetSize);

if (packetSize > 0) {
  Serial.println("before read");

  int len = udp.read(buffer, sizeof(buffer) - 1); // leave space for null
  Serial.println("After read udp");

  if (len > 0) {
    buffer[len] = '\0'; // null-terminate safely

    int motor_l = 0, motor_r = 0, joy_l = 0, joy_r = 0;
    int b[16] = {0};

    int parsed = sscanf(buffer, "%d,%d,%d,%d,"
                                 "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                        &motor_l, &motor_r, &joy_l, &joy_r,
                        &b[0], &b[1], &b[2], &b[3], &b[4], &b[5],
                        &b[6], &b[7], &b[8], &b[9], &b[10], &b[11],
                        &b[12], &b[13], &b[14], &b[15]);

    Serial.print("Parsed fields: ");
    Serial.println(parsed);
    Serial.print("motor_l: ");
    Serial.println(motor_l);

    // Example safety: if parsed < expected, ignore packet
    if (parsed < 22) {
      Serial.println("Warning: incomplete or invalid packet");
      return;
    }

    // controlHead(head, b[0], b[1]);
  }
}

}

void controlHead (Stepper, int left, int right) {
  
}
