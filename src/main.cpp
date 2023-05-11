//Platform libraries
#include <Arduino.h>

//E-INK libraries
#include <GxEPD2_7C.h> //Include only part for 7-color displays
#include "GxEPD2_display_selection.h" //Display selection
#include "Fonts/FreeMonoBold12pt7b.h" //Font selection

//Bitmap test
#include "bitmaps/Bitmaps200x200.h"
#include "bitmaps/Bitmaps400x300.h"
#include "imageFull.h"

// #include "bitmaps/Bitmaps3c200x200.h"
// #include "bitmaps/WS_Bitmaps7c192x143.h"
#include "bitmaps/WS_Bitmaps7c300x180.h"

//My "libraries"
#include "debug.h"

//Text demo printing
void demoPrint()
{
    Debug::printLine("Start printing");

    const char testText[] = "Text 1 test sdfsffsd";
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

        display.setTextColor(GxEPD_GREEN);
        display.setCursor(0,80);
        display.print(testText);

        display.setTextColor(GxEPD_YELLOW);
        display.setCursor(0,100);
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

void demoPrint5()
{
  display.setRotation(0);
  display.fillScreen(GxEPD_WHITE);
  Serial.println("DrawImageMethod is drawing");
  display.firstPage();
  do
  {
    //display.drawBitmap(0,0, Image7color, 800, 480, GxEPD_BLACK);
    display.drawImage(WS_Bitmap7c300x180, GxEPD_RED, 0, 0, 300, 180);

  } while (display.nextPage());

  delay(5000);
  Serial.println("DrawImageMethod is drawing different image");
  display.firstPage();
  do
  {
    //display.drawBitmap(0,0, Image7color, 800, 480, GxEPD_BLACK);
    display.drawImage(Bitmap400x300_1, GxEPD_RED, 0, 0, 300, 180);

  } while (display.nextPage());

  delay(5000);
  Serial.println("DrawNative is drawing same image as previous");
  display.firstPage();
  do
  {
    //display.drawBitmap(0,0, Image7color, 800, 480, GxEPD_BLACK);
    //display.drawImage(Bitmap400x300_1, GxEPD_RED, 0, 0, 300, 180);
    display.drawNative(Bitmap400x300_1, 0, 0 , 0, 400, 300, false, false, false);

  } while (display.nextPage());
  
}

struct bitmap_pair
{
  const unsigned char* black;
  const unsigned char* red;
};

void demoPrint6()
{
  // bitmap_pair bitmap_pairs[] =
  // {
  //   //{Bitmap3c200x200_black, Bitmap3c200x200_red},
  //   {WS_Bitmap3c200x200_black, WS_Bitmap3c200x200_red}
  // };
  // if (display.epd2.panel == GxEPD2::GDEW0154Z04)
  // {
  //   display.firstPage();
  //   do
  //   {
  //     display.fillScreen(GxEPD_WHITE);
  //     // Bitmap3c200x200_black has 2 bits per pixel
  //     // taken from Adafruit_GFX.cpp, modified
  //     int16_t byteWidth = (display.epd2.WIDTH + 7) / 8; // Bitmap scanline pad = whole byte
  //     uint8_t byte = 0;
  //     display.drawInvertedBitmap(0, 0, Bitmap3c200x200_red, display.epd2.WIDTH, display.epd2.HEIGHT, GxEPD_RED);
  //   }
  //   while (display.nextPage());
  //   delay(2000);
  //   for (uint16_t i = 0; i < sizeof(bitmap_pairs) / sizeof(bitmap_pair); i++)
  //   {
  //     display.firstPage();
  //     do
  //     {
  //       display.fillScreen(GxEPD_WHITE);
  //       display.drawInvertedBitmap(0, 0, bitmap_pairs[i].black, display.epd2.WIDTH, display.epd2.HEIGHT, GxEPD_BLACK);
  //       display.drawInvertedBitmap(0, 0, bitmap_pairs[i].red, display.epd2.WIDTH, display.epd2.HEIGHT, GxEPD_RED);
  //     }
  //     while (display.nextPage());
  //     delay(2000);
  //   }
  // }
  // if (display.epd2.hasColor)
  // {
  //   display.clearScreen(); // use default for white
  //   int16_t x = (int16_t(display.epd2.WIDTH) - 200) / 2;
  //   int16_t y = (int16_t(display.epd2.HEIGHT) - 200) / 2;
  //   for (uint16_t i = 0; i < sizeof(bitmap_pairs) / sizeof(bitmap_pair); i++)
  //   {
  //     display.drawImage(bitmap_pairs[i].black, bitmap_pairs[i].red, x, y, 200, 200, false, false, true);
  //     delay(2000);
  //   }
  //   display.writeScreenBuffer(); // use default for white
  //   display.writeImage(bitmap_pairs[0].black, bitmap_pairs[0].red, 0, 0, 200, 200, false, false, true);
  //   display.writeImage(bitmap_pairs[0].black, bitmap_pairs[0].red, int16_t(display.epd2.WIDTH) - 200, int16_t(display.epd2.HEIGHT) - 200, 200, 200, false, false, true);
  //   display.refresh();
  //   delay(2000);
  // }
}

void demoPrint7()
{
    //display.drawNative(WS_Bitmap7c192x143, 0, (display.epd2.WIDTH - 192) / 2, (display.epd2.HEIGHT - 143) / 2, 192, 143, false, false, true);
    display.drawNative(Image7color, 0, 0, 0, display.epd2.WIDTH, display.epd2.HEIGHT, false, false, true);
    delay(5000);
}


void setup()
{
    Serial.begin(115200);
    display.init(115200, true, 2, false); //Default values from example code, not much documentation to wrap my head around it.
    Debug::printLine("Initial");
    demoPrint5();
    delay(8000);
    //demoPrint7();
    display.hibernate();
    Debug::printLine("Finished writing");    
} 

void loop()
{
  
}
