#include "pins_arduino.h"
#include "DefuseMe.h"
#include "LED.h"
#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"


DefuseMeModule module;
Adafruit_7segment matrix = Adafruit_7segment();
LED armedLED(0);
LED strikeLED[4] { (9), (6), (5), (3) };

byte blink = 0;
byte dim = 0;
long countdown = 0;
byte state = 1;
byte strikes = 0;
unsigned long millisSlot = 0;

///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////

void setup (void)
{
  //Serial.begin (115200);

  Serial.print("Main Display");
  Serial.println();

  matrix.begin(0x70);

  //The Values we want to send out to our neighbours
  tag *ourtags = new tag[2];
  ourtags[0] = {.name = F("ACTIVE"), .data = "false"}; //passive module =>no user interaction possible
  ourtags[1] = {.name = F("7SEG"), .data = "1"}; //7 segment panels

  //creates the module description and waits for the bomb controller to send the broadcasts of the other members and start the game
  module.waitForInit(NULL, 0, F("ID:8888\n"
                                "VERSION:0.1\n"
                                "URL:https://example.com/\n"
                                "AUTHOR:JK\n"
                                "DESC:Main Display\n"
                                "REPO:https://github.com/me/awesome-module.git\n"),
                     ourtags, 2);

}

///////////////////////////////////////////////////////////////////////////////

void loop (void)
{
  if (module.updateState())
  {
    countdown = module.getGameState().time;
    state = module.getGameState().state;
    strikes = module.getGameState().strikes;

    armedLED = state;

    blink = !blink;

    int sec = countdown / 1000;
    int min = sec / 60;
    sec = sec % 60;

    matrix.writeDigitNum(0, min / 10);
    matrix.writeDigitNum(1, min % 10);
    matrix.drawColon(blink);
    matrix.writeDigitNum(3, sec / 10);
    matrix.writeDigitNum(4, sec % 10);
    matrix.writeDisplay();
  }

  unsigned long millisAct = millis();
  if (millisAct < millisSlot)
    return;
  millisSlot = millisAct + 4;

  if (state == 2)
  {
    for (byte i = 0; i < 4; i++)
      strikeLED[i] = !random(4);
  }
  else if (state == 0)
  {
    for (byte i = 0; i < 4; i++)
      strikeLED[i] = 0;
  }
  else
  {
    dim--;
    for (byte i = 0; i < 4; i++)
      strikeLED[i].dim((strikes > i) ? dim + 23 * i : 0);
  }
}

///////////////////////////////////////////////////////////////////////////////

