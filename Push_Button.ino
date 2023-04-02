#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"

#define RA8875_INT 3
#define RA8875_CS 10
#define RA8875_RESET 9

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
uint16_t x, y;

void setup() {
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
  tft.fillRect(350, 200, 100, 80, RA8875_BLUE);
}

//Setting a button
bool pushstate = false;
int push = 0;

//setting screen bounds or something idk
const int TS_LEFT = 40, TS_RT = 980, TS_TOP = 120, TS_BOT = 920;

uint16_t scaled_x;
uint16_t scaled_y;


void loop() {
  
  float xScale = 1024.0F/tft.width();
  float yScale = 1024.0F/tft.height();


/* Clear any previous interrupts to avoid false buffered reads */
  tft.touchRead(&x, &y);
  delay(50);

/* Wait around for a new touch event (INT pin goes low) */
  while (digitalRead(RA8875_INT))
  {
  }
  
/* Make sure this is really a touch event */
  if (tft.touched()) {
    tft.touchRead(&x, &y);
//    scaled_x = (x/xScale);
//    scaled_y = (y/yScale);

    scaled_x = map(x, TS_LEFT, TS_RT, 0, tft.width());
    scaled_y = map(y, TS_TOP, TS_BOT, 0, tft.height());

    Serial.print("X = ");
    Serial.print(scaled_x);
    Serial.print(", y = ");
    Serial.println(scaled_y);
  }



  if ((scaled_x >= 350) && (scaled_x <= 450) && (scaled_y >= 200) && (scaled_y <= 280)) {
    push = 1;
  }
  else {
  push = 0;
  }

  Serial.print("Button:");
  Serial.print(push);
  Serial.print("  ");

}
