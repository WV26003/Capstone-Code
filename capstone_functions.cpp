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

/*
void linear_fader(Adafruit_seesaw &seesaw, AudioAmplifier &amp, int analog_in){
    uint16_t analog_read = seesaw.analogRead(analog_in);
    float fade = analog_read;
    float scalar = (fade/1023)*3.16;
    amp.gain(scalar);

	//Serial.print(scalar);
	//Serial.print(", ");
}
*/

void linear_fader(Adafruit_seesaw &seesaw, AudioAmplifier &amp, int analog_in){
    uint16_t analog_read = seesaw.analogRead(analog_in);					//Reading in analog value
	float buffer_1 = analog_read;											//Converting Analog Value to float

	if (buffer_1 <= 767) {													//Segmenting the Fader into a lower 3/4 and an upper 1/4

	float scaled_lower = (buffer_1/767);									//Obtaining a 0-1 percentage of the lower 3/4 of the fader
	amp.gain(scaled_lower);													//Input the previous value into the gain function to achieve an attentuation up to pass-through
	
	float gain_lower = 20*log10(scaled_lower);
	//Serial.print("GL: ");													For Testing Purposes
	//Serial.print(gain_lower);
    //Serial.println(", ");
	}
	else if (buffer_1 > 767){												//Scaling the upper 1/4 of the fader
	float min_val = 767;						
	float max_val = 1023;
	float scaled_upper = ((buffer_1/max_val) - (min_val/max_val));			//Obtaining a 0-1 percentage of the upper 1/4 of the fader
  
	float new_max_val = .25;											
	float scaling_buffer = (1 + (2.16) * (scaled_upper/new_max_val));		//Scaling the .gain input between 1 and 3.16
	amp.gain(scaling_buffer);												//Input the previous value into the gain function to achieve pass-through up to 10dB
	
	float gain_upper = 20*log10(scaling_buffer);
	//Serial.print("GU: ");													For Testing Purposes
	//Serial.print(gain_upper);
	//Serial.println(", ");
	}
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
				encoder_position = new_position;      // and save for next round
				//Serial.print(gain);
				//Serial.print(", ");
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
*/

void encoder_fader(Adafruit_seesaw &seesaw, AudioAmplifier &amp, int32_t &encoder_position) {
	int32_t new_position = seesaw.getEncoderPosition();
	float gain = 0;
  	// did we move around?
  	if (encoder_position != new_position) {
    		if ((new_position < 61) && (new_position > -1)) {
    			float position_math = new_position;
    			if (position_math <= 45) {													//Segmenting the Fader into a lower 3/4 and an upper 1/4

				float scaled_lower = (position_math/45);										//Obtaining a 0-1 percentage of the lower 3/4 of the fader
				amp.gain(scaled_lower);													//Input the previous value into the gain function to achieve an attentuation up to pass-through
		
				float gain_lower = 20*log10(scaled_lower);
				Serial.print("GL: ");													//For Testing Purposes
				Serial.print(gain_lower);
				Serial.println(", ");
				}
				else if (position_math > 45){												//Scaling the upper 1/4 of the fader
				float min_val = 45;						
				float max_val = 60;
				float scaled_upper = ((position_math/max_val) - (min_val/max_val));			//Obtaining a 0-1 percentage of the upper 1/4 of the fader
  
				float new_max_val = .25;											
				float scaling_buffer = (1 + (2.16) * (scaled_upper/new_max_val));		//Scaling the .gain input between 1 and 3.16
				amp.gain(scaling_buffer);												//Input the previous value into the gain function to achieve pass-through up to 10dB
	
				float gain_upper = 20*log10(scaling_buffer);
				Serial.print("GU: ");													//For Testing Purposes
				Serial.print(gain_upper);
				Serial.println(", ");
				}
				
			encoder_position = new_position;      // and save for next round
			
    		}
	}
    if (new_position > 60) {
		seesaw.setEncoderPosition(60);
	}
	else if (new_position <= 0) {
		seesaw.setEncoderPosition(0);
		amp.gain(0);
    }		


}