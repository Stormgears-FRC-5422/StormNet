#include <Wire.h>
#include <VL53L0X.h>
#include <TI_TCA9548A.h>

//#define XSHUT_pin6 not required for address change
#define XSHUT_pin5 26
#define XSHUT_pin4 25
#define XSHUT_pin3 24
#define XSHUT_pin2 23
#define XSHUT_pin1 22

//ADDRESS_DEFAULT 0b0101001 or 41
//#define Sensor1_newAddress 41 not required address change
#define Sensor2_newAddress 42
#define Sensor3_newAddress 47
#define Sensor4_newAddress 44
#define Sensor5_newAddress 45
#define Sensor6_newAddress 46

VL53L0X Sensor1;
VL53L0X Sensor2;
VL53L0X Sensor3;
VL53L0X Sensor4;
/*  VL53L0X Sensor5;
  VL53L0X Sensor6;
*/
int maxLidars = 4;  // Total number of installed LiDar sensors
int lidarReadings[5] = { 0, 0, 0, 0 };  //  An integer array for storing sensor readings

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void network_loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 1");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
          //          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
          //            int sensorReading = analogRead(analogChannel);
          //            int sensorReading = Sensor1.readRangeContinuousMillimeters();
          for (int lidarSensors = 0; lidarSensors < maxLidars; lidarSensors++) {
            client.print("Sensor ");
            client.print(lidarSensors + 1);
            client.print(": ");
            if (lidarReadings[lidarSensors] > 8180)
              client.print("Out of range");
            else
              client.print(lidarReadings[lidarSensors]);
            //client.print(" is ");
            //client.print(sensorReading);
            client.println("<br />");
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void setup()
{ /*WARNING*/
  //Shutdown pins of VL53L0X ACTIVE-LOW-ONLY NOT 5V TOLERANT. 5V will fry them
  pinMode(XSHUT_pin1, OUTPUT);
  pinMode(XSHUT_pin2, OUTPUT);
  pinMode(XSHUT_pin3, OUTPUT);
  pinMode(XSHUT_pin4, OUTPUT);
  pinMode(XSHUT_pin5, OUTPUT);

  Serial.begin(115200);

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  Wire.begin();
  //Set address of sensor and power up next one
  /*  Sensor6.setAddress(Sensor6_newAddress);
    pinMode(XSHUT_pin5, INPUT);
    delay(10); //For power-up procedure t-boot max 1.2ms "Datasheet: 2.9 Power sequence"
    Sensor5.setAddress(Sensor5_newAddress);
    pinMode(XSHUT_pin4, INPUT);
    delay(10);
    Sensor4.setAddress(Sensor4_newAddress);
  */
  pinMode(XSHUT_pin4, INPUT);
  delay(10);
  Sensor4.setAddress(Sensor4_newAddress);
  Serial.println("Sensor4 address set");
  pinMode(XSHUT_pin3, INPUT);
  delay(10);
  Sensor3.setAddress(Sensor3_newAddress);
  Serial.println("Sensor3 address set");
  pinMode(XSHUT_pin2, INPUT);
  delay(10);
  Sensor2.setAddress(Sensor2_newAddress);
  Serial.println("Sensor2 address set");
  pinMode(XSHUT_pin1, INPUT);
  delay(10);
  //  Sensor1.setAddress(Sensor1_newAddress);

  Sensor1.init();
  Sensor2.init();
  Sensor3.init();
  Sensor4.init();
  /*      Sensor5.init();
        Sensor6.init();
  */
  Sensor1.setTimeout(500);
  Sensor2.setTimeout(500);
  Sensor3.setTimeout(500);
  Sensor4.setTimeout(500);
  /*
    Sensor5.setTimeout(500);
    Sensor6.setTimeout(500);
  */
  // Start continuous back-to-back mode (take readings as
  // fast as possible).  To use continuous timed mode
  // instead, provide a desired inter-measurement period in
  // ms (e.g. sensor.startContinuous(100)).
  Sensor1.startContinuous();
  Sensor2.startContinuous();
  Sensor3.startContinuous();
  Sensor4.startContinuous();
  /*
    Sensor5.startContinuous();
    Sensor6.startContinuous();
  */
}

void I2CScan() {
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(1000);           // wait 5 seconds for next scan
}
void loop()
{
  Serial.println(".");
//  I2CScan();  // Uncomment this to see addresses for all I2C devices on the bus
  lidarReadings[0] = Sensor1.readRangeContinuousMillimeters();
  lidarReadings[1] = Sensor2.readRangeContinuousMillimeters();
  lidarReadings[2] = Sensor3.readRangeContinuousMillimeters();
  lidarReadings[3] = Sensor4.readRangeContinuousMillimeters();
  network_loop();

}
