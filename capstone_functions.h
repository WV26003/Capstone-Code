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
const unsigned long debounceTime = 200;



//Function prototypes
// Qwiic Button
void button_toggle(QwiicButton &button, bool &ledState); // Qwiic Button On/OFF toggle
//Linear Fader Control
void linear_fader(Adafruit_seesaw &seesaw, AudioAmplifier &amp, int analog_read);
//Muting Stuff
void mute(AudioMixer4 &mixer, int channel_num);
void unmute(AudioMixer4 &mixer, int channel_num);
void output_mix_mute_control(AudioMixer4 &mixer, int channel_num, bool &Sstate);
//Enocoder Functions
void encoder_button(Adafruit_seesaw &seesaw, seesaw_NeoPixel &neopixel, int SS_SWITCH, int SS_NEOPIX, bool &LEDState, uint32_t &lastDebounceTime, bool &lastButtonState);
void toggleNeoPixel(seesaw_NeoPixel &neopixel);
bool debounceButton(bool buttonPressed, bool &LEDState, uint32_t &lastDebounceTime, bool &lastButtonState);
bool readEncoderButton(Adafruit_seesaw &seesaw, int SS_SWITCH);
void encoder_preamp(Adafruit_seesaw &seesaw, AudioControlSGTL5000 sgtl5000, int32_t &encoder_position);
void encoder_fader(Adafruit_seesaw &seesaw, AudioAmplifier &amp, int32_t &encoder_position);
//Touchscreen





#endif