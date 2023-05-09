//Platform libraries
#include <Arduino.h>

//E-INK libraries
#include <GxEPD2_7C.h> //Include only part for 7-color displays
#include "GxEPD2_display_selection.h" //Display selection
#include "Fonts/FreeMonoBold12pt7b.h" //Font selection

//Bitmap test
#include "bitmaps/Bitmaps200x200.h"
#include "bitmaps/Bitmaps400x300.h"

//My "libraries"
#include "debug.h"

//Text demo printing
void demoPrint()
{
    Debug::printLine("Start printing");

    const char testText[] = "Hello flustration";
    display.setFullWindow();
    display.setRotation(1); // number 0 - 3, each rotate by 90°

    display.setFont(&FreeMonoBold12pt7b);
    display.firstPage();
    do
    {
        display.fillScreen(GxEPD_WHITE);
        display.setTextColor(GxEPD_GREEN);
        display.setCursor(0,20);
        display.print(testText);

        display.setTextColor(GxEPD_RED);
        display.setCursor(0,40);
        display.print(testText);

        display.setTextColor(GxEPD_BLUE);
        display.setCursor(0,60);
        display.print(testText);

    } while (display.nextPage());
}

//Image bitemap printing
void demoPrint2()
{
    display.firstPage();
    do
    {
        display.fillScreen(GxEPD_WHITE);
        //display.drawBitmap(100, 100, Bitmap400x300_1, 400, 300, GxEPD_BLACK); //monochrome only
        display.drawInvertedBitmap(100, 100, Bitmap400x300_1, 400, 300, GxEPD_BLACK);


    } while (display.nextPage());
}


void demoPrint3()
{
  display.setRotation(0);
  display.firstPage();
  do
  {
    display.drawRect(display.width() / 8, display.height() / 8, display.width() * 3 / 4, display.height() * 3 / 4, GxEPD_BLACK);
    display.drawLine(display.width() / 8, display.height() / 8, display.width() * 7 / 8, display.height() * 7 / 8, GxEPD_BLACK);
    display.drawLine(display.width() / 8, display.height() * 7 / 8, display.width() *7 / 8, display.height() / 8, GxEPD_BLACK);
    display.drawCircle(display.width() / 2, display.height() / 2, display.height() / 4, GxEPD_BLACK);
    display.drawPixel(display.width() / 4, display.height() / 2 , GxEPD_BLACK);
    display.drawPixel(display.width() * 3 / 4, display.height() / 2 , GxEPD_BLACK);
  }
  while (display.nextPage());
}

void demoPrint4()
{
  display.setRotation(0);
  uint16_t h = display.height() / 7;
  display.firstPage();
  do
  {
    display.fillRect(0, 0, display.width(), h, GxEPD_BLACK);
    display.fillRect(0, h, display.width(), h, GxEPD_WHITE);
    display.fillRect(0, 2 * h, display.width(), h, GxEPD_GREEN);
    display.fillRect(0, 3 * h, display.width(), h, GxEPD_BLUE);
    display.fillRect(0, 4 * h, display.width(), h, GxEPD_RED);
    display.fillRect(0, 5 * h, display.width(), h, GxEPD_YELLOW);
    display.fillRect(0, 6 * h, display.width(), h, GxEPD_ORANGE);
  }
  while (display.nextPage());
}

void setup()
{
    Serial.begin(115200);
    display.init(115200, true, 2, false); //Default values from example code, not much documentation to wrap my head around it.
    //demoPrint();
    demoPrint4();
    display.hibernate();
    Debug::printLine("Finished writing");
}

void loop()
{
  
}
