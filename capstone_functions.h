#ifndef capstone_functions_h
#define capstone_functions_h


// Libraries and Headers
#include <Arduino.h>
#include <SparkFun_Qwiic_Button.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <math.h>
#include "Adafruit_seesaw.h"
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"
#include <seesaw_neopixel.h>

//Constants

//Touchscreen consts
const int TS_LEFT = 40;
const int TS_RT = 980;
const int TS_TOP = 120;
const int TS_BOT = 920;
const unsigned long debounceTime = 1000;


//Function prototypes
// Qwiic Button
void button_toggle(QwiicButton &button, bool &ledState); // Qwiic Button On/OFF toggle
//Linear Fader Control
void linear_fader(Adafruit_seesaw &seesaw, AudioAmplifier &amp, int analog_read);
//Muting Stuff
void mute(AudioMixer4 &mixer, int channel_num);
void unmute(AudioMixer4 &mixer, int channel_num);
void mutecontrol(AudioMixer4 &mixer, int channel_num, bool &Sstate);
//Enocoder Functions
void encoder_button(Adafruit_seesaw &seesaw, seesaw_NeoPixel &neopixel, int SS_SWITCH, int SS_NEOPIX, bool &LEDState, uint32_t &lastDebounceTime, bool &lastButtonState);
void toggleNeoPixel(seesaw_NeoPixel &neopixel);
bool debounceButton(bool buttonPressed, bool &LEDState, uint32_t &lastDebounceTime, bool &lastButtonState);
bool readEncoderButton(Adafruit_seesaw &seesaw, int SS_SWITCH);
//Touchscreen
void handleButtonPress(bool &push, unsigned long &lastButtonPress, unsigned long &currentMillis, unsigned long debounceTime);
void drawMenu(bool push, Adafruit_RA8875 tft);
bool isButtonPressed(int x, int y, int x1, int x2, int y1, int y2);
void readTouchInput(int &x, int &y, Adafruit_RA8875 tft) ;
void getMappedCoordinates( int &x, int &y, Adafruit_RA8875 tft);
void handleMenuPress(bool& push, unsigned long& lastButtonPress, unsigned long currentMillis, int debounceTime, Adafruit_RA8875& tft);
void printCoordinates(int x, int y) ;
void printMenu(Adafruit_RA8875 tft);
void printPEQ(Adafruit_RA8875 tft);



#endif