#include "capstone_functions.h"

#define RA8875_INT     3
#define RA8875_CS      10
#define RA8875_RESET   9
Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
bool push = false;
bool engage = false;
unsigned long lastButtonPress = 0;
Slider hpf_slide{720,720,160,460,-1,-1,0};
channel ch;

peq_buttons peq;
peq_buttons peq1;
peq_buttons peq2;
peq_buttons peq3;
peq_buttons peq4;
peq_buttons peq5;
peq_buttons peq6;

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
tsbutton HPF{646,800,6,66,0,17};
tsbutton HPFSLIDE{720,720,160,460,0,18};


//Level Indication
float level[10]           = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int scaled_start[10]      = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int scaled_extension[10]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


//Audio Signal Connections
AudioInputI2SQuad        i2s_quad;       //xy=454,293
AudioAmplifier           amp7; //xy=654,392
AudioAmplifier           amp8; //xy=654,420
AudioAmplifier           amp5; //xy=655,335
AudioAmplifier           amp6; //xy=655,363
AudioAmplifier           amp3; //xy=657,262
AudioAmplifier           amp4; //xy=657,290
AudioAmplifier           amp1;           //xy=658,205
AudioAmplifier           amp2; //xy=658,233
AudioAnalyzePeak         peak8; //xy=840,513
AudioAnalyzePeak         peak7; //xy=841,484
AudioAnalyzePeak         peak6; //xy=842,456
AudioMixer4              mixer2; //xy=843,336
AudioAnalyzePeak         peak5; //xy=843,427
AudioMixer4              mixer1;         //xy=844,267
AudioAnalyzePeak         peak4; //xy=846,194
AudioAnalyzePeak         peak3; //xy=847,165
AudioAnalyzePeak         peak2; //xy=848,137
AudioAnalyzePeak         peak1;          //xy=849,108
AudioOutputI2SQuad       is_quad;        //xy=1057,309
AudioAnalyzePeak         peak10; //xy=1058,438
AudioAnalyzePeak         peak9; //xy=1059,409
AudioConnection          patchCord1(i2s_quad, 0, amp1, 0);
AudioConnection          patchCord2(i2s_quad, 0, amp5, 0);
AudioConnection          patchCord3(i2s_quad, 1, amp2, 0);
AudioConnection          patchCord4(i2s_quad, 1, amp6, 0);
AudioConnection          patchCord5(i2s_quad, 2, amp3, 0);
AudioConnection          patchCord6(i2s_quad, 2, amp7, 0);
AudioConnection          patchCord7(i2s_quad, 3, amp4, 0);
AudioConnection          patchCord8(i2s_quad, 3, amp8, 0);
AudioConnection          patchCord9(amp7, peak7);
AudioConnection          patchCord10(amp7, 0, mixer2, 2);
AudioConnection          patchCord11(amp8, peak8);
AudioConnection          patchCord12(amp8, 0, mixer2, 3);
AudioConnection          patchCord13(amp5, peak5);
AudioConnection          patchCord14(amp5, 0, mixer2, 0);
AudioConnection          patchCord15(amp6, peak6);
AudioConnection          patchCord16(amp6, 0, mixer2, 1);
AudioConnection          patchCord17(amp3, peak3);
AudioConnection          patchCord18(amp3, 0, mixer1, 2);
AudioConnection          patchCord19(amp4, peak4);
AudioConnection          patchCord20(amp4, 0, mixer1, 3);
AudioConnection          patchCord21(amp1, peak1);
AudioConnection          patchCord22(amp1, 0, mixer1, 0);
AudioConnection          patchCord23(amp2, peak2);
AudioConnection          patchCord24(amp2, 0, mixer1, 1);
AudioConnection          patchCord25(mixer2, 0, is_quad, 2);
AudioConnection          patchCord26(mixer2, 0, is_quad, 3);
AudioConnection          patchCord27(mixer2, peak10);
AudioConnection          patchCord28(mixer1, 0, is_quad, 0);
AudioConnection          patchCord29(mixer1, 0, is_quad, 1);
AudioConnection          patchCord30(mixer1, peak9);
AudioControlSGTL5000     sgtl5000_1;     //xy=571,99
AudioControlSGTL5000     sgtl5000_2;     //xy=699,98




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




//Touchscreen Setup
  Serial.begin(9600);
  Serial.println("Hello, RA8875!");
  initializeChannel(ch);
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
}

void loop() {
  int x, y;
  unsigned long currentMillis = millis();
  readTouchInput(x, y, tft);
  if (push == false){
      level[0] = peak1.read();
      level[1] = peak2.read();
      level[2] = peak3.read();
      level[3] = peak4.read();
      level[4] = peak5.read();
      level[5] = peak6.read();
      level[6] = peak7.read();
      level[7] = peak8.read();
      level[8] = peak9.read();
      level[9] = peak10.read();
      level_indication(level, scaled_start, scaled_extension, tft);
   }

    if (tft.touched()) {
    getMappedCoordinates(x, y, tft);
   //printCoordinates(x, y);

    
    if (isButtonPressed(x, y, PEQSwitch)) {
      handleMenuPress(ch, ch.peqs[ch.active -1], push, lastButtonPress, currentMillis, debounceTime, tft);
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
      handlePeqButtonPress(ch, 1, lastButtonPress, currentMillis, debounceTime, tft);
    }else if(isButtonPressed(x, y, b1)&& push == true){
      handlePeqButtonPress(ch, 2, lastButtonPress, currentMillis, debounceTime, tft);
    }else if(isButtonPressed(x, y, hp)&& push == true){
      handlePeqButtonPress(ch, 3, lastButtonPress, currentMillis, debounceTime, tft);
    }else if(isButtonPressed(x, y, b2)&& push == true){
      handlePeqButtonPress(ch, 4, lastButtonPress, currentMillis, debounceTime, tft);
    }else if(isButtonPressed(x, y, b3)&& push == true){
      handlePeqButtonPress(ch, 5, lastButtonPress, currentMillis, debounceTime, tft);
    }else if(isButtonPressed(x, y, hs)&& push == true){
      handlePeqButtonPress(ch, 6, lastButtonPress, currentMillis, debounceTime, tft);
    }else if(isButtonPressed(x, y, b4)&& push == true){
      handlePeqButtonPress(ch, 7, lastButtonPress, currentMillis, debounceTime, tft);
    }else if(isButtonPressed(x, y, lp)&& push == true){
      handlePeqButtonPress(ch, 8, lastButtonPress, currentMillis, debounceTime, tft);
    }else if(isButtonPressed(x, y, OnOff)&& push == true){
      handlePeqButtonPress(ch, 9, lastButtonPress, currentMillis, debounceTime, tft);
    }else if(isButtonPressed(x, y, HPF)&& push == true){
      handlePeqButtonPress(ch, 10, lastButtonPress, currentMillis, debounceTime, tft);
    }else if (isButtonPressed(x, y, HPFSLIDE)&& push == true ) {
      handleHPFSlider(tft, y, ch.peqs[ch.active -1].hpfSlider, ch.peqs[ch.active -1]);
    }else {
      lastButtonPress = currentMillis;
    }
  }
  
}
