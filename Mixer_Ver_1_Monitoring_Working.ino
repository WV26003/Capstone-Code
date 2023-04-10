# include <capstone_functions.h>


#define ENCODER_SWITCH        24
#define ENCODER_NEOPIX        6

#define ENCODER_BASE_ADDR          0x36  // I2C address, starts with 0x36

// create 4 encoders
Adafruit_seesaw encoders[4];
// create 4 encoder pixels
seesaw_NeoPixel encoder_pixels[4] = {
  seesaw_NeoPixel(1, ENCODER_NEOPIX, NEO_GRB + NEO_KHZ800),
  seesaw_NeoPixel(1, ENCODER_NEOPIX, NEO_GRB + NEO_KHZ800),
  seesaw_NeoPixel(1, ENCODER_NEOPIX, NEO_GRB + NEO_KHZ800),
  seesaw_NeoPixel(1, ENCODER_NEOPIX, NEO_GRB + NEO_KHZ800)};


//Stores encoder position
int32_t encoder_positions[] = {0, 0, 0, 0};
bool found_encoders[]       = {false, false, false, false};

uint32_t lastDebounceTime[4];
bool last[4] = {false, false, false, false};
bool cur[4] = {false, false, false, false};
bool led[4] = {false, false, false, false};

bool mute_status[4] = {false, false, false, false};


//Setting Addresses for 6 linear faders
#define  Linear_Fader_ADDR_1 0x30
#define  Linear_Fader_ADDR_2 0x31
#define  Linear_Fader_ADDR_3 0x32
#define  Linear_Fader_ADDR_4 0x33
#define  Linear_Fader_ADDR_5 0x34
#define  Linear_Fader_ADDR_6 0x35

#define  L_Fader_ANALOGIN   18


Adafruit_seesaw L_Fader_1;
Adafruit_seesaw L_Fader_2;
Adafruit_seesaw L_Fader_3;
Adafruit_seesaw L_Fader_4;
Adafruit_seesaw L_Fader_5;
Adafruit_seesaw L_Fader_6;


//Quicc Button Setup
QwiicButton Qbutton[8]; //0x60 //0x61 //0x62 //0x63 //0x64 //0x65 //0x66 //0x67

uint8_t brightness = 100;     //The brightness to set the LED to when the button is pushed
                              //Can be any value between 0 (off) and 255 (max)
bool QLEDState[8] = {false, false, false, false, false, false, false, false};     

//Audio Signal Flow
AudioInputI2SQuad        i2s_quad;       //xy=354,446
AudioAmplifier           amp6;           //xy=549,515
AudioAmplifier           amp7;           //xy=549,546
AudioAmplifier           amp8;           //xy=549,576
AudioAmplifier           amp5;           //xy=550,485
AudioAmplifier           amp1;           //xy=551,332
AudioAmplifier           amp2;           //xy=551,362
AudioAmplifier           amp3;           //xy=551,392
AudioAmplifier           amp4;           //xy=551,422
AudioMixer4              mixer3;         //xy=700,196
AudioMixer4              mixer1;         //xy=701,375
AudioMixer4              mixer2;         //xy=701,444
AudioAmplifier           amp10;          //xy=835,423
AudioAmplifier           amp9;           //xy=836,392
AudioMixer4              mixer4;         //xy=865,229
AudioAmplifier           amp12;          //xy=964,423
AudioAmplifier           amp11;          //xy=965,392
AudioOutputI2SQuad       is_quad;        //xy=1101,405
AudioConnection          patchCord1(i2s_quad, 0, amp1, 0);
AudioConnection          patchCord2(i2s_quad, 0, amp5, 0);
AudioConnection          patchCord3(i2s_quad, 0, mixer3, 0);
AudioConnection          patchCord4(i2s_quad, 1, amp2, 0);
AudioConnection          patchCord5(i2s_quad, 1, amp6, 0);
AudioConnection          patchCord6(i2s_quad, 1, mixer3, 1);
AudioConnection          patchCord7(i2s_quad, 2, amp3, 0);
AudioConnection          patchCord8(i2s_quad, 2, amp7, 0);
AudioConnection          patchCord9(i2s_quad, 2, mixer3, 2);
AudioConnection          patchCord10(i2s_quad, 3, amp4, 0);
AudioConnection          patchCord11(i2s_quad, 3, amp8, 0);
AudioConnection          patchCord12(i2s_quad, 3, mixer3, 3);
AudioConnection          patchCord13(amp6, 0, mixer2, 1);
AudioConnection          patchCord14(amp7, 0, mixer2, 2);
AudioConnection          patchCord15(amp8, 0, mixer2, 3);
AudioConnection          patchCord16(amp5, 0, mixer2, 0);
AudioConnection          patchCord17(amp1, 0, mixer1, 0);
AudioConnection          patchCord18(amp2, 0, mixer1, 1);
AudioConnection          patchCord19(amp3, 0, mixer1, 2);
AudioConnection          patchCord20(amp4, 0, mixer1, 3);
AudioConnection          patchCord21(mixer3, 0, mixer4, 0);
AudioConnection          patchCord22(mixer1, amp9);
AudioConnection          patchCord23(mixer1, 0, mixer4, 1);
AudioConnection          patchCord24(mixer2, amp10);
AudioConnection          patchCord25(mixer2, 0, mixer4, 2);
AudioConnection          patchCord26(amp10, amp12);
AudioConnection          patchCord27(amp9, amp11);
AudioConnection          patchCord28(mixer4, 0, is_quad, 0);
AudioConnection          patchCord29(mixer4, 0, is_quad, 1);
AudioConnection          patchCord30(amp12, 0, is_quad, 3);
AudioConnection          patchCord31(amp11, 0, is_quad, 2);
AudioControlSGTL5000     sgtl5000_1;     //xy=571,99
AudioControlSGTL5000     sgtl5000_2;     //xy=726,100


void setup() {
  AudioMemory(30);

  // Enable the first audio shield, select input, and enable output
  sgtl5000_1.setAddress(LOW);
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_1.volume(0.5);
  sgtl5000_1.lineOutLevel(29);

  // Enable the second audio shield, select input, and enable output
  sgtl5000_2.setAddress(HIGH);
  sgtl5000_2.enable();
  sgtl5000_2.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_2.volume(0.5);
  sgtl5000_2.lineOutLevel(29);


//Engaging Encoders
Serial.begin(115200);
  //while (!Serial) delay(10);

  Serial.println("Looking for seesaws!");

  for (uint8_t enc=0; enc<sizeof(found_encoders); enc++) {
    // See if we can find encoders on this address
    if (! encoders[enc].begin(ENCODER_BASE_ADDR + enc) ||
        ! encoder_pixels[enc].begin(ENCODER_BASE_ADDR + enc)) {
      Serial.print("Couldn't find encoder #");
      Serial.println(enc);
    } else {
      Serial.print("Found encoder + pixel #");
      Serial.println(enc);

  // use a pin for the built in encoder switch
  encoders[enc].pinMode(ENCODER_SWITCH, INPUT_PULLUP);


  // set not so bright!
  encoder_pixels[enc].setBrightness(30);
  encoder_pixels[enc].show();

  found_encoders[enc] = true;
    }
  }
  Serial.println("Encoders started");

//Engaging Linear Faders
  if (!L_Fader_1.begin(Linear_Fader_ADDR_1)){
    Serial.println(F("L_Fader_1 not found!"));
    while(1) delay(10);
  }
  else {
    Serial.println(F("L_Fader_1 found!"));
  }
  
  if (!L_Fader_2.begin(Linear_Fader_ADDR_2)){
    Serial.println(F("L_Fader_2 not found!"));
    while(1) delay(10);
  }
  else {
    Serial.println(F("L_Fader_2 found!"));
  }
  
  if (!L_Fader_3.begin(Linear_Fader_ADDR_3)){
    Serial.println(F("L_Fader_3 not found!"));
    while(1) delay(10);
  }
  else {
    Serial.println(F("L_Fader_3 found!"));
  }
  
  if (!L_Fader_4.begin(Linear_Fader_ADDR_4)){
    Serial.println(F("L_Fader_4 not found!"));
    while(1) delay(10);
  }
    else {
    Serial.println(F("L_Fader_4 found!"));
  }

  if (!L_Fader_5.begin(Linear_Fader_ADDR_5)){
    Serial.println(F("L_Fader_5 not found!"));
    while(1) delay(10);
  }
  else {
    Serial.println(F("L_Fader_5 found!"));
  }
  
  if (!L_Fader_6.begin(Linear_Fader_ADDR_6)){
    Serial.println(F("L_Fader_6 not found!"));
    while(1) delay(10);
  }
  else {
    Serial.println(F("L_Fader_6 found!"));
  }


//Engaging Buttons
  Wire2.begin();
  if (Qbutton[0].begin(0x60, Wire2) == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1);
  }
  Serial.println("Qbutton1 acknowledged.");
  if (Qbutton[1].begin(0x61, Wire2) == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1);
  }
  
  Serial.println("Qbutton2 acknowledged.");
  if (Qbutton[2].begin(0x62, Wire2) == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1);
  }
  Serial.println("Qbutton3 acknowledged.");
 
  if (Qbutton[3].begin(0x63, Wire2) == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1);
  }

  Serial.println("Qbutton4 acknowledged.");
  if (Qbutton[4].begin(0x64, Wire2) == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1);
  }
  Serial.println("Qbutton5 acknowledged.");
  if (Qbutton[5].begin(0x65, Wire2) == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1);
  }
  Serial.println("Qbutton6 acknowledged.");
  if (Qbutton[6].begin(0x66, Wire2) == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1);
  }
  Serial.println("Qbutton7 acknowledged.");
  if (Qbutton[7].begin(0x67, Wire2) == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1);
  }
    Serial.println("Qbutton8 acknowledged.");

    
//Start LEDs off
  Qbutton[0].LEDoff();
  Qbutton[1].LEDoff();
  Qbutton[2].LEDoff();
  Qbutton[3].LEDoff();
  Qbutton[4].LEDoff();
  Qbutton[5].LEDoff();
  Qbutton[6].LEDoff();
  Qbutton[7].LEDoff();


  
}



int memory_test = 0;



void loop() {

//Linear Fader Control of Primary Mix Inputs
  linear_fader(L_Fader_1, amp1, L_Fader_ANALOGIN);
  linear_fader(L_Fader_2, amp2, L_Fader_ANALOGIN);
  linear_fader(L_Fader_3, amp3, L_Fader_ANALOGIN);
  linear_fader(L_Fader_4, amp4, L_Fader_ANALOGIN);

//Pre-amp gain of the four inputs defined by first four encoders
  encoder_fader(encoders[0], amp5, encoder_positions[0]);
  encoder_fader(encoders[1], amp6, encoder_positions[1]);
  encoder_fader(encoders[2], amp7, encoder_positions[2]);
  encoder_fader(encoders[3], amp8, encoder_positions[3]);

//Linear Fader Control of Output Mixes (Primary and Auxiliary)
  linear_fader(L_Fader_5, amp9, L_Fader_ANALOGIN);
  linear_fader(L_Fader_6, amp10, L_Fader_ANALOGIN);
  //Serial.println("");

//Toggling Qwicc Buttons
  button_toggle(Qbutton[0], QLEDState[0]);
  button_toggle(Qbutton[1], QLEDState[1]);
  button_toggle(Qbutton[2], QLEDState[2]);
  button_toggle(Qbutton[3], QLEDState[3]);
  button_toggle(Qbutton[4], QLEDState[4]);
  button_toggle(Qbutton[5], QLEDState[5]);
  button_toggle(Qbutton[6], QLEDState[6]);
  button_toggle(Qbutton[7], QLEDState[7]);

//Muting Output Mixes Via Qwicc Buttons

  output_mix_mute_control(amp11, QLEDState[6]);
  output_mix_mute_control(amp12, QLEDState[7]);

//Toggling Encoder Buttons
  encoder_button(encoders[0], encoder_pixels[0], ENCODER_SWITCH, ENCODER_NEOPIX, led[0], lastDebounceTime[0], last[0]);
  encoder_button(encoders[1], encoder_pixels[1], ENCODER_SWITCH, ENCODER_NEOPIX, led[1], lastDebounceTime[1], last[1]);
  encoder_button(encoders[2], encoder_pixels[2], ENCODER_SWITCH, ENCODER_NEOPIX, led[2], lastDebounceTime[2], last[2]);
  encoder_button(encoders[3], encoder_pixels[3], ENCODER_SWITCH, ENCODER_NEOPIX, led[3], lastDebounceTime[3], last[3]);

//Muting Inputs with Encoder Buttons  
 muting_status_array(encoder_pixels, mute_status);
 input_muting(mixer1, mixer2, encoder_pixels, mute_status);

//Monitoring
  monitoring(mixer3, mixer4, QLEDState);

//Memory Usage Monitoring
memory_test = AudioMemoryUsageMax();
Serial.print("           ");
Serial.print(memory_test);
Serial.println("           ");

}
