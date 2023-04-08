/// Header
#include <Arduino.h>
#include "capstone_functions.h"
#define BUTTON_DEBOUNCE_TIME 20 

 //Function Defintions
void button_toggle(QwiicButton &button, bool &ledState){
   if (button.hasBeenClicked()) {
        ledState = !ledState; // Toggle the LED state
        if (ledState) {
            button.LEDon(); // Turn the LED on
        } else {
            button.LEDoff(); // Turn the LED off
        }
        button.clearEventBits(); // Clear button click event
    }
}

void mute(AudioMixer4 &mixer, int channel_num){
    mixer.gain(channel_num, 0);
}

void unmute(AudioMixer4 &mixer, int channel_num){
    mixer.gain(channel_num, 1);
}

void linear_fader(Adafruit_seesaw &seesaw, AudioAmplifier &amp, int analog_in){
    uint16_t analog_read = seesaw.analogRead(analog_in);
    float fade = analog_read;
    float scalar = (fade/1023)*3.16;
    amp.gain(scalar);
}
 void mutecontrol(AudioMixer4 &mixer, int channel_num, bool &state){
  	if (state == true) {
     		mute(mixer, channel_num);
  	}
  	else {
    		unmute(mixer, channel_num);
  	}
}

bool readEncoderButton(Adafruit_seesaw &seesaw, int SS_SWITCH) {
    return !seesaw.digitalRead(SS_SWITCH);
}

bool debounceButton(bool buttonPressed, bool &LEDState, uint32_t &lastDebounceTime, bool &lastButtonState) {
    bool stateChanged = false;
    if (buttonPressed != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > BUTTON_DEBOUNCE_TIME) {
        if (buttonPressed != LEDState) {
            LEDState = buttonPressed;
            stateChanged = true;
        }
    }

    lastButtonState = buttonPressed;
    return stateChanged;
}

void toggleNeoPixel(seesaw_NeoPixel &neopixel) {
    uint32_t currentColor = neopixel.getPixelColor(0);
    if (currentColor == neopixel.Color(0, 0, 0)) {
        neopixel.setPixelColor(0, neopixel.Color(255, 0, 0));
    } else {
        neopixel.setPixelColor(0, neopixel.Color(0, 0, 0));
    }
    neopixel.show();
}

void encoder_button(Adafruit_seesaw &seesaw, seesaw_NeoPixel &neopixel, int SS_SWITCH, int SS_NEOPIX, bool &LEDState, uint32_t &lastDebounceTime, bool &lastButtonState) {
    bool encoderButtonPressed = readEncoderButton(seesaw, SS_SWITCH);
    bool stateChanged = debounceButton(encoderButtonPressed, LEDState, lastDebounceTime, lastButtonState);

    if (stateChanged && LEDState) {
        toggleNeoPixel(neopixel);
    }
}

void readTouchInput(int &x, int &y, Adafruit_RA8875 tft) {
uint16_t w = static_cast<uint16_t>(x);
uint16_t e = static_cast<uint16_t>(y);
uint16_t* xp = &w;
uint16_t* yp = &e;

tft.touchRead(xp, yp);
x = static_cast<int>(*xp);
y = static_cast<int>(*yp);
}

void getMappedCoordinates(int &x, int &y, Adafruit_RA8875 tft) {
  x = map(x, TS_LEFT, TS_RT, 0, tft.width());
  y = map(y, TS_TOP, TS_BOT, 0, tft.height());
}

void printCoordinates(int x, int y) {
  Serial.print("X = ");
  Serial.print(x);
  Serial.print(", y = ");
  Serial.println(y);
}

bool isButtonPressed(int x, int y, int x1, int x2, int y1, int y2) {
  return (x >= x1 && x <= x2 && y >= y1 && y <= y2);
}

void handleMenuPress(bool& push, unsigned long& lastButtonPress, unsigned long currentMillis, int debounceTime, Adafruit_RA8875& tft) {
  if (currentMillis - lastButtonPress >= debounceTime) {
    bool previousPushState = push;
    push = !push;
    lastButtonPress = currentMillis;

    if (previousPushState != push) { // Only draw the screen if the button state has changed
      drawMenu(push, tft);
    }
  }
}

void drawMenu(bool push, Adafruit_RA8875 tft) {
  if (push) {
    printPEQ(tft);
  } else {
    printMenu(tft);
  }
}

void printPEQ(Adafruit_RA8875 tft){
  ///bool exitControl = 0;

  tft.fillScreen(RA8875_WHITE);
  
  ///LEFT BUTTONS (make just one rect for black back)
  tft.fillRect(0, 0, 120, 480, RA8875_BLACK); 
  tft.fillRect(4, 6, 110, 60, RA8875_GREEN);
  tft.fillRect(4, 74, 110, 60, RA8875_GREEN);
  tft.fillRect(4, 142, 110, 60, RA8875_GREEN);
  tft.fillRect(4, 210, 110, 60, RA8875_GREEN);
  tft.fillRect(4, 278, 110, 60, RA8875_GREEN);
  tft.fillRect(4, 346, 110, 60, RA8875_GREEN); 
  tft.fillRect(4, 414, 110, 60, RA8875_GREEN);

  ///Cuttoff, Bandwidth, Gain, On/OFF Boxes
  tft.fillRect(120, 0, 158, 280, RA8875_BLACK);
  tft.fillRect(122, 6, 150, 60, RA8875_CYAN);
  tft.fillRect(122, 74, 150, 60, RA8875_CYAN);
  tft.fillRect(122, 142, 150, 60, RA8875_CYAN); 
  tft.fillRect(122, 210, 150, 60, RA8875_GREEN);

  ///High-Pass Filter boxes
  tft.fillRect(638, 0, 162, 140, RA8875_BLACK);
  tft.fillRect(646, 6, 150, 60, RA8875_CYAN);
  tft.fillRect(646, 74, 150, 60, RA8875_CYAN);


  ///Bands Selector
  tft.fillRect(272, 270, 374, 214, RA8875_BLACK);
  tft.fillRect(280, 278, 86, 60, RA8875_GREEN);
  tft.fillRect(280, 346, 86, 60, RA8875_GREEN);
  tft.fillRect(280, 414, 86, 60, RA8875_GREEN);
  tft.fillRect(374, 278, 170, 60, RA8875_CYAN);
  tft.fillRect(374, 346, 81, 60, RA8875_GREEN);
  tft.fillRect(463, 346, 81, 60, RA8875_GREEN);
  tft.fillRect(374, 414, 170, 60, RA8875_CYAN);
  tft.fillRect(552, 278, 86, 60, RA8875_GREEN);
  tft.fillRect(552, 346, 86, 60, RA8875_GREEN);
  tft.fillRect(552, 414, 86, 60, RA8875_GREEN);




  //Print text in boxes
  tft.textMode();
  tft.textColor(RA8875_BLACK, RA8875_GREEN);    
  tft.textEnlarge(2);

  tft.textSetCursor(22, 12);
  tft.textWrite("IND");

  tft.textSetCursor(10, 80);
  tft.textWrite("IN 1");

  tft.textSetCursor(10, 148);
  tft.textWrite("IN 2");

  tft.textSetCursor(10, 216);
  tft.textWrite("IN 3");
  // On/off
  tft.textSetCursor(10, 216);
  tft.textWrite("IN 3");

  tft.textSetCursor(10, 284);
  tft.textWrite("IN 4");

  tft.textSetCursor(22, 352);
  tft.textWrite("PRI");

  tft.textSetCursor(22, 420);
  tft.textWrite("AUX");

  ///Cut/Band/Gain text
  tft.textEnlarge(1);
  tft.textColor(RA8875_BLACK, RA8875_CYAN);
  tft.textSetCursor(128, 18);
  tft.textWrite("Cut: ");
  tft.textSetCursor(128, 86);
  tft.textWrite("Band: ");
  tft.textSetCursor(128, 154);
  tft.textWrite("Gain: ");

  ///High-Pass Filter text
  tft.textSetCursor(648, 18);
  tft.textWrite("High-Pass");
  tft.textSetCursor(668, 86);
  tft.textWrite("Cut: ");

  ///Put back into graphics mode
  tft.graphicsMode();
};

void printMenu(Adafruit_RA8875 tft){
  
  tft.fillScreen(RA8875_WHITE);

  //Sets up blackspace
  tft.fillRect(0, 0, 120, 480, RA8875_BLACK);
  tft.fillRect(0, 0, 800, 74, RA8875_BLACK);
  tft.fillRect(120, 74, 74, 270, RA8875_BLACK);

  //Pre amps
  tft.fillRect(120, 6, 68, 60, RA8875_CYAN);
  tft.fillRect(120, 74, 68, 60, RA8875_CYAN);
  tft.fillRect(120, 142, 68, 60, RA8875_CYAN);
  tft.fillRect(120, 210, 68, 60, RA8875_CYAN);
  tft.fillRect(120, 278, 68, 60, RA8875_CYAN);   ///6 pixels of black space
 

  // PEQ and Channels
  tft.fillRect(4, 6, 110, 60, RA8875_CYAN);
  tft.fillRect(4, 74, 110, 60, RA8875_GREEN);
  tft.fillRect(4, 142, 110, 60, RA8875_GREEN);
  tft.fillRect(4, 210, 110, 60, RA8875_GREEN);
  tft.fillRect(4, 278, 110, 60, RA8875_GREEN);
  tft.fillRect(4, 346, 110, 60, RA8875_GREEN); 
  tft.fillRect(4, 414, 110, 60, RA8875_GREEN);
//Levels
  tft.fillRect(194, 6, 55, 60, RA8875_CYAN);
  tft.fillRect(255, 6, 55, 60, RA8875_CYAN);
  tft.fillRect(316, 6, 55, 60, RA8875_CYAN);
  tft.fillRect(377, 6, 55, 60, RA8875_CYAN);
  tft.fillRect(438, 6, 55, 60, RA8875_CYAN);
  tft.fillRect(499, 6, 55, 60, RA8875_CYAN);
  tft.fillRect(560, 6, 55, 60, RA8875_CYAN);
  tft.fillRect(621, 6, 55, 60, RA8875_CYAN);
  tft.fillRect(682, 6, 55, 60, RA8875_CYAN);
  tft.fillRect(743, 6, 55, 60, RA8875_CYAN);
  tft.textMode();
  //Level Text
  tft.textColor(RA8875_BLACK, RA8875_CYAN);    
  tft.textEnlarge(1);
  tft.textSetCursor(205, 16);
  tft.textWrite("P1");
  tft.textSetCursor(266, 16);
  tft.textWrite("P2");
  tft.textSetCursor(327, 16);
  tft.textWrite("P3");
  tft.textSetCursor(388, 16);
  tft.textWrite("P4");
  tft.textSetCursor(449, 16);
  tft.textWrite("A1");
  tft.textSetCursor(510, 16);
  tft.textWrite("A2");
  tft.textSetCursor(571, 16);
  tft.textWrite("A3");
  tft.textSetCursor(632, 16);
  tft.textWrite("A4");
  tft.textSetCursor(693, 16);
  tft.textWrite("MP");
  tft.textSetCursor(754, 16);
  tft.textWrite("MA");
  //Pre-Amp text
  tft.textSetCursor(130, 16); //offset 18
  tft.textWrite("P-A");
  tft.textSetCursor(137, 86); //offset 18
  tft.textWrite("63");
  tft.textSetCursor(137, 154); //offset 18
  tft.textWrite("63");
  tft.textSetCursor(137, 222); //offset 18
  tft.textWrite("63");
  tft.textSetCursor(137, 290); //offset 18
  tft.textWrite("63");
  //Channel and PEQ text
  tft.textColor(RA8875_BLACK, RA8875_CYAN);     
  tft.textEnlarge(2);
  tft.textSetCursor(22, 12);
  tft.textWrite("ASP");
  tft.textColor(RA8875_BLACK, RA8875_GREEN); 
  tft.textSetCursor(10, 80);
  tft.textWrite("IN 1");
  tft.textSetCursor(10, 148);
  tft.textWrite("IN 2");
  tft.textSetCursor(10, 216);
  tft.textWrite("IN 3");
  tft.textSetCursor(10, 284);
  tft.textWrite("IN 4");
  tft.textSetCursor(22, 352);
  tft.textWrite("PRI");
  tft.textSetCursor(22, 420);
  tft.textWrite("AUX");
  tft.graphicsMode();

  
/*
  //Prints center text box
  tft.fillRect(315, 0, 170, 55, RA8875_BLACK);
  tft.fillRect(320, 0, 160, 50, RA8875_WHITE);

  //Prints right text box
  tft.fillRect(535, 0, 170, 55, RA8875_BLACK);
  tft.fillRect(540, 0, 160, 50, RA8875_WHITE);
  
  //Sets up the text portion of the print
  tft.textMode();
  tft.textColor(RA8875_BLACK, RA8875_WHITE);    
  tft.textEnlarge(1);

  //Print text in left box
  tft.textSetCursor(155, 10);
  tft.textWrite("PEQ");

  //Print text in center box
  tft.textSetCursor(360, 10);
  tft.textWrite("SUB 2");

  //Print text in right box
  tft.textSetCursor(580, 10);
  tft.textWrite("SUB 3");
*/
  //Puts back into graphics mode (Not sure if this is needed yet but text didn't work without textMode)
  tft.graphicsMode();
}
