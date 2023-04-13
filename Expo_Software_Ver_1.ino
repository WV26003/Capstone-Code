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


//Touchscreen Setup
#define RA8875_INT 3
#define RA8875_CS 10
#define RA8875_RESET 9

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
bool push = false;
unsigned long lastButtonPress = 0;

struct sliderBox {
  int leftBound;
  int rightBound;
  int topBound;
  int botBound;
};
channel ch;
peq_buttons peq;

tsbutton PEQSwitch{ 4, 114, 6, 66,0,1};
tsbutton Input1{ 4, 114, 74, 134,0,2};
tsbutton Input2{ 4, 114, 142, 202,0,3};
tsbutton Input3{ 4, 114, 210, 276,0,4};
tsbutton Input4{ 4, 114, 278, 338,0,5};
tsbutton PRI{ 4, 114, 346, 406,0,6};
tsbutton AUX{ 4, 114, 414, 474,0,7};
tsbutton ls{280,366,278,338,0,8};
tsbutton b1{280,366,346,406,0,9};
tsbutton hp{280,366,414,474,0,10};
tsbutton b2{374,455,346,406,0,11};
tsbutton b3{463,544,346,406,0,12};
tsbutton hs{552,638,278,338,0,13};
tsbutton b4{552,638,346,406,0,14};
tsbutton lp{552,638,414,474,0,15};
tsbutton OnOff{ 122, 272, 210, 270,0,16};


//Level Indication
float level[10]           = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int scaled_start[10]      = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int scaled_extension[10]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

float linear_faders[6]    = {0, 0, 0, 0, 0, 0};
float rotary_faders[4]    = {0, 0, 0, 0};

     

//Audio Signal Flow
AudioInputI2SQuad        i2s_quad;       //xy=58.5,485
AudioAmplifier           amp1;           //xy=185.5,328
AudioAmplifier           amp2;           //xy=201.5,358
AudioAmplifier           amp3;           //xy=217.5,386
AudioAmplifier           amp4;           //xy=234.5,416
AudioAnalyzePeak         peak1;          //xy=361,437
AudioAnalyzePeak         peak2;          //xy=384,467
AudioAnalyzePeak         peak3;          //xy=412,497
AudioAnalyzePeak         peak4;          //xy=440,525
AudioAmplifier           amp5;           //xy=562.5,539
AudioAmplifier           amp6;           //xy=581.5,570
AudioAmplifier           amp7;           //xy=592.5,599
AudioAmplifier           amp8;           //xy=608.5,630
AudioAnalyzePeak         peak5;          //xy=773,621
AudioAnalyzePeak         peak6;          //xy=795,650
AudioAnalyzePeak         peak7;          //xy=824,678
AudioAnalyzePeak         peak8;          //xy=853,709
AudioMixer4              mixer1;         //xy=1014.5,376
AudioMixer4              mixer2;         //xy=1121.5,448
AudioMixer4              mixer3;         //xy=1235.5,267
AudioMixer4              mixer4;         //xy=1355.5,303
AudioAmplifier           amp9;           //xy=1469.5,410
AudioAmplifier           amp10;          //xy=1485.5,440
AudioAnalyzePeak         peak9;          //xy=1646,484
AudioAnalyzePeak         peak10;         //xy=1668,513
AudioAmplifier           amp11;          //xy=1772.5,409
AudioAmplifier           amp12;          //xy=1795.5,439
AudioOutputI2SQuad       is_quad;        //xy=1955.5,414
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
AudioConnection          patchCord13(amp1, 0, mixer1, 0);
AudioConnection          patchCord14(amp1, peak1);
AudioConnection          patchCord15(amp2, 0, mixer1, 1);
AudioConnection          patchCord16(amp2, peak2);
AudioConnection          patchCord17(amp3, 0, mixer1, 2);
AudioConnection          patchCord18(amp3, peak3);
AudioConnection          patchCord19(amp4, 0, mixer1, 3);
AudioConnection          patchCord20(amp4, peak4);
AudioConnection          patchCord21(amp5, 0, mixer2, 0);
AudioConnection          patchCord22(amp5, peak5);
AudioConnection          patchCord23(amp6, 0, mixer2, 1);
AudioConnection          patchCord24(amp6, peak6);
AudioConnection          patchCord25(amp7, 0, mixer2, 2);
AudioConnection          patchCord26(amp7, peak7);
AudioConnection          patchCord27(amp8, 0, mixer2, 3);
AudioConnection          patchCord28(amp8, peak8);
AudioConnection          patchCord29(mixer1, amp9);
AudioConnection          patchCord30(mixer1, 0, mixer4, 1);
AudioConnection          patchCord31(mixer2, amp10);
AudioConnection          patchCord32(mixer2, 0, mixer4, 2);
AudioConnection          patchCord33(mixer3, 0, mixer4, 0);
AudioConnection          patchCord34(mixer4, 0, is_quad, 0);
AudioConnection          patchCord35(mixer4, 0, is_quad, 1);
AudioConnection          patchCord36(amp9, amp11);
AudioConnection          patchCord37(amp9, peak9);
AudioConnection          patchCord38(amp10, amp12);
AudioConnection          patchCord39(amp10, peak10);
AudioConnection          patchCord40(amp11, 0, is_quad, 2);
AudioConnection          patchCord41(amp12, 0, is_quad, 3);
AudioControlSGTL5000     sgtl5000_1;     //xy=275.5,138
AudioControlSGTL5000     sgtl5000_2;     //xy=430.5,139


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



//Touchscreen Setup
  if (!tft.begin(RA8875_800x480)) {
    Serial.println("RA8875 not found ... check your wires!");
    while (1);
  }

  Serial.println("Found RA8875");
  tft.displayOn(true);
  tft.GPIOX(true);
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024);
  tft.PWM1out(255);

  Serial.println("Enabled the touch screen");
  pinMode(RA8875_INT, INPUT);
  digitalWrite(RA8875_INT, HIGH);
  tft.touchEnable(true);
  tft.fillScreen(RA8875_WHITE);

  tft.fillRect(200, 200, 400, 10, RA8875_BLACK);
  tft.fillRect(200, 175, 80, 60, RA8875_GREEN);
  printMenu(tft);


  encoder_positions[0] = encoders[0].getEncoderPosition();
  encoder_positions[1] = encoders[1].getEncoderPosition();
  encoder_positions[2] = encoders[2].getEncoderPosition();
  encoder_positions[3] = encoders[3].getEncoderPosition();    
}



int memory_test = 0;



void loop() {

//Linear Fader Control of Primary Mix Inputs
  linear_faders[0] = linear_fader(L_Fader_1, amp1, L_Fader_ANALOGIN);
  linear_faders[1] = linear_fader(L_Fader_2, amp2, L_Fader_ANALOGIN);
  linear_faders[2] = linear_fader(L_Fader_3, amp3, L_Fader_ANALOGIN);
  linear_faders[3] = linear_fader(L_Fader_4, amp4, L_Fader_ANALOGIN);

//Pre-amp gain of the four inputs defined by first four encoders
  rotary_faders[0] = encoder_fader(encoders[0], amp5, encoder_positions[0]);
  rotary_faders[1] = encoder_fader(encoders[1], amp6, encoder_positions[1]);
  rotary_faders[2] = encoder_fader(encoders[2], amp7, encoder_positions[2]);
  rotary_faders[3] = encoder_fader(encoders[3], amp8, encoder_positions[3]);
Serial.println();
//Linear Fader Control of Output Mixes (Primary and Auxiliary)
  linear_faders[4] = linear_fader(L_Fader_5, amp9, L_Fader_ANALOGIN);
  linear_faders[5] = linear_fader(L_Fader_6, amp10, L_Fader_ANALOGIN);
  //Serial.println("");



//Touchscreen
  int x, y;
  unsigned long currentMillis = millis();
  readTouchInput(x, y, tft);

  if (push == false){

//Linear Fader Indication
    if (linear_faders[0] == 0) {
      level[0] = 0;
    }
    else {
      level[0] = peak1.read();
    }
    
    if (linear_faders[1] == 0) {
      level[1] = 0;
    }
    else {
      level[1] = peak2.read();
    }

    if (linear_faders[2] == 0) {
      level[2] = 0;
    }
    else {
      level[2] = peak3.read();
    }

    if (linear_faders[3] == 0) {
      level[3] = 0;
    }
    else {
      level[3] = peak4.read();
    }
    
  
    if (linear_faders[4] == 0 ||((linear_faders[0] == 0) && (linear_faders[1] == 0) && (linear_faders[2] == 0) && (linear_faders[3] == 0))) {
      level[8] = 0;
    }
    else {
      level[8] = peak9.read();
    }

//Rotary Fader Indication
    if (rotary_faders[0] <= 0) {
      level[4] = 0;
    }
    else {
      level[4] = peak5.read();
    }

    if (rotary_faders[1] <= 0) {
      level[5] = 0;
    }
    else {
      level[5] = peak6.read();
    }

    if (rotary_faders[2] <= 0) {
      level[6] = 0;
    }
    else {
      level[6] = peak7.read();
    }
    
    if (rotary_faders[3] <= 0) {
      level[7] = 0;
    }
    else {
      level[7] = peak8.read();
    }
    
    if ((linear_faders[5] == 0) || ((rotary_faders[0] == 0) && (rotary_faders[1] == 0) && (rotary_faders[2] == 0) && (rotary_faders[3] == 0))) {
      level[9] = 0;
    }
    else {
      level[9] = peak10.read();
    }
    
    level_indication(level, scaled_start, scaled_extension, tft);    
  }

  
  if (tft.touched()) {
    getMappedCoordinates(x, y, tft);
   printCoordinates(x, y);
/*
    if (isButtonPressed(x, y, PEQSwitch)) {
      handleMenuPress(ch, peq, push, lastButtonPress, currentMillis, debounceTime, tft);
    }else if(isButtonPressed(x, y, Input1)&& push == true){

        handleChannelPress(ch.input1, ch, 1, lastButtonPress, currentMillis, debounceTime, tft); 
    }else if(isButtonPressed(x, y, Input2)&& push == true){
      handleChannelPress(ch.input2, ch, 2, lastButtonPress, currentMillis, debounceTime, tft);
    }else if(isButtonPressed(x, y, Input3)&& push == true){
      handleChannelPress(ch.input3, ch, 3, lastButtonPress, currentMillis, debounceTime, tft);
    }else if(isButtonPressed(x, y, Input4)&& push == true){
      handleChannelPress(ch.input4, ch, 4, lastButtonPress, currentMillis, debounceTime, tft);
    }else if(isButtonPressed(x, y, PRI)&& push == true){
      handleChannelPress(ch.pri, ch, 5, lastButtonPress, currentMillis, debounceTime, tft);
    }else if(isButtonPressed(x, y, AUX)&& push == true){
      handleChannelPress(ch.aux, ch, 6, lastButtonPress, currentMillis, debounceTime, tft);
    }else if(isButtonPressed(x, y, ls)&& push == true){
      Serial.println("GOT HERE");
      handlePeqButtonPress(peq.ls, peq, 1, lastButtonPress, currentMillis, debounceTime, tft);
    }else if(isButtonPressed(x, y, b1)&& push == true){
      handlePeqButtonPress(peq.b1, peq, 2, lastButtonPress, currentMillis, debounceTime, tft);
    }else if(isButtonPressed(x, y, hp)&& push == true){
      handlePeqButtonPress(peq.hp, peq, 3, lastButtonPress, currentMillis, debounceTime, tft);
    }else if(isButtonPressed(x, y, b2)&& push == true){
      handlePeqButtonPress(peq.b2, peq, 4, lastButtonPress, currentMillis, debounceTime, tft);
    }else if(isButtonPressed(x, y, b3)&& push == true){
      handlePeqButtonPress(peq.b3, peq, 5, lastButtonPress, currentMillis, debounceTime, tft);
    }else if(isButtonPressed(x, y, hs)&& push == true){
      handlePeqButtonPress(peq.hs, peq, 6, lastButtonPress, currentMillis, debounceTime, tft);
    }else if(isButtonPressed(x, y, b4)&& push == true){
      handlePeqButtonPress(peq.b4, peq, 7, lastButtonPress, currentMillis, debounceTime, tft);
    }else if(isButtonPressed(x, y, lp)&& push == true){
      handlePeqButtonPress(peq.hp, peq, 8, lastButtonPress, currentMillis, debounceTime, tft);
    }else if(isButtonPressed(x, y, OnOff)&& push == true){
      handlePeqButtonPress(peq.status, peq, 9, lastButtonPress, currentMillis, debounceTime, tft);
    }else {
      lastButtonPress = currentMillis;
    }
*/
  }










  

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

/*
//Memory Usage Monitoring
memory_test = AudioMemoryUsageMax();
Serial.print("           ");
Serial.print(memory_test);
Serial.println("           ");
*/
}
