/* Code for FastLED video tutorial
 *  by Nils Gregersen 2017
 *  youtube: hamburgtech
 *  email: info@hamburgtech.de
 */

#include <FastLED.h>

#define LED_PIN     7
//#define CLOCK_PIN 4 
#define NUM_LEDS    144
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS  64

CRGB leds[NUM_LEDS];

void setup() {
    delay(1000);
    LEDS.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
//  LEDS.addLeds<LED_TYPE, LED_PIN, CLOCK_PIN COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
}

void loop()
{
  FastLED.clear();
  fill_solid(leds, NUM_LEDS, CRGB(145,44,20));
  FastLED.show();
  delay(1000);
  FastLED.clear();

  leds[31] = CRGB::HotPink;
  leds[32] = CRGB::BlueViolet;
  leds[33] = CRGB::HotPink;
  leds[34] = CRGB::YellowGreen;
  leds[35] = CRGB::HotPink;
  leds[36] = CRGB::BlueViolet;
  leds[37] = CRGB::HotPink;
  leds[38] = CRGB::YellowGreen;
  leds[39] = CRGB::HotPink;
  leds[40] = CRGB::YellowGreen;
  leds[41] = CRGB::BlueViolet;
  leds[42] = CRGB::HotPink;
  leds[43] = CRGB::HotPink;
  leds[44] = CRGB::YellowGreen;
  leds[45] = CRGB::HotPink;
    
  FastLED.show();
  delay(1000);
  FastLED.clear();

  fill_solid(leds, NUM_LEDS, CRGB(145,44,238));
  FastLED.show();
  delay(2000);
  FastLED.clear();

  for(int i=0;i<NUM_LEDS;i++){
    leds[i].setRGB(255,0,0);
    FastLED.show();
    delay(20);
 }
  FastLED.clear();

    for(int i=0;i<NUM_LEDS;i++){
    leds[i].setRGB(0,255,0);
    FastLED.show();
    delay(20);
 }
  FastLED.clear();

    for(int i=0;i<NUM_LEDS;i++){
    leds[i].setRGB(0,0,255);
    FastLED.show();
    delay(20);
 }
  FastLED.clear();

 for(int i=0;i<NUM_LEDS;i++){
   fill_rainbow(leds,i, 0, 5);
   FastLED.show();
   delay(20);
 }
  FastLED.clear();

 for(int i=0;i<NUM_LEDS;i++){  
   leds[i-2].setRGB(0,0,0);
   leds[i].setRGB(255,255,0);
   FastLED.show();
   delay(20);
 }
  FastLED.clear();    
}





