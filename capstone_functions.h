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
#include <seesaw_neopixel.h>

//Function prototypes

void button_toggle(QwiicButton &button, bool &ledState); // Qwiic Button On/OFF toggle
void linear_fader(Adafruit_seesaw &seesaw, AudioAmplifier &amp, int analog_read);
void mute(AudioMixer4 &mixer, int channel_num);
void unmute(AudioMixer4 &mixer, int channel_num);
void mutecontrol(AudioMixer4 &mixer, int channel_num, bool &Sstate);
void encoder_button(Adafruit_seesaw &seesaw, seesaw_NeoPixel &neopixel, int SS_SWITCH, int SS_NEOPIX, bool &LEDState, uint32_t &lastDebounceTime, bool &lastButtonState) ;
void toggleNeoPixel(seesaw_NeoPixel &neopixel);
bool debounceButton(bool buttonPressed, bool &LEDState, uint32_t &lastDebounceTime, bool &lastButtonState);
bool readEncoderButton(Adafruit_seesaw &seesaw, int SS_SWITCH);



#endif