#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"

//#define RA8875_INT 3
//#define RA8875_CS 10
//#define RA8875_RESET 9

#define RA8875_INT 30
#define RA8875_CS      10
#define RA8875_RESET 31

const int TS_LEFT = 40;
const int TS_RT = 980;
const int TS_TOP = 120;
const int TS_BOT = 920;



Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
uint16_t x, y;


//Setting a button
int push = 0;

//setting screen bounds or something idk
//const int TS_LEFT = 60, TS_RT = 980, TS_TOP = 80, TS_BOT = 920;

uint16_t scaled_x;
uint16_t scaled_y;


// GUItool: begin automatically generated code
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
// GUItool: end automatically generated code


//Level Indication
float level[10]           = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int scaled_start[10]      = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int scaled_extension[10]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


void level_indicators(float level[], int scaled_start[], int scaled_extension[]);

void setup() {

//Audio Setup
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
  Serial.println("RA8875 start");

  if (!tft.begin(RA8875_800x480)) {
  Serial.println("RA8875 Not Found!");
  while (1);
  }
  
  Serial.println("Found RA8875");
//Enabale Display and backlight
  tft.displayOn(true);
  tft.GPIOX(true);                              // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255);

//Enable Touchscreen
  Serial.println("Enabled the touch screen");
  pinMode(RA8875_INT, INPUT);
  digitalWrite(RA8875_INT, HIGH);
  tft.touchEnable(true);

  

  tft.fillScreen(RA8875_WHITE);

  //tft.fillRect(200, 200, 400, 10, RA8875_BLACK);
  //tft.drawRect(200, 200, 400, 10, RA8875_GREEN);
  //tft.fillRect(350, 200, 100, 80, RA8875_BLUE);

  tft.drawRect(370, 110, 60, 260, RA8875_BLACK);
  //tft.fillRect(380, 120, 40, 240, RA8875_RED);



  //Print Menu
    
  tft.fillScreen(RA8875_WHITE);

  //Sets up blackspace
  tft.fillRect(0, 0, 120, 480, RA8875_BLACK);
  tft.fillRect(0, 0, 800, 74, RA8875_BLACK);
  tft.fillRect(120, 74, 73, 270, RA8875_BLACK);

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


//Audio Settings For Test
amp1.gain(1);
amp2.gain(1);
amp3.gain(1);
amp4.gain(1);

amp5.gain(1);
amp6.gain(1);
amp7.gain(1);
amp8.gain(1);

mixer1.gain(0, 0.25);
mixer1.gain(1, 0.25);
mixer1.gain(2, 0.25);
mixer1.gain(3, 0.25);

mixer2.gain(0, 0.25);
mixer2.gain(1, 0.25);
mixer2.gain(2, 0.25);
mixer2.gain(3, 0.25);

}




void loop() {
  
  float xScale = 1024.0F/tft.width();
  float yScale = 1024.0F/tft.height();


/* Clear any previous interrupts to avoid false buffered reads */
  tft.touchRead(&x, &y);
  delay(50);


/* Wait around for a new touch event (INT pin goes low) */
//  while (digitalRead(RA8875_INT))
//  {
//  }

/* Make sure this is really a touch event */
  if (tft.touched()) {
    tft.touchRead(&x, &y);
//    scaled_x = (x/xScale);
//    scaled_y = (y/yScale);

    int16_t scaled_x = map(x, TS_LEFT, TS_RT, 0, tft.width());
    int16_t scaled_y = map(y, TS_TOP, TS_BOT, 0, tft.height());
/*
    Serial.print("X = ");
    Serial.print(scaled_x);
    Serial.print(", y = ");
    Serial.print(scaled_y);
*/
  }

level_indicators(level, scaled_start, scaled_extension);




//tft.fillRect(206, 106, 31, 294, RA8875_WHITE);
/*
  tft.fillRect(380, 120, 40, 240, RA8875_WHITE);
  tft.fillRect(380, scaled_start, 40, scaled_extension, RA8875_RED);

  tft.fillRect(500, 120, 40, 240, RA8875_RED);
*/
}

void level_indicators(float level[], int scaled_start[], int scaled_extension[]){

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

for (size_t i = 0; i < 10; i++){
  scaled_start[i] = round(106 + 294*(1 - level[i]));
  scaled_extension[i] = round(294*level[i]);
}

  Serial.print(", L: ");
  Serial.print(level[1]);
  
  Serial.print(", SL: ");
  Serial.println(scaled_start[0]);

  delay(50);

  if ((level[0] < 0.02)) {
    tft.fillRect(206, 106, 31, 294, RA8875_WHITE);
  }
  else {
    tft.fillRect(206, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(206, scaled_start[0], 31, scaled_extension[0], RA8875_RED);
  }

  if ((level[1] < 0.02)) {
    tft.fillRect(267, 106, 31, 294, RA8875_WHITE);
  }
  else {
    tft.fillRect(267, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(267, scaled_start[1], 31, scaled_extension[1], RA8875_RED);
  }

  if ((level[2] < 0.02)) {
    tft.fillRect(328, 106, 31, 294, RA8875_WHITE);
  }
  else {
    tft.fillRect(328, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(328, scaled_start[2], 31, scaled_extension[2], RA8875_RED);
  }

  if ((level[3] < 0.02)) {
    tft.fillRect(389, 106, 31, 294, RA8875_WHITE);
  }
  else {
    tft.fillRect(389, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(389, scaled_start[3], 31, scaled_extension[3], RA8875_RED);
  }

  if ((level[4] < 0.02)) {
    tft.fillRect(450, 106, 31, 294, RA8875_WHITE);
  }
  else {
    tft.fillRect(450, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(450, scaled_start[4], 31, scaled_extension[4], RA8875_RED);
  }

  if ((level[5] < 0.02)) {
    tft.fillRect(511, 106, 31, 294, RA8875_WHITE);
  }
  else {
    tft.fillRect(511, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(511, scaled_start[5], 31, scaled_extension[5], RA8875_RED);
  }

  if ((level[6] < 0.02)) {
    tft.fillRect(572, 106, 31, 294, RA8875_WHITE);
  }
  else {
    tft.fillRect(572, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(572, scaled_start[6], 31, scaled_extension[6], RA8875_RED);
  }

  if ((level[7] < 0.02)) {
    tft.fillRect(633, 106, 31, 294, RA8875_WHITE);
  }
  else {
    tft.fillRect(633, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(633, scaled_start[7], 31, scaled_extension[7], RA8875_RED);
  }
 
  if ((level[8] < 0.02)) {
    tft.fillRect(694, 106, 31, 294, RA8875_WHITE);
  }
  else {
    tft.fillRect(694, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(694, scaled_start[8], 31, scaled_extension[8], RA8875_RED);
  }

  if ((level[9] < 0.02)) {
    tft.fillRect(755, 106, 31, 294, RA8875_WHITE);
  }
  else {
    tft.fillRect(755, 106, 31, 294, RA8875_WHITE);
    tft.fillRect(755, scaled_start[9], 31, scaled_extension[9], RA8875_RED);
  }

  
}



/*
 * 
 */








/*
  if ((scaled_x >= 350) && (scaled_x <= 450) && (scaled_y >= 200) && (scaled_y <= 280)) {
    push = 1;
  }
  else {
  push = 0;
  }

  Serial.print("Button:");
  Serial.print(push);
  Serial.print("  ");
*/
