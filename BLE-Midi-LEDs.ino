/* ------------------------------------------------------------------------------------------
 * BLEMIDILeds 
 *  
 * AP, 18-JUN-2022 
 * 
 * Based on FastLED 3.3.3  - https://github.com/FastLED/FastLED 
 * and 
 * ESP32 BLE-MIDI 0.2.2    - https://github.com/max22-/ESP32-BLE-MIDI     
 * 
 * for a ESP 32 WROOM32 Devkit Module with WS2812 strips attached to PIN13    
 * ------------------------------------------------------------------------------------------ */


#define BLE_DEVNAME   "BLE MIDI Leds"
#define BLE_NUMLEDS     12
#define BLE_LEDSPERSTEP 3
#define BLE_SEGMENTS     (BLE_NUMLEDS/BLE_LEDSPERSTEP)
 
#include <Arduino.h>
#include <BLEMidi.h>

#include <FastLED.h>

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    13           // WS2812 Data Pin
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    BLE_NUMLEDS
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

bool BLEIsConnected = false;

uint8_t brightness = BRIGHTNESS;

// -----------------------------------------------------------------------------
// Color management

void SetCRGBColor(CRGB col) {
  for( int i = 0; i < NUM_LEDS; i++) 
     leds[i] = col;      
}

// -----------------------------------------------------------------------------
// Callback functions for BLE connection management and MIDI Rx
void BLE_connected()
{
   Serial.println("Connected");
   SetCRGBColor(CRGB::DarkGreen);
   BLEIsConnected = true;
}

void BLE_disconnected()
{
   Serial.println("Disconnected");
   SetCRGBColor(CRGB::DarkBlue);
   BLEIsConnected = false;
}

void onControlChange(uint8_t channel, uint8_t controller, uint8_t value, uint16_t timestamp)
{
  
    int offset = 0; 
    int i,j = 0;
    int bank = 0;
    
    CRGB IndColour = CRGB::Black;
    
    
    Serial.printf("Received control change : channel %d, controller %d, value %d (timestamp %dms)\n", channel, controller, value, timestamp);

    switch(controller) {
      case 50:  offset = -1;                                                // switch off all LEDs i
                if(value >=64)
                    SetCRGBColor(CRGB::Black);              
                break;
                
      case 51:  offset = 0;                                                 // change dedicated LED part 1
                break;
      case 52:  offset = BLE_LEDSPERSTEP;
                break;
      case 53:  offset = 2*BLE_LEDSPERSTEP;
                break;
      case 54:  offset = 3*BLE_LEDSPERSTEP;
                break;
                
      case 40:  offset = -1;                                                // change brightness of all LEDs
                brightness = value;
                if(brightness > BRIGHTNESS) brightness = BRIGHTNESS;
                FastLED.setBrightness(brightness);
                break;
                
      case 41:                                                              // change 2 LEDs at once
      case 42:  offset = -1;
                bank = (controller==42)?2*BLE_LEDSPERSTEP:0;
                for(i=0; i<4; i+=2)
                   {
                       switch((value>>i) & 0x03)
                         {
                          case 3: IndColour = CRGB::Red;
                                  break;
                          case 2: IndColour = CRGB::Green;
                                  break;
                          case 1: IndColour = CRGB::Blue;
                                  break;
                          default: IndColour = CRGB::Black;
                                  break;
                         }
                        for(j=0; j<BLE_LEDSPERSTEP; j++) leds[bank+i*BLE_LEDSPERSTEP/2+j] = IndColour;
                        
                   }
                break;
                
      default:  offset = -1;                                                // ignore
                break;
     }

    if(offset >= 0)                                                         // change dedicated LED part 2
    {
        if(value > 100)         IndColour = CRGB::Red;
        else if(value > 75)     IndColour = CRGB::Yellow;
        else if(value > 50)     IndColour = CRGB::Green;
        else if(value > 25)     IndColour = CRGB::Blue;
        else                    IndColour = CRGB::Black;

          for( int i = 0; i < BLE_NUMLEDS/BLE_SEGMENTS; i++) 
              leds[i+offset] = IndColour;     
           
    }
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing LEDs");

  // Setup LEDs
  delay(2000);
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);  
  SetCRGBColor(CRGB::DarkOrange);

  BLEIsConnected = false;
  Serial.println("Initializing bluetooth");
  BLEMidiServer.begin(BLE_DEVNAME);
  BLEMidiServer.setOnConnectCallback(BLE_connected);
  BLEMidiServer.setOnDisconnectCallback(BLE_disconnected);
  BLEMidiServer.setControlChangeCallback(onControlChange);
  
  Serial.println("Waiting for connections...");
  //BLEMidiServer.enableDebugging();  // Uncomment if you want to see some debugging output from the library

}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void loop() {
   
  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow

  if(!BLEIsConnected)
  {
     // FastLED's built-in rainbow generator
     fill_rainbow( leds, NUM_LEDS, gHue, 7);
     // send the 'leds' array out to the actual LED strip
     FastLED.show();
     FastLED.delay(1000/FRAMES_PER_SECOND);
  }
  else  
  {   
    // send the 'leds' array out to the actual LED strip
    FastLED.show();  
    FastLED.delay(1000/FRAMES_PER_SECOND);

  }
}

// -----------------------------------------------------------------------------
