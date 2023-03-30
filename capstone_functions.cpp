

/// Header
#include "capstone_functions.h"
 

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

void linear_fader(Adafruit_seesaw &seesaw, AduioAmplifier &amp, int analog_in){
    unint16_t analog_read = seesaw.analogRead(analog_in);
    float fade = analog_read;
    amp.gain(fade/1023);
}

