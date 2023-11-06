#include <Servo.h>
#include <RF24.h>

// Joystick pin definitions
#define PIN_VRX A0
#define PIN_VRY A1
#define PIN_SW  2

// Radio pin definitions
#define PIN_CE  7
#define PIN_CSN 8

/******* JOYSTICK SETUP *********/
// Struct to hold information about the current state of the joystick
struct JoystickData {
  int iVRX;
  int iVRY;
  int iSW;
};

// Values to hold joystick position and switch button
JoystickData myJoystick = { 0, 0, 0 };

/******* RADIO SETUP *********/
// Adddress to transmit to
uint8_t TXAddress[6] = "XXXXX";

// Radio initialization
RF24 radio(PIN_CE, PIN_CSN);

// Infinite loop
void loopForever(){
  while(1){}
}

void setup() {
  // Serial initialization
  Serial.begin(115200);

  // Joystick pin setup
  pinMode(PIN_VRX, INPUT);
  pinMode(PIN_VRY, INPUT);
  pinMode(PIN_SW, INPUT_PULLUP);

  // Validate that we can talk with the radio through SPI
  if(!radio.begin()) {
    Serial.println("Radio SPI interface not available");
    loopForever();
  }

  // Set power amplification to low
  radio.setPALevel(RF24_PA_LOW);

  // Set maximum payload size
  radio.setPayloadSize(sizeof(JoystickData));

  // Set the TX address that we want to write to
  radio.openWritingPipe(TXAddress);

  // Set the radio in TX mode
  radio.stopListening();
}

void loop() {
  // Read the values from  the joystick
  myJoystick.iVRX = analogRead(PIN_VRX);
  myJoystick.iVRY = analogRead(PIN_VRY);
  myJoystick.iSW = digitalRead(PIN_SW);

  
  // Transmit joystick information
  bool bTransmitSuccess = radio.write(&myJoystick, sizeof(myJoystick));

  // Check that we transmitted succesfully
  if(bTransmitSuccess){
    // Print that we transmitted successfully
    Serial.println("Successful transmit!");
    // Print joystick values
    Serial.print("iVRX: ");
    Serial.print(myJoystick.iVRX);
    Serial.print(", ");
    Serial.print("iVRY: ");
    Serial.print(myJoystick.iVRY);
    Serial.print(", ");
    Serial.print("iSW: ");
    Serial.print(myJoystick.iSW);
    Serial.println();

    // Delay for a bit, probably get rid of this later
    delay(1);
  } else {
    // Print that we did not succesfully transmit
    Serial.println("Transmit failed");
  }
}
