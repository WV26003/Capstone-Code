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

void mute_control(AudioMixer4 &mixer, int channel_num, bool &state){
  	if (state == true) {
     		mute(mixer, channel_num);
  	}
  	else {
    		unmute(mixer, channel_num);
  	}
}

//Controlling an amp object to toggle output muting
void output_mute(AudioAmplifier &amp){
    amp.gain(0);
}

void output_unmute(AudioAmplifier &amp){
    amp.gain(1);
}

//Controlling an amp object to toggle output muting
void output_mix_mute_control(AudioAmplifier &amp, bool &state){
  	if (state == true) {
     		output_mute(amp);
  	}
  	else {
    		output_unmute(amp);
  	}
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

/*
enum Field{
	set1 = 1,
	set2 = 2,
	set3 = 4,
	set4 = 8
};

#define SET1 1
#define SET2 2

byte b = SET1 | SET2;

if(b & SET1) do something
*/

//setting mute status array
void muting_status_array(seesaw_NeoPixel encoder_pixels[], bool muting_status[]){
													//If the pixel is on, the associated channel should be muted
	if (encoder_pixels[0].getPixelColor(0) > 0) {
		muting_status[0] = true;
	}
	else {
		muting_status[0] = false;
	}

	if (encoder_pixels[1].getPixelColor(0) > 0) {
		muting_status[1] = true;
	}
	else {
		muting_status[1] = false;
	}

	if (encoder_pixels[2].getPixelColor(0) > 0) {
		muting_status[2] = true;
	}
	else {
		muting_status[2] = false;
	}

	if (encoder_pixels[3].getPixelColor(0) > 0) {
		muting_status[3] = true;
	}
	else {
		muting_status[3] = false;
	}
	//Serial.print(muting_status[0]);
	//Serial.print(muting_status[1]);
	//Serial.print(muting_status[2]);
	//Serial.println(muting_status[3]);
}


/*
//Input mute Control and mixer gain adjustment
void input_muting(AudioMixer4 &mixer1, AudioMixer4 &mixer2, seesaw_NeoPixel encoder_pixels[], bool muting_status[]){
	int numOn = 0;							//Counts the number of input channels on
	for(size_t i = 0; i < 4; i++){
        if(muting_status[i] == false){
			numOn++;
		}
    }
	Serial.print(numOn);
	Serial.print(", ");
	
	
	
	if(numOn != 0){							//Muting and scaling the gain of the appropriate input channels
        float numerator = 1;
		float gainVal = numerator/numOn;
		Serial.println(gainVal);
		
		for(size_t i = 0; i < 4; i++){
			if(muting_status[i] == false){
				
            
			mixer1.gain(i, gainVal);
			mixer2.gain(i, gainVal);
			}
			else {
            mixer1.gain(i, 0);
			mixer2.gain(i, 0);
			}
        }
    }
	else {									//If no input channels are on (they are all muted) then mute all mixer channels
            mixer1.gain(0, 0);
			mixer1.gain(1, 0);
			mixer1.gain(2, 0);
			mixer1.gain(3, 0);
			
			mixer2.gain(0, 0);
			mixer2.gain(1, 0);
			mixer2.gain(2, 0);
			mixer2.gain(3, 0);
			
			Serial.println("0");
        }

}
*/


//Input mute Control and mixer gain adjustment
void input_muting(AudioMixer4 &mixer1, AudioMixer4 &mixer2, seesaw_NeoPixel encoder_pixels[], bool muting_status[]){
	int numOn = 0;							//Counts the number of input channels on
	for(size_t i = 0; i < 4; i++){
        if(muting_status[i] == false){
			numOn++;
		}
    }
	Serial.print(numOn);
	Serial.print(", ");
	
	
	
	if(numOn != 0){							//Muting and scaling the gain of the appropriate input channels
        //float numerator = 1;
		//float gainVal = numerator/numOn;
		//Serial.println(gainVal);
		
		for(size_t i = 0; i < 4; i++){
			if(muting_status[i] == false){
				
            
			mixer1.gain(i, 0.25);
			mixer2.gain(i, 0.25);
			}
			else {
            mixer1.gain(i, 0);
			mixer2.gain(i, 0);
			}
        }
    }
	else {									//If no input channels are on (they are all muted) then mute all mixer channels
            mixer1.gain(0, 0);
			mixer1.gain(1, 0);
			mixer1.gain(2, 0);
			mixer1.gain(3, 0);
			
			mixer2.gain(0, 0);
			mixer2.gain(1, 0);
			mixer2.gain(2, 0);
			mixer2.gain(3, 0);
			
			Serial.println("0");
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
  	// did we move around?

	//Serial.print("NP: ");													//For Testing Purposes
	//Serial.print(new_position);
	//Serial.print(", ");
	//Serial.print("EP: ");													//For Testing Purposes
	//Serial.print(encoder_position);
	//Serial.println(", ");
	
	if (new_position == 0) {
		amp.gain(0);
	}

  	if (encoder_position != new_position) {
    		if ((new_position < 61) && (new_position > -1)) {
    			float position_math = new_position;
    			if (position_math <= 45) {													//Segmenting the Fader into a lower 3/4 and an upper 1/4

				float scaled_lower = (position_math/45);										//Obtaining a 0-1 percentage of the lower 3/4 of the fader
				amp.gain(scaled_lower);													//Input the previous value into the gain function to achieve an attentuation up to pass-through
		
				float gain_lower = 20*log10(scaled_lower);
				//Serial.print("GL: ");													//For Testing Purposes
				//Serial.print(gain_lower);
				//Serial.println(", ");
				}
				else if (position_math > 45){												//Scaling the upper 1/4 of the fader
				float min_val = 45;						
				float max_val = 60;
				float scaled_upper = ((position_math/max_val) - (min_val/max_val));			//Obtaining a 0-1 percentage of the upper 1/4 of the fader
  
				float new_max_val = .25;											
				float scaling_buffer = (1 + (2.16) * (scaled_upper/new_max_val));		//Scaling the .gain input between 1 and 3.16
				amp.gain(scaling_buffer);												//Input the previous value into the gain function to achieve pass-through up to 10dB
	
				float gain_upper = 20*log10(scaling_buffer);
				//Serial.print("GU: ");													//For Testing Purposes
				//Serial.print(gain_upper);
				//Serial.println(", ");
				}
	
    		}
		encoder_position = new_position;
	}
	if (new_position > 60) {
		seesaw.setEncoderPosition(60);
	}
	else if (new_position < 0) {
		seesaw.setEncoderPosition(0);
    }

}