#ifndef capstone_functions
#define capstone_functions


// Libraries and Headers
#include "SparkFun_Qwiic_Button.cpp"
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "Adafruit_seesaw.h"

//Function prototypes

void button_toggle(QwiicButton &button, bool &ledState); // Qwiic Button On/OFF toggle
void linear_fader(Adafruit_seesaw &seesaw, AudioAmplifier &amp, uint16_t analog_read);
void mute(AudioMixer4 &mixer, int channel_num);
void unmute(AudioMixer4 &mixer, int channel_num);





#endif