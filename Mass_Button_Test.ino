#include <capstone_functions.h>

QwiicButton Qbutton1; // 0x60
QwiicButton Qbutton2; //0x61
QwiicButton Qbutton3; //0x62
QwiicButton Qbutton4; //0x63
QwiicButton Qbutton5;//0x64
QwiicButton Qbutton6;//0x65
QwiicButton Qbutton7;//0x66
QwiicButton Qbutton8;//0x67
uint8_t brightness = 100;   //The brightness to set the LED to when the button is pushed
bool QLEDState1 = false;      //Can be any value between 0 (off) and 255 (max)
bool QLEDState2 = false;
bool QLEDState3 = false;
bool QLEDState4 = false;
bool QLEDState5 = false;
bool QLEDState6 = false;
bool QLEDState7 = false;
bool QLEDState8 = false;


void setup() {
  Wire1.begin();
  if (Qbutton1.begin(0x60, Wire1) == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1);
  }
  Serial.println("Qbutton1 acknowledged.");
  if (Qbutton2.begin(0x61, Wire1) == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1);
  }
  
  Serial.println("Qbutton2 acknowledged.");
  if (Qbutton3.begin(0x62, Wire1) == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1);
  }
  Serial.println("Qbutton3 acknowledged.");
 
  if (Qbutton4.begin(0x63, Wire1) == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1);
  }
  Serial.println("Qbutton4 acknowledged.");
  if (Qbutton5.begin(0x64, Wire1) == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1);
  }
  Serial.println("Qbutton5 acknowledged.");
  if (Qbutton6.begin(0x65, Wire1) == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1);
  }
    Serial.println("Qbutton6 acknowledged.");
     if (Qbutton7.begin(0x66, Wire1) == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1);
  }
    Serial.println("Qbutton7 acknowledged.");
     if (Qbutton8.begin(0x67, Wire1) == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1);
  }
    Serial.println("Qbutton8 acknowledged.");

  Qbutton1.LEDoff();
  Qbutton2.LEDoff();
  Qbutton3.LEDoff();
  Qbutton4.LEDoff();
  Qbutton5.LEDoff();
  Qbutton6.LEDoff();
  Qbutton7.LEDoff();
  Qbutton8.LEDoff();
}

void loop() {
  button_toggle(Qbutton1, QLEDState1);
  button_toggle(Qbutton2, QLEDState2);
  button_toggle(Qbutton3, QLEDState3);
  button_toggle(Qbutton4, QLEDState4);
  button_toggle(Qbutton5, QLEDState5);
  button_toggle(Qbutton6, QLEDState6);
  button_toggle(Qbutton7, QLEDState7);
  button_toggle(Qbutton8, QLEDState8);
 
}
