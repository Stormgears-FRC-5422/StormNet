
#include <Adafruit_NeoPixel.h>


#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      14

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixelSolid = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRBW + NEO_KHZ800);
int delayval = 500; // delay for half a second
int incomingByte = 0;
int pulIn = 12;
unsigned long duration;
int switchPin = 11;
int buttonState;
int oldButtonState;

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  Serial.begin(9600);
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code
  pixels.begin(); // This initializes the NeoPixel library.
  pinMode(pulIn, INPUT);
  
    pinMode(switchPin, INPUT);
}
int determinator=1;
void loop() {
  
   buttonState= digitalRead(switchPin);
  
  if(buttonState==HIGH){
    //Serial.println("On");
  }
  else if(buttonState==LOW){
    //Serial.println("IT IS ON LOW");
  }
  if(oldButtonState!=buttonState &&oldButtonState!=LOW){
    if(determinator<8){
      determinator+=1;
    }
    else{
      determinator=1;
    }
    Serial.println("Changed color");
    Serial.println(determinator);
  }
  oldButtonState=buttonState;
  
switch(determinator){
  //IMPLEMENT DIFFERENT PATTERNS FOR DIFFERENT CASES.
   case 1:
     setColor(0,150,150); // Moderately bright greenish blue color.
      //colorWipe(pixels.Color(0, 255, 0), 50); // Green
      break;
   case 2:
      setColor(150,0,150); // Moderately bright purple color.
      //theaterChase(pixels.Color(127, 0, 0), 10); // Red
      break;
   case 3: 
       setColor(150,150,150); // Moderately orangeish color.
      //theaterChase(pixels.Color(127, 0, 0), 100); // Red
      break;      
   case 4: 
      setColor(0,150,150); // Moderately bright greenish blue color.
      break;
   case 5:
      setColor3(150,0,150); // Moderately bright purple color.
      break;
   case 6:
      setColor(150,150,0); // Moderately orangeish color.
      break;
   case 7:
      setColor2(200,150,0); // Moderately brownish color.
      break;
   case 8:
       setColor2(150,0,0); // Moderately brownish color.
      break; 
}
}


void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<pixels.numPixels(); i++) {
    pixels.setPixelColor(i, c);
    pixels.show();
  }
}
void setColor(int R, int G, int B)
{
    for(int i=0;i<NUMPIXELS;i++){
      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      pixelSolid.setPixelColor(i, pixelSolid.Color(R,G,B));
      pixelSolid.show(); // This sends the updated pixel color to the hardware.
      }
}
void setColor2(int R, int G, int B)
{
  int RDelta=R/NUMPIXELS;
  int GDelta=(R-G)/NUMPIXELS;
  //Serial.println(RDelta);
    for(int i=0;i<NUMPIXELS;i++){
      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      pixelSolid.setPixelColor(i, pixelSolid.Color(R-RDelta*i,G+GDelta*i,B));
      pixelSolid.show(); // This sends the updated pixel color to the hardware.
      }
     
}

void setColor3(int R, int G, int B)
{
  int RDelta=R/NUMPIXELS;
  int GDelta=(R-G)/NUMPIXELS;
  //Serial.println(RDelta);
    for(int i=0;i<NUMPIXELS;i++){
      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      if(i%2==0){
        pixelSolid.setPixelColor(i, pixelSolid.Color(R,G,B));}
      else{
       pixelSolid.setPixelColor(i, pixelSolid.Color(G,R,B));}
      pixelSolid.show(); // This sends the updated pixel color to the hardware.
      }
     
}
//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, c);    //turn every third pixel on
      }
      pixels.show();

      delay(wait);

      for (uint16_t i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

