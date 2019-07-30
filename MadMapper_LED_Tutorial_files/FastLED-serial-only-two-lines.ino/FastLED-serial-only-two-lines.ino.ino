#include <SPI.h>
#include <FastLED.h>

#define MAD_LED_PACKET_HEADER 0xFF
#define MAD_LED_DATA 0xBE
#define MAD_LED_DETECTION 0xAE
#define MAD_LED_DETECTION_REPLY 0xEA
#define MAD_LED_PROTOCOL_VERSION 0x01

//#define DEBUG_MODE
// To test LEDs (FastLED) without the need of MadMapper or MadRouter sending data on serial port
//#define JUST_TEST_LEDS

#define NUM_LEDS_LINE0 100
#define DATA_PIN_LINE0 5
#define CLOCK_PIN_LINE0 6

#define NUM_LEDS_LINE1 100
#define DATA_PIN_LINE1 5
#define CLOCK_PIN_LINE1 6

#if NUM_LEDS_LINE0 > NUM_LEDS_LINE1
  #define NUM_LEDS_IN_LONGEST_LINE NUM_LEDS_LINE0
#else
  #define NUM_LEDS_IN_LONGEST_LINE NUM_LEDS_LINE1
#endif

// Fast LED Buffers
CRGB leds_line0[NUM_LEDS_LINE0];
CRGB leds_line1[NUM_LEDS_LINE1];

// MadLED protocol buffer
char dataFrame[NUM_LEDS_IN_LONGEST_LINE*3];
int readingFrameOnLine=-1;
bool gotNewDataFrame=false;

enum State {
  State_WaitingNextPacket,
  State_GotPacketHeader,
  State_WaitingLineNumber,
  State_WaitingChannelCountByte1,
  State_WaitingChannelCountByte2,
  State_ReadingDmxFrame
};

State inputState=State_WaitingNextPacket;
unsigned int channelsLeftToRead=0;
char* frameWritePtr=dataFrame;

void setup() { 
  Serial.begin(921600);
  
  for (unsigned int i=0; i<sizeof(dataFrame); i++) dataFrame[i]=0;

  // Here setup FastLED first LED line protocol
  // Uncomment/edit one of the following lines for your leds arrangement.
  // FastLED.addLeds<TM1803, DATA_PIN_LINE0, RGB>(leds_line0, NUM_LEDS_LINE0);
  // FastLED.addLeds<TM1804, DATA_PIN_LINE0, RGB>(leds_line0, NUM_LEDS_LINE0);
  // FastLED.addLeds<TM1809, DATA_PIN_LINE0, RGB>(leds_line0, NUM_LEDS_LINE0);
  // FastLED.addLeds<WS2811, DATA_PIN_LINE0, RGB>(leds_line0, NUM_LEDS_LINE0);
  // FastLED.addLeds<WS2812, DATA_PIN_LINE0, RGB>(leds_line0, NUM_LEDS_LINE0);
  // FastLED.addLeds<WS2812B, DATA_PIN_LINE0, RGB>(leds_line0, NUM_LEDS_LINE0);
  // FastLED.addLeds<NEOPIXEL, DATA_PIN_LINE0>(leds_line0, NUM_LEDS_LINE0);
  // FastLED.addLeds<APA104, DATA_PIN_LINE0, RGB>(leds_line0, NUM_LEDS_LINE0);
  // FastLED.addLeds<UCS1903, DATA_PIN_LINE0, RGB>(leds_line0, NUM_LEDS_LINE0);
  // FastLED.addLeds<UCS1903B, DATA_PIN_LINE0, RGB>(leds_line0, NUM_LEDS_LINE0);
  // FastLED.addLeds<GW6205, DATA_PIN_LINE0, RGB>(leds_line0, NUM_LEDS_LINE0);
  // FastLED.addLeds<GW6205_400, DATA_PIN_LINE0, RGB>(leds_line0, NUM_LEDS_LINE0);
  FastLED.addLeds<APA102, DATA_PIN_LINE0, CLOCK_PIN_LINE0, RGB, DATA_RATE_MHZ(2)>(leds_line0, NUM_LEDS_LINE0);
  // FastLED.addLeds<WS2801, DATA_PIN_LINE0, CLOCK_PIN_LINE0, RGB>(leds_line0, NUM_LEDS_LINE0);
  // FastLED.addLeds<SM16716, DATA_PIN_LINE0, CLOCK_PIN_LINE0, RGB>(leds_line0, NUM_LEDS_LINE0);
  // FastLED.addLeds<LPD8806, DATA_PIN_LINE0, CLOCK_PIN_LINE0, RGB>(leds_line0, NUM_LEDS_LINE0);
  // FastLED.addLeds<P9813, DATA_PIN_LINE0, CLOCK_PIN_LINE0, RGB>(leds_line0, NUM_LEDS_LINE0);
  // FastLED.addLeds<DOTSTAR, DATA_PIN_LINE0, CLOCK_PIN_LINE0, RGB>(leds_line0, NUM_LEDS_LINE0);

  // Here setup FastLED second LED line protocol
  // Uncomment/edit one of the following lines for your leds arrangement.
  // FastLED.addLeds<TM1803, DATA_PIN_LINE1, RGB>(leds_line1, NUM_LEDS_LINE1);
  // FastLED.addLeds<TM1804, DATA_PIN_LINE1, RGB>(leds_line1, NUM_LEDS_LINE1);
  // FastLED.addLeds<TM1809, DATA_PIN_LINE1, RGB>(leds_line1, NUM_LEDS_LINE1);
  // FastLED.addLeds<WS2811, DATA_PIN_LINE1, RGB>(leds_line1, NUM_LEDS_LINE1);
  // FastLED.addLeds<WS2812, DATA_PIN_LINE1, RGB>(leds_line1, NUM_LEDS_LINE1);
  // FastLED.addLeds<WS2812B, DATA_PIN_LINE1, RGB>(leds_line1, NUM_LEDS_LINE1);
  // FastLED.addLeds<NEOPIXEL, DATA_PIN_LINE1>(leds_line1, NUM_LEDS_LINE1);
  // FastLED.addLeds<APA104, DATA_PIN_LINE1, RGB>(leds_line1, NUM_LEDS_LINE1);
  // FastLED.addLeds<UCS1903, DATA_PIN_LINE1, RGB>(leds_line1, NUM_LEDS_LINE1);
  // FastLED.addLeds<UCS1903B, DATA_PIN_LINE1, RGB>(leds_line1, NUM_LEDS_LINE1);
  // FastLED.addLeds<GW6205, DATA_PIN_LINE1, RGB>(leds_line1, NUM_LEDS_LINE1);
  // FastLED.addLeds<GW6205_400, DATA_PIN_LINE1, RGB>(leds_line1, NUM_LEDS_LINE1);
  FastLED.addLeds<APA102, DATA_PIN_LINE1, CLOCK_PIN_LINE1, RGB, DATA_RATE_MHZ(2)>(leds_line1, NUM_LEDS_LINE1);
  // FastLED.addLeds<WS2801, DATA_PIN_LINE1, CLOCK_PIN_LINE1, RGB>(leds_line1, NUM_LEDS_LINE1);
  // FastLED.addLeds<SM16716, DATA_PIN_LINE1, CLOCK_PIN_LINE1, RGB>(leds_line1, NUM_LEDS_LINE1);
  // FastLED.addLeds<LPD8806, DATA_PIN_LINE1, CLOCK_PIN_LINE1, RGB>(leds_line1, NUM_LEDS_LINE1);
  // FastLED.addLeds<P9813, DATA_PIN_LINE1, CLOCK_PIN_LINE1, RGB>(leds_line1, NUM_LEDS_LINE1);
  // FastLED.addLeds<DOTSTAR, DATA_PIN_LINE1, CLOCK_PIN_LINE1, RGB>(leds_line1, NUM_LEDS_LINE1);

  Serial.print("Setup done");
}

void processByte(unsigned char currentByte) {
  #ifdef DEBUG_MODE
    Serial.print("GOT BYTE: "); Serial.print(currentByte,HEX);
  #endif
  if (currentByte==MAD_LED_PACKET_HEADER) {
    inputState=State_GotPacketHeader;
    #ifdef DEBUG_MODE
      Serial.print("GOT PH ");
    #endif
  } else
  if (inputState == State_WaitingNextPacket) {
    // Just ignore this byte, we're not processing a packet at the moment
    // Wait for next packet start (xFF)
  } else 
  if (inputState == State_GotPacketHeader) {
    if (currentByte==MAD_LED_DETECTION) {
      // Send back detection reply
      Serial.write(MAD_LED_DETECTION_REPLY);
      Serial.write(MAD_LED_PROTOCOL_VERSION);
      inputState=State_WaitingNextPacket;
    } else if (currentByte==MAD_LED_DATA) {
      inputState=State_WaitingLineNumber;
      #ifdef DEBUG_MODE
        Serial.print("GOT LD ");
      #endif
    } else {
      // Unknown packet start, reset
      inputState=State_WaitingNextPacket;
    }
  } else 
  if (inputState == State_WaitingLineNumber) {
    if (currentByte>0x7F) {
      // Error, reset
      inputState=State_WaitingNextPacket;
      #ifdef DEBUG_MODE
        Serial.print("ErrLineNum: "); Serial.print(currentByte);
      #endif
    } else {
      readingFrameOnLine=currentByte;
      inputState=State_WaitingChannelCountByte1;
      #ifdef DEBUG_MODE
        Serial.print("GOT LN ");
      #endif
    }
  } else
  if (inputState == State_WaitingChannelCountByte1) {
    if (currentByte>0x7F) {
      // Error, reset
      inputState=State_WaitingNextPacket;
      #ifdef DEBUG_MODE
        Serial.print("ErrChCNT1: "); Serial.print(currentByte);
      #endif
    } else {
      channelsLeftToRead=currentByte;
      inputState=State_WaitingChannelCountByte2;
      #ifdef DEBUG_MODE
        Serial.print("GOT CHC1 ");
      #endif
    }
  } else
  if (inputState == State_WaitingChannelCountByte2) {
    if (currentByte>0x7F) {
      // Error, reset
      inputState=State_WaitingNextPacket;
      #ifdef DEBUG_MODE
        Serial.print("ErrChCNT2: "); Serial.print(currentByte);
      #endif
    } else {
      channelsLeftToRead+=(int(currentByte)<<7);
      if (channelsLeftToRead==0) {
        // Error, reset
        inputState=State_WaitingNextPacket;
        #ifdef DEBUG_MODE
          Serial.print("ErrChCNT=0");
        #endif
      } else {
        frameWritePtr=dataFrame;
        inputState=State_ReadingDmxFrame;
        #ifdef DEBUG_MODE
          Serial.print("GOT CHC2 ");
        #endif
      }
    }
  } else 
  if (inputState==State_ReadingDmxFrame) {
    *frameWritePtr++ = currentByte;
    channelsLeftToRead--;
    if (channelsLeftToRead==0) {
      // Finished reading DMX Frame
      inputState=State_WaitingNextPacket;
      gotNewDataFrame=true;
      #ifdef DEBUG_MODE
        Serial.print("GOT DATA ");
      #endif
    }
  }
}

void loop() {
  // We read a maximum of 30000 bytes before we should call FastLED.show again
  // This is a good setting for the teensy, it depends on CPU speed, so it should be set lower on a slower CPU (ie arduino)
  // This limit (bytesRead<30000) is useless for protocols with a clock
  // But necessary when controlling more than 600 hundred RGB leds with WS2811 / WS2812
  int bytesRead=0;
  while (Serial.available() > 0 && bytesRead<30000) {
    processByte(Serial.read()); bytesRead++;
    if (gotNewDataFrame) break;
  }

  #ifdef JUST_TEST_LEDS
    static int value=0;
    value = (value + 1) % 255;
    for (int i=0; i<NUM_LEDS_LINE0; i++) leds_line0[i]=CRGB(value,value,value);
  #else
    if (gotNewDataFrame) {
      gotNewDataFrame=false;
      char* dataPtr=dataFrame;
      // Copy the data frame we received in the correct FastLED buffer
      if (readingFrameOnLine==0) {
        for (int i=0; i<NUM_LEDS_LINE0; i++) {leds_line0[i]=CRGB(dataPtr[0],dataPtr[1],dataPtr[2]); dataPtr+=3;}
      } else if (readingFrameOnLine==1) {
        for (int i=0; i<NUM_LEDS_LINE1; i++) {leds_line1[i]=CRGB(dataPtr[0],dataPtr[1],dataPtr[2]); dataPtr+=3;}
      }
    }
  #endif
  
  FastLED.show();
}
