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

	Serial.print(scalar);
	Serial.print(", ");
}

void output_mix_mute_control(AudioMixer4 &mixer, int channel_num, bool &state){
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


/*

void encoder_preamp(Adafruit_seesaw &seesaw, AudioControlSGTL5000 sgtl5000, int32_t &encoder_position) {
	int32_t new_position = seesaw.getEncoderPosition();
  	// did we move around?
  	if (encoder_position != new_position) {
    		if ((new_position < 21) && (new_position > -1)) {
    			float position_math = new_position;
			int preamp = round((position_math/20) * 63);
			sgt15000.micGain(preamp);
    		}
    		if (new_position > 20) {
      		seesaw.setEncoderPosition(20);
   		}
    		else if (new_position < 0) {
      	seesaw.setEncoderPosition(0);
    		}
	}

}

*/

/*
void encoder_fader(Adafruit_seesaw &seesaw, AudioAmplifier &amp, int32_t &encoder_position) {
	int32_t new_position = seesaw.getEncoderPosition();
	float gain = 0;
  	// did we move around?
  	if (encoder_position != new_position) {
    		if ((new_position < 21) && (new_position > -1)) {
    			float position_math = new_position;
    			gain = (position_math/20)*3.16;	// 3.16 scales the gain to a maximum of 10dB	
				amp.gain(gain);
				
			Serial.print(gain);
			Serial.print(", ");
    		}
    		if (new_position > 20) {
      		seesaw.setEncoderPosition(20);
			}
    		else if (new_position < 0) {
      	seesaw.setEncoderPosition(0);
    		}
	}


}
*/
/*
void encoder_fader(Adafruit_seesaw &seesaw, AudioAmplifier &amp, int32_t &encoder_position) {
	int32_t new_position = seesaw.getEncoderPosition();
	float gain = 0;
  	// did we move around?
  	if (encoder_position != new_position) {
    		if ((new_position < 21) && (new_position > -1)) {
    			float position_math = new_position;
    			gain = (position_math/20)*3.16;	// 3.16 scales the gain to a maximum of 10dB	
				amp.gain(gain);
				
			Serial.print(gain);
			Serial.print(", ");
			encoder_position = new_position;      // and save for next round
    		}
    		if (new_position > 20) {
      		seesaw.setEncoderPosition(20);
			}
    		else if (new_position < 0) {
      	seesaw.setEncoderPosition(0);
    		}
	}


}
*/

void encoder_fader(Adafruit_seesaw &seesaw, AudioAmplifier &amp, int32_t &encoder_position) {
	int32_t new_position = seesaw.getEncoderPosition();
	float gain = 0;
  	// did we move around?
  	if (encoder_position != new_position) {
    		if ((new_position < 21) && (new_position > -1)) {
    			float position_math = new_position;
    			gain = (position_math/20)*3.16;	// 3.16 scales the gain to a maximum of 10dB	
				amp.gain(gain);
				encoder_position = new_position;      // and save for next round
				Serial.print(gain);
				Serial.print(", ");
    		}
	}
    		if (new_position > 20) {
      		seesaw.setEncoderPosition(20);
			}
    		else if (new_position <= 0) {
			seesaw.setEncoderPosition(0);
			amp.gain(0);
    		}		


}