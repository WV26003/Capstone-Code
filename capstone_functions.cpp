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


