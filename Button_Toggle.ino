/******************************************************************************
  Turns on the Button's built in LED when pressed, and prints status over Serial!

  Fischer Moseley @ SparkFun Electronics
  Original Creation Date: July 24, 2019

  This code is Lemonadeware; if you see me (or any other SparkFun employee) at the
  local, and you've found our code helpful, please buy us a round!

  Hardware Connections:
  Attach the Qwiic Shield to your Arduino/Photon/ESP32 or other
  Plug the button into the shield
  Print it to the serial monitor at 115200 baud.

  Distributed as-is; no warranty is given.
******************************************************************************/

#include <SparkFun_Qwiic_Button.h>
QwiicButton button1;
QwiicButton button2;
uint8_t brightness = 100;   //The brightness to set the LED to when the button is pushed
bool ledState1 = false;      //Can be any value between 0 (off) and 255 (max)
bool ledState2 = false;
void button_toggle(QwiicButton &button, bool &ledState);

void setup() {
  Serial.begin(115200);
  Serial.println("Qwiic button examples");
  Wire1.begin(); //Join I2C bus

  //check if button will acknowledge over I2C
  if (button1.begin(0x5B, Wire1) == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1);
  }
  Serial.println("Button1 acknowledged.");
  if (button2.begin(0x6F, Wire1) == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1);
  }
  Serial.println("Button2 acknowledged.");

  //Start with the LED off
  button1.LEDoff();
  button2.LEDoff();
}

void loop() {
  button_toggle(button1, ledState1);
  button_toggle(button2, ledState2);
}

void button_toggle(QwiicButton &button, bool &ledState){
   if (button.hasBeenClicked()) {
    ledState = !ledState; // Toggle the LED state

    if (ledState) {
      button.LEDon(); // Turn the LED on
    } else {
      button.LEDoff(); // Turn the LED off
    }

    button.clearEventBits(); // Clear button click event
  }
}
