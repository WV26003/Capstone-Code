/// Header
#include <Arduino.h>
#include "capstone_functions.h"
#define BUTTON_DEBOUNCE_TIME 5
#define DARK_GREEN 0x0640
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
        neopixel.setPixelColor(0, neopixel.Color(0, 255, 0));
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

bool isButtonPressed(int x, int y, tsbutton &button) {
  return (x >= button.x_begin && x <= button.x_end && y >= button.y_begin && y <= button.y_end);
}

void handleMenuPress(channel& ch, peq_buttons &peq, bool& push, unsigned long& lastButtonPress, unsigned long currentMillis, int debounceTime, Adafruit_RA8875& tft){
  if (currentMillis - lastButtonPress >= debounceTime) {
    bool previousPushState = push;
    push = !push;
    lastButtonPress = currentMillis;
    Serial.println("Button Toggle");

    if (previousPushState != push) { // Only draw the screen if the button state has changed
      drawMenu(push, tft, peq, ch);
    }
  }
}

void handlePeqButtonPress(channel &ch, int buttonIndex, unsigned long &lastButtonPress, unsigned long currentMillis, int debounceTime, Adafruit_RA8875 &tft) {
 
  if (currentMillis - lastButtonPress >= debounceTime) {
    lastButtonPress = currentMillis;
    Serial.print("Button ");
    Serial.print(buttonIndex);
    Serial.println(" Toggle");
    
    if(ch.active != 0 )checkPEQButtons(ch.peqs[ch.active - 1], buttonIndex, tft);
    updateSliders(tft, ch.peqs[ch.active - 1]);
  }
}
void checkPEQButtons(peq_buttons &peq, int currentButtonIndex, Adafruit_RA8875 &tft) {
  if (currentButtonIndex == 9){
    peq.status = !peq.status;
  }
  tft.fillRect(122, 210, 150, 60, peq.status ? RA8875_YELLOW : RA8875_GREEN);
  drawButtonText(tft, 128, 216, "ON/OFF", RA8875_BLACK, peq.status ? RA8875_YELLOW : RA8875_GREEN, 2);

  if ((currentButtonIndex >= 1 && currentButtonIndex <= 8  && peq.status == true) || currentButtonIndex == 10) {
    peq.ls = (currentButtonIndex == 1);
    peq.b1 = (currentButtonIndex == 2);
    peq.hp = (currentButtonIndex == 3);
    peq.b2 = (currentButtonIndex == 4);
    peq.b3 = (currentButtonIndex == 5);
    peq.hs = (currentButtonIndex == 6);
    peq.b4 = (currentButtonIndex == 7);
    peq.lp = (currentButtonIndex == 8);
    if (currentButtonIndex == 10) peq.hpf = !peq.hpf;
    if (currentButtonIndex == 1) peq.bs1 = 1;
    if (currentButtonIndex == 2) peq.bs1 = 2;
    if (currentButtonIndex == 3) peq.bs1 = 3;
    if (currentButtonIndex == 6) peq.bs2 = 1;
    if (currentButtonIndex == 7) peq.bs2 = 2;
    if (currentButtonIndex == 8) peq.bs2 = 3;
  }

  // Redraw buttons with new background colors
  tft.fillRect(280, 278, 86, 60, peq.ls ? RA8875_YELLOW : RA8875_GREEN);
  drawButtonText(tft, 300, 287, "L.S", RA8875_BLACK,  peq.ls ? RA8875_YELLOW : RA8875_GREEN, 1);
 
  tft.fillRect(280, 346, 86, 60, peq.b1 ? RA8875_YELLOW : RA8875_GREEN);
  drawButtonText(tft, 300, 360, "B.1", RA8875_BLACK, peq.b1 ? RA8875_YELLOW : RA8875_GREEN, 1);

  tft.fillRect(280, 414, 86, 60, peq.hp ? RA8875_YELLOW : RA8875_GREEN);
  drawButtonText(tft, 300, 428, "H.P", RA8875_BLACK, peq.hp ? RA8875_YELLOW : RA8875_GREEN, 1);

  if(peq.bs1 == 1 && peq.ls == false){
    tft.fillRect(280, 278, 86, 60, RA8875_BLUE);
    drawButtonText(tft, 300, 287, "L.S", RA8875_BLACK, RA8875_BLUE, 1);
  } else if(peq.bs1 == 2 && peq.b1 == false){
    tft.fillRect(280, 346, 86, 60, RA8875_BLUE);
    drawButtonText(tft, 300, 360, "B.1", RA8875_BLACK, RA8875_BLUE, 1);
  }else if(peq.bs1 == 3 && peq.hp == false){
    tft.fillRect(280, 414, 86, 60, RA8875_BLUE);
    drawButtonText(tft, 300, 428, "H.P", RA8875_BLACK, RA8875_BLUE, 1);
  }

  tft.fillRect(374, 346, 81, 60, peq.b2 ? RA8875_YELLOW : RA8875_GREEN);
  drawButtonText(tft, 394, 360, "B.2", RA8875_BLACK, peq.b2 ? RA8875_YELLOW : RA8875_GREEN, 1);

  tft.fillRect(463, 346, 81, 60, peq.b3 ? RA8875_YELLOW : RA8875_GREEN);
  drawButtonText(tft, 483, 360, "B.3", RA8875_BLACK, peq.b3 ? RA8875_YELLOW : RA8875_GREEN, 1);



  tft.fillRect(552, 278, 86, 60, peq.hs ? RA8875_YELLOW : RA8875_GREEN);
  drawButtonText(tft, 572, 287, "H.S", RA8875_BLACK, peq.hs ? RA8875_YELLOW : RA8875_GREEN, 1);

  tft.fillRect(552, 346, 86, 60, peq.b4 ? RA8875_YELLOW : RA8875_GREEN);
  drawButtonText(tft, 572, 360, "B.4", RA8875_BLACK, peq.b4 ? RA8875_YELLOW : RA8875_GREEN, 1);

  tft.fillRect(552, 414, 86, 60, peq.lp ? RA8875_YELLOW : RA8875_GREEN);
  drawButtonText(tft, 572, 428, "L.P", RA8875_BLACK, peq.lp ? RA8875_YELLOW : RA8875_GREEN, 1);

 if(peq.bs2 == 1 && peq.hs == false){
    tft.fillRect(552, 278, 86, 60, RA8875_BLUE);
    drawButtonText(tft, 572, 287, "H.S", RA8875_BLACK, RA8875_BLUE, 1);
  } else if(peq.bs2 == 2 && peq.b4 == false){
    tft.fillRect(552, 346, 86, 60, RA8875_BLUE);
    drawButtonText(tft, 572, 360, "B.4", RA8875_BLACK, RA8875_BLUE, 1);
  }else if(peq.bs2 == 3 && peq.lp == false){
    tft.fillRect(552, 414, 86, 60, RA8875_BLUE);
    drawButtonText(tft, 572, 428, "L.P", RA8875_BLACK, RA8875_BLUE, 1);
  }

  tft.fillRect(646, 6, 136, 60, peq.hpf ? RA8875_BLUE : RA8875_CYAN);
  drawButtonText(tft, 648, 18, "HighPass", RA8875_BLACK, peq.hpf ? RA8875_BLUE : RA8875_CYAN, 1);

  Serial.println("Current PEQ state:");
  Serial.print("LS: "); Serial.println(peq.ls ? "ON" : "OFF");
  Serial.print("B1: "); Serial.println(peq.b1 ? "ON" : "OFF");
  Serial.print("HP: "); Serial.println(peq.hp ? "ON" : "OFF");
  Serial.print("B2: "); Serial.println(peq.b2 ? "ON" : "OFF");
  Serial.print("B3: "); Serial.println(peq.b3? "ON" : "OFF");
  Serial.print("HS: "); Serial.println(peq.hs? "ON" : "OFF");
  Serial.print("B4: "); Serial.println(peq.b4? "ON" : "OFF");
  Serial.print("LP: "); Serial.println(peq.lp? "ON" : "OFF");
  Serial.print("On/Off: "); Serial.println(peq.status? "ON" : "OFF");
}

void handleChannelPress(bool& button, channel& ch, int buttonIndex, unsigned long& lastButtonPress, unsigned long currentMillis, int debounceTime, Adafruit_RA8875 &tft) {
  if (currentMillis - lastButtonPress >= debounceTime) {
    bool previousButtonState = button;
    button = true;
    lastButtonPress = currentMillis;
    Serial.print("Button ");
    Serial.print(buttonIndex);
    Serial.println(" Toggle");

    if (previousButtonState != button) { // Only reset other buttons if the current button state has changed
      resetOtherChannels(ch, buttonIndex, tft);
      updateSliders(tft, ch.peqs[ch.active -1]);
    }
  }
}



void resetOtherChannels(channel& ch, int currentButtonIndex, Adafruit_RA8875 &tft) {
  if (currentButtonIndex >= 1 && currentButtonIndex <= 6) {
    ch.input1 = (currentButtonIndex == 1);
    ch.input2 = (currentButtonIndex == 2);
    ch.input3 = (currentButtonIndex == 3);
    ch.input4 = (currentButtonIndex == 4);
    ch.pri = (currentButtonIndex == 5);
    ch.aux = (currentButtonIndex == 6);
    ch.active = currentButtonIndex;
    tft.fillRect(4, 74, 110, 60, ch.input1 ? RA8875_YELLOW : RA8875_GREEN);
    drawButtonText(tft, 10, 80, "IN 1", RA8875_BLACK, ch.input1 ? RA8875_YELLOW : RA8875_GREEN, 2);
    tft.fillRect(4, 142, 110, 60, ch.input2 ? RA8875_YELLOW : RA8875_GREEN);
    drawButtonText(tft, 10, 148, "IN 2", RA8875_BLACK, ch.input2 ? RA8875_YELLOW : RA8875_GREEN, 2);
    tft.fillRect(4, 210, 110, 60, ch.input3 ? RA8875_YELLOW : RA8875_GREEN);
    drawButtonText(tft, 10, 216, "IN 3", RA8875_BLACK, ch.input3 ? RA8875_YELLOW : RA8875_GREEN, 2);
    tft.fillRect(4, 278, 110, 60, ch.input4 ? RA8875_YELLOW : RA8875_GREEN);
    drawButtonText(tft, 10, 284, "IN 4", RA8875_BLACK, ch.input4 ? RA8875_YELLOW : RA8875_GREEN, 2);
    tft.fillRect(4, 346, 110, 60, ch.pri ? RA8875_YELLOW : RA8875_GREEN);
    drawButtonText(tft, 22, 352, "PRI", RA8875_BLACK, ch.pri ? RA8875_YELLOW : RA8875_GREEN, 2);
    tft.fillRect(4, 414, 110, 60, ch.aux ? RA8875_YELLOW : RA8875_GREEN);
    drawButtonText(tft, 22, 420, "AUX", RA8875_BLACK, ch.aux ? RA8875_YELLOW : RA8875_GREEN, 2);

    Serial.println("Current channel state:");
    Serial.print("Input1: "); Serial.println(ch.input1 ? "ON" : "OFF");
    Serial.print("Input2: "); Serial.println(ch.input2 ? "ON" : "OFF");
    Serial.print("Input3: "); Serial.println(ch.input3 ? "ON" : "OFF");
    Serial.print("Input4: "); Serial.println(ch.input4 ? "ON" : "OFF");
    Serial.print("PRI: "); Serial.println(ch.pri ? "ON" : "OFF");
    Serial.print("AUX: "); Serial.println(ch.aux ? "ON" : "OFF");
    checkPEQButtons(ch.peqs[ch.active - 1], -1, tft);
  }
}

void drawButtonText(Adafruit_RA8875 &tft, int x, int y, const String &text, uint16_t textColor, uint16_t bgColor, int size) {
  tft.textMode();
  tft.textColor(textColor, bgColor);
  tft.textEnlarge(size);
  tft.textSetCursor(x, y);
  tft.textWrite(text.c_str());
  tft.graphicsMode();
}


void drawMenu(bool push, Adafruit_RA8875 tft, peq_buttons &peq, channel& ch) {
  if (push) {
    printPEQ(tft, peq, ch);
  } else {
    printMenu(tft);
  }
}

void printPEQ(Adafruit_RA8875 &tft, peq_buttons &peq, channel& ch) {
  tft.fillScreen(RA8875_WHITE);
  // Draw all boxes
  drawAllBoxes(tft, peq, ch);

  // Print all text
  printAllText(tft, peq, ch);

  updateSliders(tft, ch.peqs[ch.active - 1]);

  // Put back into graphics mode
  tft.graphicsMode();
}

void drawAllBoxes(Adafruit_RA8875 &tft, peq_buttons &peq, channel& ch) {
  // Draw left buttons
  drawLeftButtons(tft, ch);

  // Draw Cutoff, Bandwidth, Gain, On/Off boxes
  drawCutoffBandwidthGainBoxes(tft, peq);

  // Draw High-Pass Filter boxes
  drawHighPassFilterBoxes(tft, peq);

  // Draw Band Selectors
  drawBandSelectors(tft, peq);
 
}

void drawLeftButtons(Adafruit_RA8875 &tft, channel& ch) {
  tft.fillRect(0, 0, 120, 480, RA8875_BLACK);
  tft.fillRect(4, 6, 110, 60, RA8875_GREEN); // IND
  tft.fillRect(4, 74, 110, 60,  ch.input1 ? RA8875_YELLOW : RA8875_GREEN); //IN 1
  tft.fillRect(4, 142, 110, 60, ch.input2? RA8875_YELLOW : RA8875_GREEN); //IN 2
  tft.fillRect(4, 210, 110, 60, ch.input3 ? RA8875_YELLOW : RA8875_GREEN); //IN 3
  tft.fillRect(4, 278, 110, 60, ch.input4 ? RA8875_YELLOW : RA8875_GREEN); //IN 4
  tft.fillRect(4, 346, 110, 60, ch.pri ? RA8875_YELLOW : RA8875_GREEN); //Pri
  tft.fillRect(4, 414, 110, 60, ch.aux ? RA8875_YELLOW : RA8875_GREEN); //Aux
}

void drawCutoffBandwidthGainBoxes(Adafruit_RA8875 &tft, peq_buttons &peq) {
  tft.fillRect(120, 0, 158, 280, RA8875_BLACK); // background
  tft.fillRect(122, 6, 150, 60, RA8875_CYAN); // cutoff
  tft.fillRect(122, 74, 150, 60, RA8875_CYAN); // band
  tft.fillRect(122, 142, 150, 60, RA8875_CYAN); // gain
  tft.fillRect(122, 210, 150, 60, peq.status ? RA8875_YELLOW : RA8875_GREEN); // on/off
}

void drawHighPassFilterBoxes(Adafruit_RA8875 &tft, peq_buttons &peq) {
  tft.fillRect(638, 0, 162, 140, RA8875_BLACK);
  tft.fillRect(646, 6, 134, 60, peq.hpf ? RA8875_BLUE : RA8875_CYAN);; // high pass
  tft.fillRect(646, 74, 134, 60, RA8875_CYAN); // cut
}

void drawBandSelectors(Adafruit_RA8875 &tft, peq_buttons &peq) {
  tft.fillRect(272, 270, 374, 214, RA8875_BLACK);
  tft.fillRect(280, 278, 86, 60, peq.ls ? RA8875_YELLOW : RA8875_GREEN); //low shelf
  tft.fillRect(280, 346, 86, 60, peq.b1 ? RA8875_YELLOW : RA8875_GREEN); // bell 1
  tft.fillRect(280, 414, 86, 60, peq.hp ? RA8875_YELLOW : RA8875_GREEN); // high pass
  tft.fillRect(374, 278, 170, 60, RA8875_CYAN); // c.band
  tft.fillRect(374, 346, 81, 60, peq.b2 ? RA8875_YELLOW : RA8875_GREEN); // bell 2
  tft.fillRect(463, 346, 81, 60, peq.b3 ? RA8875_YELLOW : RA8875_GREEN); // bell 3
  tft.fillRect(374, 414, 170, 60, RA8875_CYAN);// input
  tft.fillRect(552, 278, 86, 60, peq.hs ? RA8875_YELLOW : RA8875_GREEN); // high shelf
  tft.fillRect(552, 346, 86, 60, peq.b4 ? RA8875_YELLOW : RA8875_GREEN); // bell 4
  tft.fillRect(552, 414, 86, 60, peq.lp ? RA8875_YELLOW : RA8875_GREEN); // low pass
}

void printAllText(Adafruit_RA8875 &tft, peq_buttons &peq, channel& ch) {
  // Print text in left buttons
  printLeftButtonsText(tft, ch);

  // Print Cut/Band/Gain text
  printCutoffBandwidthGainText(tft, peq);

  // Print High-Pass Filter text
  printHighPassFilterText(tft, peq);

  // Print Band Selector text
  printBandSelectorText(tft, peq);

  // Print Input Selection Text
  printInputSelectionText(tft);
}

void printLeftButtonsText(Adafruit_RA8875 &tft, channel& ch) {
  drawButtonText(tft, 22, 12, "IND", RA8875_BLACK, RA8875_GREEN, 2);
  drawButtonText(tft, 10, 80, "IN 1", RA8875_BLACK, ch.input1 ? RA8875_YELLOW : RA8875_GREEN, 2);
  drawButtonText(tft, 10, 148, "IN 2", RA8875_BLACK, ch.input2? RA8875_YELLOW : RA8875_GREEN, 2);
  drawButtonText(tft, 10, 216, "IN 3", RA8875_BLACK, ch.input3 ? RA8875_YELLOW : RA8875_GREEN, 2);
  drawButtonText(tft, 10, 284, "IN 4", RA8875_BLACK, ch.input4 ? RA8875_YELLOW : RA8875_GREEN, 2);
  drawButtonText(tft, 22, 352, "PRI", RA8875_BLACK, ch.pri ? RA8875_YELLOW : RA8875_GREEN, 2);
  drawButtonText(tft, 22, 420, "AUX", RA8875_BLACK, ch.aux ? RA8875_YELLOW : RA8875_GREEN, 2);
}

void printCutoffBandwidthGainText(Adafruit_RA8875 &tft, peq_buttons &peq) {
  drawButtonText(tft, 128, 216, "ON/OFF", RA8875_BLACK, peq.status ? RA8875_YELLOW : RA8875_GREEN, 2);
  tft.textMode();
  tft.textEnlarge(1);
  tft.textColor(RA8875_BLACK, RA8875_CYAN);
  tft.textSetCursor(128, 18);
  tft.textWrite("Cut: ");
  tft.textSetCursor(128, 86);
  tft.textWrite("Band: ");
  tft.textSetCursor(128, 154);
  tft.textWrite("Gain: ");
  tft.graphicsMode();
}

void printHighPassFilterText(Adafruit_RA8875 &tft, peq_buttons &peq) {
  drawButtonText(tft, 648, 18, "HighPass", RA8875_BLACK, peq.hpf ? RA8875_BLUE : RA8875_CYAN, 1);
  tft.textMode();
  tft.textEnlarge(1);
  tft.textColor(RA8875_BLACK, RA8875_CYAN);
  tft.textSetCursor(668, 86);
  tft.textWrite("Cut: ");
  tft.graphicsMode();
}

void printBandSelectorText(Adafruit_RA8875 &tft, peq_buttons &peq) {
  tft.textColor(RA8875_BLACK, RA8875_GREEN);
  tft.textEnlarge(1);
  drawButtonText(tft, 300, 287, "L.S", RA8875_BLACK,  peq.ls ? RA8875_YELLOW : RA8875_GREEN, 1);
  drawButtonText(tft, 300, 360, "B.1", RA8875_BLACK, peq.b1 ? RA8875_YELLOW : RA8875_GREEN, 1);
  drawButtonText(tft, 300, 428, "H.P", RA8875_BLACK, peq.hp ? RA8875_YELLOW : RA8875_GREEN, 1);
  if(peq.bs1 == 1 && peq.ls == false){
    tft.fillRect(280, 278, 86, 60, RA8875_BLUE);
    drawButtonText(tft, 300, 287, "L.S", RA8875_BLACK, RA8875_BLUE, 1);
  } else if(peq.bs1 == 2 && peq.b1 == false){
    tft.fillRect(280, 346, 86, 60, RA8875_BLUE);
    drawButtonText(tft, 300, 360, "B.1", RA8875_BLACK, RA8875_BLUE, 1);
  }else if(peq.bs1 == 3 && peq.hp == false){
    tft.fillRect(280, 414, 86, 60, RA8875_BLUE);
    drawButtonText(tft, 300, 428, "H.P", RA8875_BLACK, RA8875_BLUE, 1);
  }
  drawButtonText(tft, 394, 360, "B.2", RA8875_BLACK, peq.b2 ? RA8875_YELLOW : RA8875_GREEN, 1);
  drawButtonText(tft, 483, 360, "B.3", RA8875_BLACK, peq.b3 ? RA8875_YELLOW : RA8875_GREEN, 1);
  drawButtonText(tft, 572, 287, "H.S", RA8875_BLACK, peq.hs ? RA8875_YELLOW : RA8875_GREEN, 1);
  drawButtonText(tft, 572, 360, "B.4", RA8875_BLACK, peq.b4 ? RA8875_YELLOW : RA8875_GREEN, 1);
  drawButtonText(tft, 572, 428, "L.P", RA8875_BLACK, peq.lp ? RA8875_YELLOW : RA8875_GREEN, 1);
  if(peq.bs2 == 1 && peq.hs == false){
    tft.fillRect(552, 278, 86, 60, RA8875_BLUE);
    drawButtonText(tft, 572, 287, "H.S", RA8875_BLACK, RA8875_BLUE, 1);
  } else if(peq.bs2 == 2 && peq.b4 == false){
    tft.fillRect(552, 346, 86, 60, RA8875_BLUE);
    drawButtonText(tft, 572, 360, "B.4", RA8875_BLACK, RA8875_BLUE, 1);
  }else if(peq.bs2 == 3 && peq.lp == false){
    tft.fillRect(552, 414, 86, 60, RA8875_BLUE);
    drawButtonText(tft, 572, 428, "L.P", RA8875_BLACK, RA8875_BLUE, 1);
  }
}

void updateSliders(Adafruit_RA8875 &tft, peq_buttons &peq){
  if(peq.hpf){
    drawHPFSlider(tft,peq.hpfSlider.currentSliderPos, peq.hpfSlider);
  }else{
      tft.fillRect(700, 140, 40, 330, RA8875_WHITE);
  }
}
void handleHPFSlider(Adafruit_RA8875 &tft, int y, Slider &slide, peq_buttons &peq) {
  slide.lastSliderPos = -1;
  slide.currentSliderPos = -1;

  slide.currentSliderPos = constrain(y, slide.y_begin, slide.y_end);
  slide.slider_value  = (float)(slide.y_end - slide.currentSliderPos) / (slide.y_end - slide.y_begin) * 100;
  Serial.println(slide.slider_value);

  if (slide.currentSliderPos != -1 && slide.currentSliderPos != slide.lastSliderPos && peq.hpf) {
    drawHPFSlider(tft, slide.currentSliderPos, slide);
    slide.lastSliderPos = slide.currentSliderPos;
  }else{
      tft.fillRect(slide.x_begin-20, slide.y_end+30, 41, slide.y_begin - slide.y_end-20, RA8875_WHITE);
  }
}

void drawHPFSlider(Adafruit_RA8875 &tft, int circleY , Slider &slide) {
  tft.fillRect(slide.x_begin-20, slide.y_end+10, 41, slide.y_begin - slide.y_end-20, RA8875_WHITE);  // Update the rectangle to start at the end Y position and go to the start Y position
  tft.drawLine(slide.x_begin, slide.y_begin, slide.x_begin, slide.y_end, RA8875_BLACK);
  if(circleY == -1){
    tft.fillCircle(slide.x_begin, slide.y_end, 10, RA8875_BLUE);
  }else{

    tft.fillCircle(slide.x_begin, circleY, 10, RA8875_BLUE);
  }
}


void initializeChannel(channel &ch) {

    for (int i = 0; i < 6; ++i) {
        ch.peqs[i].hpfSlider.x_begin = 720;
        ch.peqs[i].hpfSlider.x_end = 720;
        ch.peqs[i].hpfSlider.y_begin = 160;
        ch.peqs[i].hpfSlider.y_end = 460;
        ch.peqs[i].hpfSlider.currentSliderPos = -1;
        ch.peqs[i].hpfSlider.lastSliderPos = -1;
        ch.peqs[i].hpfSlider.slider_value = 0;
    }
}



void printInputSelectionText(Adafruit_RA8875 &tft) {
  tft.textMode();
  tft.textColor(RA8875_BLACK, RA8875_CYAN);
  tft.textSetCursor(390, 287);
  tft.textWrite("PEQ BANDS");
  tft.graphicsMode();
}
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
