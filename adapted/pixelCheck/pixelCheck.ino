#include <FastLED.h>

#define LED_PIN     7
#define NUM_LEDS    50
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
#define BRIGHTNESS  64

// Define the array of leds
CRGB leds[NUM_LEDS];

void setup() { 
    delay(1000);
    LEDS.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
}

void loop() { 
  // Turn the LED on, then pause
  FastLED.clear();

  for(int i=0; i<NUM_LEDS; i++){
    leds[i] = CRGB::BlueViolet;
    FastLED.show();
    delay(200);
    // Now turn the LED off, then pause
    leds[i] = CRGB::Black;
    FastLED.show();
    delay(50);
  }

  FastLED.clear();
}
