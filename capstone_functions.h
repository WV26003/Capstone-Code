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

//Structs
struct Slider{
  int x_begin;
  int x_end;
  int y_begin;
  int y_end;
  int currentSliderPos;
  int lastSliderPos;
  float slider_value;
  
};

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
    bool hpf = false;
    bool status = false;
    Slider hpfSlider;
};

struct channel{
    int active = 0;
    bool input1 = 0;
    bool input2 = 0;
    bool input3 = 0;
    bool input4 = 0;
    bool pri = 0;
    bool aux = 0;
    peq_buttons peqs[6];
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
void mute_control(AudioMixer4 &mixer, int channel_num, bool &state);

void output_mute(AudioAmplifier &amp);
void output_unmute(AudioAmplifier &amp);
void output_mix_mute_control(AudioAmplifier &amp, bool &state);

void muting_status_array(seesaw_NeoPixel encoder_pixels[], bool muting_status[]);
void input_muting(AudioMixer4 &mixer1, AudioMixer4 &mixer2, seesaw_NeoPixel encoder_pixels[], bool muting_status[]);

//Monitoring Function
void monitoring(AudioMixer4 &mixer3, AudioMixer4 &mixer4, bool monitoring_status[]);

//Enocoder Functions
void encoder_button(Adafruit_seesaw &seesaw, seesaw_NeoPixel &neopixel, int SS_SWITCH, int SS_NEOPIX, bool &LEDState, uint32_t &lastDebounceTime, bool &lastButtonState);
void toggleNeoPixel(seesaw_NeoPixel &neopixel);
bool debounceButton(bool buttonPressed, bool &LEDState, uint32_t &lastDebounceTime, bool &lastButtonState);
bool readEncoderButton(Adafruit_seesaw &seesaw, int SS_SWITCH);
void encoder_preamp(Adafruit_seesaw &seesaw, AudioControlSGTL5000 sgtl5000, int32_t &encoder_position);
void encoder_fader(Adafruit_seesaw &seesaw, AudioAmplifier &amp, int32_t &encoder_position);

//Touchscreen
void handleButtonPress(bool &push, unsigned long &lastButtonPress, unsigned long &currentMillis, unsigned long debounceTime);
void drawMenu(bool push, Adafruit_RA8875 tft, peq_buttons &peq, channel& ch);
bool isButtonPressed(int x, int y, tsbutton &button);
void readTouchInput(int &x, int &y, Adafruit_RA8875 tft);
void getMappedCoordinates( int &x, int &y, Adafruit_RA8875 tft);
void handleMenuPress(channel& ch, peq_buttons &peq, bool& push, unsigned long& lastButtonPress, unsigned long currentMillis, int debounceTime, Adafruit_RA8875& tft);
void handlePeqButtonPress(channel &ch, int buttonIndex, unsigned long &lastButtonPress, unsigned long currentMillis, int debounceTime, Adafruit_RA8875 &tft);
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
void drawHighPassFilterBoxes(Adafruit_RA8875 &tft, peq_buttons &peq);
void drawBandSelectors(Adafruit_RA8875 &tft, peq_buttons &peq);
void printAllText(Adafruit_RA8875 &tft, peq_buttons &peq, channel& ch);
void printLeftButtonsText(Adafruit_RA8875 &tft, channel& ch);
void printCutoffBandwidthGainText(Adafruit_RA8875 &tft, peq_buttons &peq);
void printHighPassFilterText(Adafruit_RA8875 &tft, peq_buttons &peq);
void printBandSelectorText(Adafruit_RA8875 &tft, peq_buttons &peq);
void printInputSelectionText(Adafruit_RA8875 &tft);
//ts sliders
void drawHPFSlider(Adafruit_RA8875 &tft, int circleY , Slider &slide);
void handleHPFSlider(Adafruit_RA8875 &tft, int y, Slider &slide, peq_buttons &peq);
void updateSliders(Adafruit_RA8875 &tft, peq_buttons &peq);


void initializeChannel(channel &ch) ;

void level_indication(float level[], int scaled_start[], int scaled_extension[], Adafruit_RA8875 tft);


#endif