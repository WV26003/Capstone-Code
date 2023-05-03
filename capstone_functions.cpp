/// Header
#include <Arduino.h>
#include "capstone_functions.h"
#define BUTTON_DEBOUNCE_TIME 5 



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

float linear_fader(Adafruit_seesaw &seesaw, AudioAmplifier &amp, int analog_in){
    uint16_t analog_read = seesaw.analogRead(analog_in);					//Reading in analog value
	float buffer_1 = analog_read;											//Converting Analog Value to float
	if (buffer_1 <= 767) {													//Segmenting the Fader into a lower 3/4 and an upper 1/4

		float scaled_lower = (buffer_1/767);									//Obtaining a 0-1 percentage of the lower 3/4 of the fader
		amp.gain(scaled_lower);													//Input the previous value into the gain function to achieve an attentuation up to pass-through
		return scaled_lower;
	}
	else{												//Scaling the upper 1/4 of the fader
		float min_val = 767;						
		float max_val = 1023;
		float scaled_upper = ((buffer_1/max_val) - (min_val/max_val));			//Obtaining a 0-1 percentage of the upper 1/4 of the fader
		float new_max_val = .25;											
		float scaling_buffer = (1 + (2.16) * (scaled_upper/new_max_val));		//Scaling the .gain input between 1 and 3.16
		amp.gain(scaling_buffer);												//Input the previous value into the gain function to achieve pass-through up to 10dB
		return scaling_buffer;
	}
return 0;
}








/*
void linear_fader(Adafruit_seesaw &seesaw, AudioAmplifier &amp, int analog_in){
    uint16_t analog_read = seesaw.analogRead(analog_in);					//Reading in analog value
	float buffer_1 = analog_read;											//Converting Analog Value to float

	if (buffer_1 <= 767) {													//Segmenting the Fader into a lower 3/4 and an upper 1/4

	float scaled_lower = (buffer_1/767);									//Obtaining a 0-1 percentage of the lower 3/4 of the fader
	//Serial.print("SL: ");
	//Serial.println(scaled_lower);
	amp.gain(scaled_lower);													//Input the previous value into the gain function to achieve an attentuation up to pass-through
	
	//float gain_lower = 20*log10(scaled_lower);
	//Serial.print("GL: ");													//For Testing Purposes
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
	
	//float gain_upper = 20*log10(scaling_buffer);
	//Serial.print("GU: ");													//For Testing Purposes
	//Serial.print(gain_upper);
	//Serial.println(", ");
	}
}
*/


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





//Input mute Control and mixer gain adjustment
void input_muting(AudioMixer4 &mixer1, AudioMixer4 &mixer2, seesaw_NeoPixel encoder_pixels[], bool muting_status[]){
	int numOn = 0;							//Counts the number of input channels on
	for(size_t i = 0; i < 4; i++){
        	if(muting_status[i] == false){
			numOn++;
		}
    	}
	if(numOn != 0){							//Muting and scaling the gain of the appropriate input channels
		for(size_t i = 0; i < 4; i++){
			if(muting_status[i] == false){
				mixer1.gain(i, 0.25);
				mixer2.gain(i, 0.25);
			}
			else{
            			mixer1.gain(i, 0);
				mixer2.gain(i, 0);
			}
        	}
    	}
	else {									//If no input channels are on (they are all muted) then mute all mixer channels	Zmixer1.gain(0, 0);
		mixer1.gain(1, 0);
		mixer1.gain(2, 0);
		mixer1.gain(3, 0);	
		mixer2.gain(0, 0);
		mixer2.gain(1, 0);
		mixer2.gain(2, 0);
		mixer2.gain(3, 0);
        }
}


void monitoring(AudioMixer4 &mixer3, AudioMixer4 &mixer4, bool monitoring_status[]) {
	
	if (monitoring_status[4] == true || monitoring_status[5] == true) { 
		for(size_t i = 4; i < 6; i++){
			int channel = (i - 3);
			if(monitoring_status[i] == true){

				mixer4.gain(channel, 0.5);
				mixer4.gain(0, 0);
			}
			else {
				mixer4.gain(channel, 0);
			}
		}
	}
	else {
		mixer4.gain(1, 0);
		mixer4.gain(2, 0);
		for(size_t i = 0; i < 4; i++){
			if(monitoring_status[i] == true){
				mixer3.gain(i, 0.25);
				mixer4.gain(0, 1);
			}
			else {
				mixer3.gain(i, 0);
			}
		}
	}
}





float encoder_fader(Adafruit_seesaw &seesaw, AudioAmplifier &amp, int32_t &encoder_position) {
	int32_t new_position = seesaw.getEncoderPosition();
	if (new_position == 0) {
		amp.gain(0);
	}
  	if (encoder_position != new_position) {
    		if ((new_position < 61) && (new_position > -1)) {
    			float position_math = new_position;
    			if (position_math <= 45) {													//Segmenting the Fader into a lower 3/4 and an upper 1/4
				float scaled_lower = (position_math/45);										//Obtaining a 0-1 percentage of the lower 3/4 of the fader
				amp.gain(scaled_lower);													//For Testing Purposes
				Serial.print(scaled_lower);
				Serial.print(", ");
				return scaled_lower;
			}
			else if (position_math > 45){												//Scaling the upper 1/4 of the fader
				float min_val = 45;						
				float max_val = 60;
				float scaled_upper = ((position_math/max_val) - (min_val/max_val));			//Obtaining a 0-1 percentage of the upper 1/4 of the fader
				float new_max_val = .25;											
				float scaling_buffer = (1 + (2.16) * (scaled_upper/new_max_val));		//Scaling the .gain input between 1 and 3.16
				amp.gain(scaling_buffer);												//Input the previous value into the gain function to achieve pass-through up to 10dB
				Serial.print(scaling_buffer);
				Serial.print(", ");
				return scaling_buffer;
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
	return 0;
}















//Touchscreen

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

void handlePeqButtonPress(bool &button, peq_buttons &peq, int buttonIndex, unsigned long &lastButtonPress, unsigned long currentMillis, int debounceTime, Adafruit_RA8875 &tft) {
  //if (!peq.status) {
  //  return; // If PEQ is off, don't toggle any buttons
  //}

  if (currentMillis - lastButtonPress >= debounceTime) {
    lastButtonPress = currentMillis;
    Serial.print("Button ");
    Serial.print(buttonIndex);
    Serial.println(" Toggle");

    checkPEQButtons(peq, buttonIndex, tft);
  }
}
void checkPEQButtons(peq_buttons &peq, int currentButtonIndex, Adafruit_RA8875 &tft) {
  // Update button states
  if (currentButtonIndex == 9){
    peq.status = !peq.status;
  }
  if (currentButtonIndex >= 1 && currentButtonIndex <= 8) {
    Serial.println("In loop");
    if (currentButtonIndex == 1) {
      peq.bs1 = 1;
      peq.ls = true;
      peq.b1 = false;
      peq.hp = false;
      peq.b2 = false;
      peq.b3 = false;
      peq.hs = false;
      peq.b4 = false;
      peq.lp = false;
      }
    if (currentButtonIndex == 2)  {
      peq.bs1 = 2;
      peq.ls = false;
      peq.b1 = true;
      peq.hp = false;
      peq.b2 = false;
      peq.b3 = false;
      peq.hs = false;
      peq.b4 = false;
      peq.lp = false;
      }
    if (currentButtonIndex == 3){
      peq.bs1 = 3;
      peq.ls = false;
      peq.b1 = false;
      peq.hp = true;
      peq.b2 = false;
      peq.b3 = false;
      peq.hs = false;
      peq.b4 = false;
      peq.lp = false;
      }
    if (currentButtonIndex == 4)  {
      peq.ls = false;
      peq.b1 = false;
      peq.hp = false;
      peq.b2 = true;
      peq.b3 = false;
      peq.hs = false;
      peq.b4 = false;
      peq.lp = false;
      }
    if (currentButtonIndex == 5)  {
      peq.ls = false;
      peq.b1 = false;
      peq.hp = false;
      peq.b2 = false;
      peq.b3 = true;
      peq.hs = false;
      peq.b4 = false;
      peq.lp = false;
    }
    if (currentButtonIndex == 6)  {
      peq.bs2 = 1;
      peq.ls = false;
      peq.b1 = false;
      peq.hp = false;
      peq.b2 = false;
      peq.b3 = false;
      peq.hs = true;
      peq.b4 = false;
      peq.lp = false;
    }
    if (currentButtonIndex == 7)  {
      peq.bs2 = 2;
      peq.ls = false;
      peq.b1 = false;
      peq.hp = false;
      peq.b2 = false;
      peq.b3 = false;
      peq.hs = false;
      peq.b4 = true;
      peq.lp = false;
      }
    if (currentButtonIndex == 8)  {
      peq.bs2 = 3;
      peq.ls = false;
      peq.b1 = false;
      peq.hp = false;
      peq.b2 = false;
      peq.b3 = false;
      peq.hs = false;
      peq.b4 = false;
      peq.lp = true;
    }
  }
  tft.fillRect(122, 210, 150, 60, peq.status ? RA8875_YELLOW : RA8875_GREEN);
  drawButtonText(tft, 128, 216, "ON/OFF", RA8875_BLACK, peq.status ? RA8875_YELLOW : RA8875_GREEN, 2);


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
    }
  }
}



void resetOtherChannels(channel& ch, int currentButtonIndex, Adafruit_RA8875 &tft) {
  if (currentButtonIndex >= 1 && currentButtonIndex <= 6) {
    if (currentButtonIndex != 1) {
      ch.input1 = false;
      tft.fillRect(4, 74, 110, 60, RA8875_GREEN);
    } else {
      tft.fillRect(4, 74, 110, 60, RA8875_YELLOW);
    }
    drawButtonText(tft, 10, 80, "IN 1", RA8875_BLACK, currentButtonIndex == 1 ? RA8875_YELLOW : RA8875_GREEN, 2);

    if (currentButtonIndex != 2) {
      ch.input2 = false;
      tft.fillRect(4, 142, 110, 60, RA8875_GREEN);
    } else {
      tft.fillRect(4, 142, 110, 60, RA8875_YELLOW);
    }
    drawButtonText(tft, 10, 148, "IN 2", RA8875_BLACK, currentButtonIndex == 2 ? RA8875_YELLOW : RA8875_GREEN, 2);

    if (currentButtonIndex != 3) {
      ch.input3 = false;
      tft.fillRect(4, 210, 110, 60, RA8875_GREEN);
    } else {
      tft.fillRect(4, 210, 110, 60, RA8875_YELLOW);
    }
    drawButtonText(tft, 10, 216, "IN 3", RA8875_BLACK, currentButtonIndex == 3 ? RA8875_YELLOW : RA8875_GREEN, 2);

    if (currentButtonIndex != 4) {
      ch.input4 = false;
      tft.fillRect(4, 278, 110, 60, RA8875_GREEN);
    } else {
      tft.fillRect(4, 278, 110, 60, RA8875_YELLOW);
    }
    drawButtonText(tft, 10, 284, "IN 4", RA8875_BLACK, currentButtonIndex == 4 ? RA8875_YELLOW : RA8875_GREEN, 2);

    if (currentButtonIndex != 5) {
      ch.aux = false;
      tft.fillRect(4, 346, 110, 60, RA8875_GREEN);
    } else {
      tft.fillRect(4, 346, 110, 60, RA8875_YELLOW);
    }
    drawButtonText(tft, 22, 352, "PRI", RA8875_BLACK, currentButtonIndex == 5 ? RA8875_YELLOW : RA8875_GREEN, 2);

    if (currentButtonIndex != 6) {
      ch.pri = false;
      tft.fillRect(4, 414, 110, 60, RA8875_GREEN);
    } else {
      tft.fillRect(4, 414, 110, 60, RA8875_YELLOW);
    }
    drawButtonText(tft, 22, 420, "AUX", RA8875_BLACK, currentButtonIndex == 6 ? RA8875_YELLOW : RA8875_GREEN, 2);

  Serial.println("Current channel state:");
  Serial.print("Input1: "); Serial.println(ch.input1 ? "ON" : "OFF");
  Serial.print("Input2: "); Serial.println(ch.input2 ? "ON" : "OFF");
  Serial.print("Input3: "); Serial.println(ch.input3 ? "ON" : "OFF");
  Serial.print("Input4: "); Serial.println(ch.input4 ? "ON" : "OFF");
  Serial.print("PRI: "); Serial.println(ch.pri ? "ON" : "OFF");
  Serial.print("AUX: "); Serial.println(ch.aux ? "ON" : "OFF");
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

  // Put back into graphics mode
  tft.graphicsMode();
}

void drawAllBoxes(Adafruit_RA8875 &tft, peq_buttons &peq, channel& ch) {
  // Draw left buttons
  drawLeftButtons(tft, ch);

  // Draw Cutoff, Bandwidth, Gain, On/Off boxes
  drawCutoffBandwidthGainBoxes(tft, peq);

  // Draw High-Pass Filter boxes
  drawHighPassFilterBoxes(tft);

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

void drawHighPassFilterBoxes(Adafruit_RA8875 &tft) {
  tft.fillRect(638, 0, 162, 140, RA8875_BLACK);
  tft.fillRect(646, 6, 150, 60, RA8875_CYAN); // high pass
  tft.fillRect(646, 74, 150, 60, RA8875_CYAN); // cut
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
  printHighPassFilterText(tft);

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

void printHighPassFilterText(Adafruit_RA8875 &tft) {
  tft.textMode();
  tft.textEnlarge(1);
  tft.textSetCursor(648, 18);
  tft.textWrite("High-Pass");
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

void printInputSelectionText(Adafruit_RA8875 &tft) {
  tft.textMode();
  tft.textColor(RA8875_BLACK, RA8875_CYAN);
  tft.textSetCursor(380, 428);
  tft.textWrite("INPUT: ");
  tft.textSetCursor(380, 287);
  tft.textWrite("C.Band: ");
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

//Level Indicator Boxes
  tft.fillRect(200, 100, 43, 306, RA8875_BLACK);
  tft.fillRect(261, 100, 43, 306, RA8875_BLACK);
  tft.fillRect(322, 100, 43, 306, RA8875_BLACK);
  tft.fillRect(383, 100, 43, 306, RA8875_BLACK);
  tft.fillRect(444, 100, 43, 306, RA8875_BLACK);
  tft.fillRect(505, 100, 43, 306, RA8875_BLACK);
  tft.fillRect(566, 100, 43, 306, RA8875_BLACK);
  tft.fillRect(627, 100, 43, 306, RA8875_BLACK);
  tft.fillRect(688, 100, 43, 306, RA8875_BLACK);
  tft.fillRect(749, 100, 43, 306, RA8875_BLACK);

  tft.fillRect(206, 106, 31, 294, RA8875_WHITE);
  tft.fillRect(267, 106, 31, 294, RA8875_WHITE);
  tft.fillRect(328, 106, 31, 294, RA8875_WHITE);
  tft.fillRect(389, 106, 31, 294, RA8875_WHITE);
  tft.fillRect(450, 106, 31, 294, RA8875_WHITE);
  tft.fillRect(511, 106, 31, 294, RA8875_WHITE);
  tft.fillRect(572, 106, 31, 294, RA8875_WHITE);
  tft.fillRect(633, 106, 31, 294, RA8875_WHITE);
  tft.fillRect(694, 106, 31, 294, RA8875_WHITE);
  tft.fillRect(755, 106, 31, 294, RA8875_WHITE);

  
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

 
void level_indication(float level[], int scaled_start[], int scaled_extension[], Adafruit_RA8875 tft) {


for (size_t i = 0; i < 10; i++){
  scaled_start[i] = round(106 + 294*(1 - level[i]));
  scaled_extension[i] = round(294*level[i]);
}

  //Serial.print(", L: ");
  //Serial.print(level[0]);
  
  //Serial.print(", SL: ");
  //Serial.println(scaled_start[0]);

  delay(50);

  if ((level[0] < 0.02)) {
    tft.fillRect(206, 106, 31, 294, RA8875_WHITE);
  }
  else if ((level[0] > 0.02) && (level[0] < 0.91)){
    tft.fillRect(206, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(206, scaled_start[0], 31, scaled_extension[0], RA8875_BLUE);
  }
  else if ((level[0] > 0.90)){
    tft.fillRect(206, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(206, scaled_start[0], 31, scaled_extension[0], RA8875_RED);
  }
  
  if ((level[1] < 0.02)) {
    tft.fillRect(267, 106, 31, 294, RA8875_WHITE);
  }
  else if ((level[1] > 0.02) && (level[1] < 0.91)){
    tft.fillRect(267, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(267, scaled_start[1], 31, scaled_extension[1], RA8875_BLUE);
  }
  else if ((level[1] > 0.90)){
    tft.fillRect(267, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(267, scaled_start[1], 31, scaled_extension[1], RA8875_RED);
  }

  if ((level[2] < 0.02)) {
    tft.fillRect(328, 106, 31, 294, RA8875_WHITE);
  }
  else if ((level[2] > 0.02) && (level[2] < 0.91)){
    tft.fillRect(328, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(328, scaled_start[2], 31, scaled_extension[2], RA8875_BLUE);
  }
  else if ((level[2] > 0.90)){
    tft.fillRect(328, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(328, scaled_start[2], 31, scaled_extension[2], RA8875_RED);
  }

  if ((level[3] < 0.02)) {
    tft.fillRect(389, 106, 31, 294, RA8875_WHITE);
  }
  else if ((level[3] > 0.02) && (level[3] < 0.91)){
    tft.fillRect(389, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(389, scaled_start[3], 31, scaled_extension[3], RA8875_BLUE);
  }
  else if ((level[3] > 0.90)){
    tft.fillRect(389, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(389, scaled_start[3], 31, scaled_extension[3], RA8875_RED);
  }

  if ((level[4] < 0.02)) {
    tft.fillRect(450, 106, 31, 294, RA8875_WHITE);
  }
  else if ((level[4] > 0.02) && (level[4] < 0.91)){
    tft.fillRect(450, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(450, scaled_start[4], 31, scaled_extension[4], RA8875_MAGENTA);
  }
  else if ((level[4] > 0.90)){
    tft.fillRect(450, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(450, scaled_start[4], 31, scaled_extension[4], RA8875_RED);
  }

  if ((level[5] < 0.02)) {
    tft.fillRect(511, 106, 31, 294, RA8875_WHITE);
  }
  else if ((level[5] > 0.02) && (level[5] < 0.91)){
    tft.fillRect(511, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(511, scaled_start[5], 31, scaled_extension[5], RA8875_MAGENTA);
  }
  else if ((level[5] > 0.90)){
    tft.fillRect(511, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(511, scaled_start[5], 31, scaled_extension[5], RA8875_RED);
  }

  if ((level[6] < 0.02)) {
    tft.fillRect(572, 106, 31, 294, RA8875_WHITE);
  }
  else if ((level[6] > 0.02) && (level[6] < 0.91)){
    tft.fillRect(572, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(572, scaled_start[6], 31, scaled_extension[6], RA8875_MAGENTA);
  }
  else if ((level[6] > 0.90)){
    tft.fillRect(572, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(572, scaled_start[6], 31, scaled_extension[6], RA8875_RED);
  }

  if ((level[7] < 0.02)) {
    tft.fillRect(633, 106, 31, 294, RA8875_WHITE);
  }
  else if ((level[7] > 0.02) && (level[7] < 0.91)){
    tft.fillRect(633, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(633, scaled_start[7], 31, scaled_extension[7], RA8875_MAGENTA);
  }
  else if ((level[7] > 0.90)){
    tft.fillRect(633, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(633, scaled_start[7], 31, scaled_extension[7], RA8875_RED);
  }
 
  if ((level[8] < 0.02)) {
    tft.fillRect(694, 106, 31, 294, RA8875_WHITE);
  }
  else if ((level[8] > 0.02) && (level[8] < 0.91)){
    tft.fillRect(694, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(694, scaled_start[8], 31, scaled_extension[8], RA8875_BLUE);
  }
  else if ((level[8] > 0.90)){
    tft.fillRect(694, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(694, scaled_start[8], 31, scaled_extension[8], RA8875_RED);
  }

  if ((level[9] < 0.02)) {
    tft.fillRect(755, 106, 31, 294, RA8875_WHITE);
  }
  else if ((level[9] > 0.02) && (level[9] < 0.91)){
    tft.fillRect(755, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(755, scaled_start[9], 31, scaled_extension[9], RA8875_MAGENTA);
  }
  else if ((level[9] > 0.90)){
    tft.fillRect(755, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(755, scaled_start[9], 31, scaled_extension[9], RA8875_RED);
  }
  

}
