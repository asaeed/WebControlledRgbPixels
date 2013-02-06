#include "SPI.h"
#include "Adafruit_WS2801.h"

int dataPin  = 2;    // Yellow wire on Adafruit Pixels
int clockPin = 3;    // Green wire on Adafruit Pixels

const int numLights = 25;
uint32_t colorArray[numLights];
int hueArray[numLights];
Adafruit_WS2801 strip = Adafruit_WS2801(25, dataPin, clockPin);

int inByte = -1;
const int inStringLength = 12;
char inString[inStringLength];
int stringPos = 0;
int paramValues[3];


void setup() {
  Serial.begin(115200);
  strip.begin();

  // Update LED contents, to start they are all 'off'
  strip.show();
  
  for (int i=0; i < numLights; i++){
    //colorArray[i] = Wheel(random(55,85));
    colorArray[i] = Wheel(62+i);
  }
}

void loop() {
  if (Serial.available() > 0)
    handleSerial();
}


void handleSerial() {
  inByte = Serial.read();
  
  // save only ASCII numeric characters (ASCII 0 - 9) and commas:
  if (((inByte >= '0') && (inByte <= '9')) || inByte == ','){
    inString[stringPos] = inByte;
    //Serial.println(inByte);
    stringPos++;
  }
  
  // if you get an ASCII carriage return:
  if (inByte == '\n' || inByte == '|') {
    inString[stringPos] = 0;
    Serial.print("data: "); Serial.println(inString);
    int num = 0;
    int d = 0;
    for (int c = 0; c < inStringLength; c++) {
      if (inString[c] == ',' || inString[c] == 0){
        paramValues[d] = num;
        num = 0;
        d++;
      } else {
        if (num != 0){
          //skip until next comma or end
        } else {
          //turns out this assigns multi digit number until the next comma
          num = atoi(&inString[c]);
        }
      }
    } 
    
    // DO STUFF HERE
    hueArray[paramValues[0]] = paramValues[1];
    strip.setPixelColor(paramValues[0], Wheel(paramValues[1]));
    strip.show();  
    
    // clear the string with spaces
    for (int c = 0; c < inStringLength; c++) {
      inString[c] = ' ';
    } 
    // reset the string pointer:
    stringPos = 0;
  }
}

void fallColors()
{
  for (int i=0; i < numLights; i++) {
    colorArray[i] = colorArray[(i+1) % (numLights-1)];
  }
  
  for (int i=0; i < numLights; i++) {
    strip.setPixelColor(i, colorArray[i]);
    //strip.setPixelColor(i, Wheel(100));
    // 0 - green
    // 50 - yellow
    // 100 - pink
    // 150 - purple
  }  
}

void pulse()
{
  int i;
  int color = randomColor();
  int led = random(0,25);
  for (i=0; i < 255; i++){
    strip.setPixelColor(led, Color(i, 0, 0));
    strip.show();
    delay(10);
  } 
  for (i=255; i >= 0; i--){
    strip.setPixelColor(led, Color(i, 0, 0));
    strip.show();
    delay(10);
  } 
}

int randomColor()
{
  return Color(random(255), random(255), random(255));
}

void dart(uint32_t c, uint8_t wait) {
  int i;
  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
      strip.setPixelColor(i, 0);
  }
}

void dartBack(uint32_t c, uint8_t wait) {
  int i;
  for (i=strip.numPixels()-1; i >= 0; i--) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
      strip.setPixelColor(i, 0);
  }
}

void rainbow(uint8_t wait) {
  int i, j;
   
  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 255));
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(uint8_t wait) {
  int i, j;
  
  for (j=0; j < 256 * 5; j++) {     // 5 cycles of all 25 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 256 / strip.numPixels()) + j) % 256) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;
  
  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170; 
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
