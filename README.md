# BLE-Midi-LEDs
Sunday afternoon project - Simple Bluetooth-MIDI controlled LED strip.

Created with
- FastLED 3.3.3  - https://github.com/FastLED/FastLED 
- ESP32 BLE MIDI 0.2.2 https://github.com/max22-/ESP32-BLE-MIDI  
- Arduino 1.8.19
  
WS2812 based LED strip attached to a ESP32 running a BLE MIDI client. Currently, 12 LEDs are grouped by 3 and just showing up with some standard colours.
Refer to the Sketch for more details

The ESP32 is responding to the following MIDI CCs

| CC | Function                       | value |
|----|--------------------------------|-------|
| 40 | Brightness                     |       |
| 50 | Blackout                       | >64   |
| 50 | Segement 1                     |       |
| .. |                                |       |
| 54 | Segement 4                     |       |

If there's no MIDI Connections the LEDs show up with the FastLEDs rainbiw pattern.

Alexander, June 2022



