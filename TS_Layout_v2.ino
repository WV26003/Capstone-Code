#include "capstone_functions.h"

#define RA8875_INT     3
#define RA8875_CS      10
#define RA8875_RESET   9
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


void setup() {
  Serial.begin(9600);
  Serial.println("Hello, RA8875!");

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
  if (tft.touched()) {
    getMappedCoordinates(x, y, tft);
   printCoordinates(x, y);

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
  }
}

////Code dungeon for stuff that may or may not be important
  ///Initially in the main loop before the touch event stuff 
  /*
  while (digitalRead(RA8875_INT))
  {
  }
  */
