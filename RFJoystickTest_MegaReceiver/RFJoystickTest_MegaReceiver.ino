#include <Servo.h>
#include <RF24.h>

// Servo pin definitions
#define PIN_SERVO1 A2
#define PIN_SERVO2 A3

// LED pin definition
#define PIN_LED 3

// Radio pin definitions
#define PIN_CE  7
#define PIN_CSN 8

// Value to hold servo angle
int iServo1Angle = 0;
int iServo2Angle = 0;

// Servo definition
Servo servo1;
Servo servo2;

// Struct to hold received joystick information
struct JoystickData {
  int iVRX;
  int iVRY;
  int iSW;
};

// Struct to hold the information coming from the other Arduino
JoystickData receivedJoystickData = {0, 0, 0};

/******* RADIO SETUP *********/
// Adddress to transmit to
uint8_t RXAddress[6] = "XXXXX";

// Radio initialization
RF24 radio(PIN_CE, PIN_CSN);

// Loops forever
void loopForever(){
  while(1){}
}

void setup() {
  // Serial initialization
  Serial.begin(115200);

  // Servo attach
  servo1.attach(PIN_SERVO1);
  servo2.attach(PIN_SERVO2);

  // LED pin setup
  pinMode(PIN_LED, OUTPUT);

  // Radio setup
  // Validate that we can talk with the radio through SPI
  if(!radio.begin()) {
    Serial.println("Radio SPI interface not available");
    loopForever();
  }

  // Set power amplification to low
  radio.setPALevel(RF24_PA_LOW);

  // Set maximum payload size
  radio.setPayloadSize(sizeof(JoystickData));

  // Set the TX address that we want to read from
  radio.openReadingPipe(1, RXAddress);

  // Set the radio in RX mode
  radio.startListening();
}

void loop() {
  // Holds the value of the pipe that we're reading from
  uint8_t pipe = 0;

  // If we've received data from the radio
  if(radio.available(&pipe)){
    uint8_t iSize = radio.getPayloadSize();
    radio.read(&receivedJoystickData, iSize);

    // Print joystick values
    Serial.print("Received from pipe ");
    Serial.print(pipe);
    Serial.print(": {");
    Serial.print("iVRX: ");
    Serial.print(receivedJoystickData.iVRX);
    Serial.print(", ");
    Serial.print("iVRY: ");
    Serial.print(receivedJoystickData.iVRY);
    Serial.print(", ");
    Serial.print("iSW: ");
    Serial.print(receivedJoystickData.iSW);
    Serial.print(" }");
    Serial.println();
  }


  iServo1Angle = map(receivedJoystickData.iVRX, 0, 1023, 0, 180);
  iServo2Angle = map(receivedJoystickData.iVRY, 0, 1023, 0, 180);

  servo1.write(iServo1Angle);
  servo2.write(iServo2Angle);

  digitalWrite(PIN_LED, (receivedJoystickData.iSW == 1) ? 0 : 1);
}