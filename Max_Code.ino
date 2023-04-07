#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"
/*
#if defined(EEPROM_SUPPORTED)
  #include <EEPROM.h>
#endif
#define EEPROMLOCATION 100
*/
#define RA8875_INT     3
#define RA8875_CS      10
#define RA8875_RESET   9

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
tsPoint_t       _tsLCDPoints[3];
tsPoint_t       _tsTSPoints[3];
boolean istouched = false;
uint16_t x, y;

struct sliderBox{

  int leftBound;
  int rightBound;
  int topBound;
  int botBound;

};

void printMenu(){
  
  tft.fillScreen(RA8875_WHITE);

  //Prints left text box
  tft.fillRect(95, 0, 170, 55, RA8875_BLACK); 
  tft.fillRect(100, 0, 160, 50, RA8875_WHITE);

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

  //Puts back into graphics mode (Not sure if this is needed yet but text didn't work without textMode)
  tft.graphicsMode();
}

void printPEQ(){
  ///bool exitControl = 0;

  tft.fillScreen(RA8875_WHITE);
  
  ///LEFT BUTTONS (make just one rect for black back)
  tft.fillRect(0, 0, 120, 480, RA8875_BLACK); 
  tft.fillRect(4, 6, 110, 60, RA8875_GREEN);
  tft.fillRect(4, 74, 110, 60, RA8875_GREEN);
  tft.fillRect(4, 142, 110, 60, RA8875_GREEN);
  tft.fillRect(4, 210, 110, 60, RA8875_GREEN);
  tft.fillRect(4, 278, 110, 60, RA8875_GREEN);
  tft.fillRect(4, 346, 110, 60, RA8875_GREEN); 
  tft.fillRect(4, 414, 110, 60, RA8875_GREEN);

  ///Cuttoff, Bandwidth, Gain, On/OFF Boxes
  tft.fillRect(120, 0, 158, 280, RA8875_BLACK);
  tft.fillRect(122, 6, 150, 60, RA8875_CYAN);
  tft.fillRect(122, 74, 150, 60, RA8875_CYAN);
  tft.fillRect(122, 142, 150, 60, RA8875_CYAN); 
  tft.fillRect(122, 210, 150, 60, RA8875_GREEN);

  ///High-Pass Filter boxes
  tft.fillRect(638, 0, 162, 140, RA8875_BLACK);
  tft.fillRect(646, 6, 150, 60, RA8875_CYAN);
  tft.fillRect(646, 74, 150, 60, RA8875_CYAN);


  ///Bands Selector
  tft.fillRect(272, 270, 374, 214, RA8875_BLACK);
  tft.fillRect(280, 278, 86, 60, RA8875_GREEN);
  tft.fillRect(280, 346, 86, 60, RA8875_GREEN);
  tft.fillRect(280, 414, 86, 60, RA8875_GREEN);
  tft.fillRect(374, 278, 170, 60, RA8875_CYAN);
  tft.fillRect(374, 346, 81, 60, RA8875_GREEN);
  tft.fillRect(463, 346, 81, 60, RA8875_GREEN);
  tft.fillRect(374, 414, 170, 60, RA8875_CYAN);
  tft.fillRect(552, 278, 86, 60, RA8875_GREEN);
  tft.fillRect(552, 346, 86, 60, RA8875_GREEN);
  tft.fillRect(552, 414, 86, 60, RA8875_GREEN);




  //Print text in boxes
  tft.textMode();
  tft.textColor(RA8875_BLACK, RA8875_GREEN);    
  tft.textEnlarge(2);

  tft.textSetCursor(22, 12);
  tft.textWrite("IND");

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

  ///Cut/Band/Gain text
  tft.textEnlarge(1);
  tft.textColor(RA8875_BLACK, RA8875_CYAN);
  tft.textSetCursor(128, 18);
  tft.textWrite("Cut: ");
  tft.textSetCursor(128, 86);
  tft.textWrite("Band: ");
  tft.textSetCursor(128, 154);
  tft.textWrite("Gain: ");

  ///High-Pass Filter text
  tft.textSetCursor(648, 18);
  tft.textWrite("High-Pass");
  tft.textSetCursor(668, 86);
  tft.textWrite("Cut: ");

  ///Put back into graphics mode
  tft.graphicsMode();
};

void setup() {
  Serial.begin(9600);
  Serial.println("Hello, RA8875!");

  /* Initialize the display using 'RA8875_480x272' or 'RA8875_800x480' */
  if (!tft.begin(RA8875_800x480))
  {
    Serial.println("RA8875 not found ... check your wires!");
    while (1);
  }

  /* Enables the display and sets up the backlight */
  Serial.println("Found RA8875");
  tft.displayOn(true);
  tft.GPIOX(true); // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255);

  Serial.println("Enabled the touch screen");
  pinMode(RA8875_INT, INPUT);
  digitalWrite(RA8875_INT, HIGH);
  tft.touchEnable(true);
  tft.fillScreen(RA8875_WHITE);

  tft.fillRect(200, 200, 400, 10, RA8875_BLACK); 
  tft.fillRect(200, 175, 80, 60, RA8875_GREEN);
  ///printMenu();
  ///printPEQ();
}

void loop() {
  tsPoint_t raw;

  tft.touchRead(&x, &y);    // Send touch info out to serial monitor
  delay(20);

  if (tft.touched()) {
    tft.touchRead(&x, &y);
    raw.x = x;
    raw.y = y;
    Serial.print("X = ");
    Serial.print(raw.x);
    Serial.print(", y = ");
    Serial.println(raw.y);
  }

}



////Code dungeon for stuff that may or may not be important
  ///Initially in the main loop before the touch event stuff 
  /*
  while (digitalRead(RA8875_INT))
  {
  }
  */
