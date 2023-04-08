#include "capstone_functions.h"

#define RA8875_INT     3
#define RA8875_CS      10
#define RA8875_RESET   9

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
uint16_t x, y;
bool push = false;
unsigned long lastButtonPress = 0;

struct sliderBox {
  int leftBound;
  int rightBound;
  int topBound;
  int botBound;
};

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
    readTouchInput(x, y, tft);
    getMappedCoordinates(x, y, tft);
    printCoordinates(x, y);

    if (isButtonPressed(x, y, 4, 3 + 110, 6, 6 + 60)) {
      handleMenuPress(push, lastButtonPress, currentMillis, debounceTime, tft);
    } else {
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
