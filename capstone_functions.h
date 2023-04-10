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


struct peq_buttons{
    int bs1 = 2;
    int bs2 = 2;
    bool ls = false;
    bool hs = false;
    bool lp = false;
    bool hp = false;
    bool b1 = false;
    bool b2 = false;
    bool b3 = false;
    bool b4 = false;
    bool status = false;
};

struct channel{
    bool input1 = 0;
    bool input2 = 0;
    bool input3 = 0;
    bool input4 = 0;
    bool pri = 0;
    bool aux = 0;

};

struct tsbutton{
    int x_begin = 0;
    int x_end = 0;
    int y_begin = 0;
    int y_end = 0;
    bool state = false;
    int num = 0 ;
};


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
void drawMenu(bool push, Adafruit_RA8875 tft, peq_buttons &peq, channel& ch);
bool isButtonPressed(int x, int y, tsbutton &button);
void readTouchInput(int &x, int &y, Adafruit_RA8875 tft);
void getMappedCoordinates( int &x, int &y, Adafruit_RA8875 tft);
void handleMenuPress(channel& ch, peq_buttons &peq, bool& push, unsigned long& lastButtonPress, unsigned long currentMillis, int debounceTime, Adafruit_RA8875& tft);
void handlePeqButtonPress(bool &button, peq_buttons &peq, int buttonIndex, unsigned long &lastButtonPress, unsigned long currentMillis, int debounceTime, Adafruit_RA8875 &tft);
void checkPEQButtons(peq_buttons& peq, int currentButtonIndex, Adafruit_RA8875 &tft);
void togglePEQButton(bool &button, int buttonIndex, int currentButtonIndex, Adafruit_RA8875 &tft, int x, int y, const String &label);
void handleChannelPress(bool& button, channel& ch, int buttonIndex, unsigned long& lastButtonPress, unsigned long currentMillis, int debounceTime, Adafruit_RA8875 &tft) ;
void resetOtherChannels(channel& ch, int currentButtonIndex, Adafruit_RA8875 &tft);
void drawButtonText(Adafruit_RA8875 &tft, int x, int y, const String &text, uint16_t textColor, uint16_t bgColor, int size);
void printCoordinates(int x, int y) ;
void printMenu(Adafruit_RA8875 tft);
void printPEQ(Adafruit_RA8875 &tft, peq_buttons &peq, channel& ch);
void drawAllBoxes(Adafruit_RA8875 &tft, peq_buttons &peq,channel& ch);
void drawLeftButtons(Adafruit_RA8875 &tft, channel &ch);
void drawCutoffBandwidthGainBoxes(Adafruit_RA8875 &tft, peq_buttons &peq);
void drawHighPassFilterBoxes(Adafruit_RA8875 &tft);
void drawBandSelectors(Adafruit_RA8875 &tft, peq_buttons &peq);
void printAllText(Adafruit_RA8875 &tft, peq_buttons &peq, channel& ch);
void printLeftButtonsText(Adafruit_RA8875 &tft, channel& ch);
void printCutoffBandwidthGainText(Adafruit_RA8875 &tft, peq_buttons &peq);
void printHighPassFilterText(Adafruit_RA8875 &tft);
void printBandSelectorText(Adafruit_RA8875 &tft, peq_buttons &peq);
void printInputSelectionText(Adafruit_RA8875 &tft);


#endif