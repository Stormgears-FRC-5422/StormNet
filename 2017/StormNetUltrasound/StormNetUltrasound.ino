//Motor Slave for Stormgears SmartCart

#include <Wire.h>
#include "Maxbotix.h"
#define US_SLAVE 8  // ultrasound I2C Bus ID 8

//Maxbotix rangeSensorPW(8, Maxbotix::PW, Maxbotix::LV, Maxbotix::BEST);

#ifdef MAXBOTIX_WITH_SOFTWARE_SERIAL
Maxbotix rangeSensorTX(6, Maxbotix::TX, Maxbotix::LV, Maxbotix::MEDIAN);
#endif

//Maxbotix rangeSensorAD(A0, Maxbotix::AN, Maxbotix::LV, Maxbotix::BEST, 9);


const int ledPin =  13;      // the number of the LED pin
const int enablePin = 7;  // Enable pin for ultraSonic sensor
int ledState = LOW;             // ledState used to set the LED
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;        // will store last time LED was updated
long blinkinterval = 100;           // interval at which to blink (milliseconds)
const unsigned long int i2cHeartbeatTimeout = 5000; // master must talk to slave within this number of milliseconds or LED will revert to fast pulse
unsigned long int currenti2c = 0;
unsigned long int previousi2c = 0;
float usINPUT1;
float usINPUT2;
volatile byte* INPUT1FloatPtr;
volatile byte* INPUT2FloatPtr;

unsigned int CmdByte = 0;         // incoming serial byte
//unsigned int Left_Pot,Left_PWM,Right_Pot,Right_PWM;

void setup() {
  pinMode(ledPin, OUTPUT); // set the digital pin as output
  Wire.begin(US_SLAVE);        // join i2c bus
  Wire.onRequest(requestEvent);   // register event
  Wire.onReceive(receiveEvent);   // register event
  Serial.begin(9600); // start serial port at 9600 bps and wait for port to open

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println();
  Serial.println("US Sensor Slave Controller Diagnostic System");
  Serial.println("Hit '?' for Help");
  Serial.print("US Sensor Slave ==> ");
}

void loop() { //main user command loop

  getDistance();

  //code to flash heartbeat LED =============
  currenti2c = currentMillis = millis();
  if (currenti2c - previousi2c >= i2cHeartbeatTimeout) blinkinterval = 100;
  if (currentMillis - previousMillis >= blinkinterval) {
    previousMillis = currentMillis;    // save the last time you blinked the LED
    if (ledState == LOW) {// if the LED is off turn it on and vice-versa
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    digitalWrite(ledPin, ledState);  // set the LED with the ledState of the variable
  }

  //main motor control code here ===============

  if (Serial.available() > 0) { //diagnostic menu system starts here
    CmdByte = Serial.read(); // get user command
    Serial.println(char(CmdByte));
    switch (CmdByte) { //act on command
      case 'd':
        Serial.println();
        Serial.println("   Motor Control Pot and PWM Command Display. Hit any key to exit...");
        Serial.println();
        Serial.println("      Left Pot\t\tLeft PWM\t\tRight Pot\tRight PWM");
        while (Serial.available() == 0) {
          //Left_Pot=analogRead(LEFT_POT_AIN);Right_Pot=analogRead(RIGHT_POT_AIN);
          //Left_PWM = map(Left_Pot, 0, 1023, 0, 180); Right_PWM = map(Right_Pot, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
          //=====>        leftservo.write(Left_PWM); rightservo.write(Right_PWM); delay(15);    // sets the servo position according to the scaled value
          Serial.print("      ");
          //Serial.print(Left_Pot); //print left pot value
          Serial.print("\t\t");
          //Serial.print(Left_PWM); //print left pwm value
          Serial.print("\t\t\t");
          //Serial.print(Right_Pot); //print right pot value
          Serial.print("\t\t");
          //Serial.print(Right_PWM); //print right pwm value
          Serial.print("\r");
        }
        Serial.read(); //clear the char entered to break out of display mode
        Serial.println();
        break;
      case 'b':  //bumper test
        //bumper test and display code here
        break;
      case '?':  display_motor_slave_help(); break; //help
      default:  Serial.println("   Not a recognized command."); break;
    }

    Serial.println();
    Serial.print("Motor Slave ==> ");
  }
}

void getDistance()
{
  unsigned long start;

  Serial.println("Reading...");

  // PW

#ifdef MAXBOTIX_WITH_SOFTWARE_SERIAL
  // TX
  start = millis();
  Serial.print("TX (MEDIAN): ");
  Serial.print(rangeSensorTX.getRange());
  Serial.print("cm - ");
  Serial.print(millis() - start);
  Serial.print("ms - ");
  printArray(rangeSensorTX.getSample(), rangeSensorTX.getSampleSize());
  Serial.print(" - Highest Mode: ");
  Serial.print(rangeSensorTX.getSampleMode(true));
  Serial.print(" - Lowest Mode: ");
  Serial.print(rangeSensorTX.getSampleMode(false));
  Serial.print(" - Median: ");
  Serial.print(rangeSensorTX.getSampleMedian());
  Serial.print(" - Best: ");
  float usSensorCM = rangeSensorTX.getSampleBest();
  usINPUT1 = rangeSensorTX.getSampleBest();    //your code here
  usINPUT2 = rangeSensorTX.getSampleBest();     //your code here

  //  Serial.print(rangeSensorTX.getSampleBest());
  Serial.print(usSensorCM);
  Serial.println();
#endif

  Serial.println();
  //delay(5000);
}

void printArray(float* array, uint8_t array_size) {
  Serial.print("[");
  for (int i = 0; i < array_size; i++) {
    Serial.print(array[i]);
    if (i != array_size - 1) {
      Serial.print(", ");
    }
  }
  Serial.print("]");
}

//================================
void display_motor_slave_help() {
  Serial.println();
  Serial.println("===== Motor Slave Help =====");
  Serial.println("    d:  Motor Pot and PWM value display (left and right)");
  Serial.println("    b:  Bumper test");
  Serial.println("    ?:  Help");
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  byte* Data;
  INPUT1FloatPtr = (byte*) &usINPUT1;
  INPUT2FloatPtr = (byte*) &usINPUT2;
  Data[0] = INPUT1FloatPtr[0];
  Data[1] = INPUT1FloatPtr[1];
  Data[2] = INPUT1FloatPtr[2];
  Data[3] = INPUT1FloatPtr[3];
  Data[4] = INPUT2FloatPtr[0];
  Data[5] = INPUT2FloatPtr[1];
  Data[6] = INPUT2FloatPtr[2];
  Data[7] = INPUT2FloatPtr[3];
  Wire.write(Data, 8);
}

//================================
void oldrequestEvent() { // handle i2c request from master
  blinkinterval = 1000;
  previousi2c = currenti2c;

  float usSensorVal;

  //Wire.write((usSensorCM >> 8) & 0x0000ffff); // Distance high byte
  //Wire.write((usSensorCM) & 0x0000ffff); // Distance low byte
  //Wire.write((Left_Pot>>8)&0x0000ffff);  // left pot high byte
  //Wire.write((Left_Pot)&0x0000ffff);  // left pot low byte
  //Wire.write((Left_PWM)&0x0000ffff);  // left pwm byte
  //Wire.write((Right_Pot>>8)&0x0000ffff);  // right pot high byte
  //Wire.write((Right_Pot)&0x0000ffff);  // right pot low byte
  //Wire.write((Right_PWM)&0x0000ffff);  // right pwm byte
}
//================================
void receiveEvent(int howMany) { // handles i2c write event from master
  blinkinterval = 1000;
  previousi2c = currenti2c;

  while (Wire.available()) { // loop until no more bytes available
    char c = Wire.read(); // receive byte as a character
    switch (c) { //act on command
      case 'F': //flash heartbeat LED fast
        blinkinterval = 250;
        while (Wire.available()) c = Wire.read(); //clean out i2c command buffer
        break;
      case 'S': //flash heartbeat LED slow
        blinkinterval = 1000;
        while (Wire.available()) c = Wire.read(); //clean out i2c command buffer
        break;
      default:
        while (Wire.available()) c = Wire.read(); //clean out i2c command buffer
        break;
    }
  }
}

bool wait(int waitDelay) {
  int waitStart = millis();
  int waitNow = millis();
  while (((waitNow - waitStart) < waitDelay)) {
    //    modeCheck();
    waitNow = millis();
  }
  //  return(modeChanged);
}
