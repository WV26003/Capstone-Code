#include <capstone_functions.h>



// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=74,462
AudioMixer4              mixer1;         //xy=439,466
AudioAmplifier           amp1;           //xy=676,463
AudioOutputI2S           i2s2;           //xy=941,467
AudioConnection          patchCord1(i2s1, 0, mixer1, 0);
AudioConnection          patchCord2(i2s1, 1, mixer1, 1);
AudioConnection          patchCord3(mixer1, amp1);
AudioConnection          patchCord4(amp1, 0, i2s2, 0);
AudioConnection          patchCord5(amp1, 0, i2s2, 1);
AudioControlSGTL5000     audioShield;     //xy=451,192
// GUItool: end automatically generated code

//Fader Stuff
#define  DEFAULT_I2C_ADDR 0x30
#define  ANALOGIN   18

Adafruit_seesaw seesaw;

QwiicButton button1;
QwiicButton button2;
uint8_t brightness = 100;   //The brightness to set the LED to when the button is pushed
bool ledState1 = false;      //Can be any value between 0 (off) and 255 (max)
bool ledState2 = false;
void button_toggle(QwiicButton &button, bool &ledState);

//Audio Channels
int channel_1 = 0;
int channel_2 = 1;
int channel_3 = 2;
int channel_4 = 3;




void setup() {
  
  AudioMemory(6);
  audioShield.enable();
  audioShield.inputSelect(AUDIO_INPUT_MIC);
  audioShield.volume(0.5);

  Serial.begin(115200);
  
  Wire1.begin();
  if (button1.begin(0x5B, Wire1) == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1);
  }
  Serial.println("Button1 acknowledged.");
  if (button2.begin(0x6F, Wire1) == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1);
  }
  
}

void loop() {
  linear_fader(seesaw, amp1, ANALOGIN);
  button_toggle(button1, ledState1);
  button_toggle(button2, ledState2);

//Channel 1 muting/unmuting based on LED status
  if (button1.LEDon() == true) {
     mute(mixer1, channel_1);
  }
  else {
    unmute(mixer1, channel_1);
  }

//Channel 2 muting/unmuting based on LED status  
  if (button1.LEDon() == true) {
    (mixer1, channel_2);
  }
  else {
    unmute(mixer1, channel_2);
  }

}
