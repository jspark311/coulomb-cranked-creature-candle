/*]]]]]]]]]]][[[[[[[`````////bbbbbbbbbbbbbbb
 WiFi Access Point Web Server NeoPixel 8x8 grid
 A simple web server that lets you to turn on and off NeoPixel array via the web.
 You can open address 10.10.10.1 in a web browser
 to turn on and off NeoPixel connected on pin 5.
 http://10.10.10.1/ON turns the LED on
 http://10.10.10.1/OFF turns it off
 This example is written for a network using WPA encryption.

 */

#include "Adafruit_NeoPixel.h"
#include <math.h>
  extern "C" {
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
  }

// Which pin on the ESP32 is connected to the NeoPixels?
#define PIN          14

// How many NeoPixels LEDs are attached to the ESP32?
#define NUMPIXELS   256

// We define birghtness of NeoPixel LEDs
#define BRIGHTNESS  30

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel matrix = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);





/*****************************************************************************************
* Coherent noise
******************************************************************************************
* This was adapted from AdaFruit's GFX library demos.
*
*  // plasma demo for Adafruit RGBmatrixPanel library.
*  // Demonstrates unbuffered animation on our 32x32 RGB LED matrix:
*  // http://www.adafruit.com/products/607
*
*  // Written by Limor Fried/Ladyada & Phil Burgess/PaintYourDragon
*  // for Adafruit Industries.
*  // BSD license, all text above must be included in any redistribution.
*****************************************************************************************/

static const int8_t sinetab[256] = {
     0,   2,   5,   8,  11,  15,  18,  21,
    24,  27,  30,  33,  36,  39,  42,  45,
    48,  51,  54,  56,  59,  62,  65,  67,
    70,  72,  75,  77,  80,  82,  85,  87,
    89,  91,  93,  96,  98, 100, 101, 103,
   105, 107, 108, 110, 111, 113, 114, 116,
   117, 118, 119, 120, 121, 122, 123, 123,
   124, 125, 125, 126, 126, 126, 126, 126,
   127, 126, 126, 126, 126, 126, 125, 125,
   124, 123, 123, 122, 121, 120, 119, 118,
   117, 116, 114, 113, 111, 110, 108, 107,
   105, 103, 101, 100,  98,  96,  93,  91,
    89,  87,  85,  82,  80,  77,  75,  72,
    70,  67,  65,  62,  59,  56,  54,  51,
    48,  45,  42,  39,  36,  33,  30,  27,
    24,  21,  18,  15,  11,   8,   5,   2,
     0,  -3,  -6,  -9, -12, -16, -19, -22,
   -25, -28, -31, -34, -37, -40, -43, -46,
   -49, -52, -55, -57, -60, -63, -66, -68,
   -71, -73, -76, -78, -81, -83, -86, -88,
   -90, -92, -94, -97, -99,-101,-102,-104,
  -106,-108,-109,-111,-112,-114,-115,-117,
  -118,-119,-120,-121,-122,-123,-124,-124,
  -125,-126,-126,-127,-127,-127,-127,-127,
  -128,-127,-127,-127,-127,-127,-126,-126,
  -125,-124,-124,-123,-122,-121,-120,-119,
  -118,-117,-115,-114,-112,-111,-109,-108,
  -106,-104,-102,-101, -99, -97, -94, -92,
   -90, -88, -86, -83, -81, -78, -76, -73,
   -71, -68, -66, -63, -60, -57, -55, -52,
   -49, -46, -43, -40, -37, -34, -31, -28,
   -25, -22, -19, -16, -12,  -9,  -6,  -3
};


float radius1  = 16.3, radius2  = 23.0, radius3  = 40.8, radius4  = 44.2,
            centerx1 = 16.1, centerx2 = 11.6, centerx3 = 23.4, centerx4 =  4.1,
            centery1 =  8.7, centery2 =  6.5, centery3 = 14.0, centery4 = -2.9;
float       angle1   =  0.0, angle2   =  0.0, angle3   =  0.0, angle4   =  0.0;
float       angle1_s =  0.0, angle2_s =  0.0, angle3_s =  0.0, angle4_s =  0.0;
long        hueShift =  0;
int         hue_shift_s = 2;
int         x1, x2, x3, x4, _y1, _y2, _y3, _y4, sx1, sx2, sx3, sx4;

void advance_plasma() {
  unsigned char x, y;
  long          value;
  sx1 = (int)(cos(angle1) * radius1 + centerx1);
  sx2 = (int)(cos(angle2) * radius2 + centerx2);
  sx3 = (int)(cos(angle3) * radius3 + centerx3);
  sx4 = (int)(cos(angle4) * radius4 + centerx4);
  _y1 = (int)(sin(angle1) * radius1 + centery1);
  _y2 = (int)(sin(angle2) * radius2 + centery2);
  _y3 = (int)(sin(angle3) * radius3 + centery3);
  _y4 = (int)(sin(angle4) * radius4 + centery4);

  for(y=0; y<16; y++) {
    x1 = sx1; x2 = sx2; x3 = sx3; x4 = sx4;
    for(x=0; x<16; x++) {
      value = hueShift
        + (int8_t)(sinetab[ (uint8_t)((x1 * x1 + _y1 * _y1) >> 2)])
        + (int8_t)(sinetab[ (uint8_t)((x2 * x2 + _y2 * _y2) >> 2)])
        + (int8_t)(sinetab[ (uint8_t)((x3 * x3 + _y3 * _y3) >> 3)])
        + (int8_t)(sinetab[ (uint8_t)((x4 * x4 + _y4 * _y4) >> 3)]);
      matrix.setPixelColor(x*y, value * 3);
      x1--; x2--; x3--; x4--;
    }
    _y1--; _y2--; _y3--; _y4--;
  }

    angle1 += angle1_s;
    angle2 -= angle2_s;
    angle3 += angle3_s;
    angle4 -= angle4_s;
    hueShift += hue_shift_s;
    matrix.show();
}






// Set color for LEDs in array with delay between setting each LED
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<matrix.numPixels(); i++) {
    matrix.setPixelColor(i, c);
    matrix.show();
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(uint8_t WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return matrix.Color(255 - WheelPos * 3, 0, WheelPos * 3,0);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return matrix.Color(0, WheelPos * 3, 255 - WheelPos * 3,0);
  }
  WheelPos -= 170;
  return matrix.Color(WheelPos * 3, 255 - WheelPos * 3, 0,0);
}

void rainbow(uint8_t offset) {
  for(uint16_t i = 0; i < matrix.numPixels(); i++) {
    matrix.setPixelColor(i, Wheel((i+offset) & 255));
  }
  matrix.show();
}


extern "C" {
void app_main() {
  uint8_t offset = 0;
  matrix.setBrightness(BRIGHTNESS);

  // Start NeoPixel library with all LEDs off
  matrix.begin();

  // Show settings of LEDs in NeoPixel array
  rainbow(offset++);
  while(1) {
      vTaskDelay(50 / portTICK_PERIOD_MS);
      //advance_plasma();
      rainbow(offset++);
  }
}
}
