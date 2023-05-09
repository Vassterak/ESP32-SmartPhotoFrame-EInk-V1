//Platform libraries
#include <Arduino.h>

//E-INK libraries
#include <GxEPD2_7C.h> //Include only part for 7-color displays
#include "GxEPD2_display_selection.h" //Display selection
#include "Fonts/FreeMonoBold12pt7b.h" //Font selection

//My "libraries"
#include "debug.h"

void demoPrint()
{
    Debug::printLine("Start printing");

    const char testText[] = "Hello flustration";
    display.setFullWindow();
    display.setRotation(0); // number 0 - 3, each rotate by 90°

    display.setFont(&FreeMonoBold12pt7b);
    display.setTextColor(GxEPD_RED);

    display.firstPage();
    do
    {
        display.fillScreen(GxEPD_GREEN);
        display.setCursor(0,20);
        display.print(testText);
    } while (display.nextPage());
    Debug::printLine("Finished writing");
}

void setup()
{
    Serial.begin(115200);
    display.init(115200, true, 2, false); //Default values from example code, not much documentation to wrap my head around it.
    demoPrint();

    display.hibernate();
}

void loop()
{
  
}
