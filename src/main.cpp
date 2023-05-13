//Platform libraries
#include <Arduino.h>

#if defined(ESP32)
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#endif

#define SD_CS 15  // adapted to my wiring

//E-INK libraries
#include <GxEPD2_7C.h> //Include only part for 7-color displays
#include "GxEPD2_display_selection.h" //Display selection
#include "Fonts/FreeMonoBold12pt7b.h" //Font selection

//My "libraries"
#include "debug.h"

//Variables and instances
SPIClass hspi(HSPI);
uint8_t numberOfFiles = 0;
uint8_t currentIndex = 0;
const String fileTypeName = ".bmp";
bool isBitMapDrawing = false;

//Buttons
#define MOSI_PULL_BOOT_DOWN 27
#define BUTTON_PIN 26
bool buttonWasPressed = false;
unsigned long buttonStateChangeTime = 0; // Debounce timer
const unsigned long debounceTime = 20;
unsigned long currentTime = 0;

// bitmap drawing using buffered graphics, e.g. for small bitmaps or for GxEPD2_154c
// draws BMP bitmap according to set orientation
// partial_update selects refresh mode (not effective for GxEPD2_154c)
// overwrite = true does not clear buffer before drawing, use only if buffer is full height
void drawBitmapFromSD_Buffered(String filename, int16_t x, int16_t y, bool with_color = true, bool partial_update = false);

void displayClenup()
{
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);

  } while (display.nextPage());
}

void sdCardSetup()
{
  Debug::print("Initializing SD card...");
  if (!SD.begin(SD_CS, hspi))
  {
    Debug::printLine("PART1");
    Debug::printLine("SD failed!");
    return;
  }
    Debug::printLine("SD OK!");
}

void spaces(int num)
{
  for (int i = 0; i < num; i++)
  {
    Serial.print(" ");
  }
}

void listFiles()
{
  Debug::printLine("All Files on SD:");
  File root = SD.open("/");
  if (root)
  {
    if (root.isDirectory())
    {
      File file = root.openNextFile();
      while (file)
      {
        Serial.print("  ");
        Serial.print(file.name());
        spaces(20 - strlen(file.name()));
        Serial.print("  ");
        Serial.print(file.size());
        Serial.print(" bytes");
        Debug::printLine("");
        file = root.openNextFile();
        numberOfFiles++; //read all files available 
      }
      Debug::printLine("no more files...");
      numberOfFiles--;
    }
    else
    Serial.print("Not a directory");
  }
  else
    Serial.print("failed to open root directory");
}

void drawBitmap()
{
  display.setRotation(0);
  // int16_t x = (display.width() - 200) / 2;
  // int16_t y = (display.height() - 200) / 2;
  // drawBitmapFromSD_Buffered("borka24bit.bmp", 0,0, true, false);
  // delay(5000);
  //Memory allocation is not good practise on MCU but ES32 is powerfull enought, but for production code, do not use it...
  String imageName = String(currentIndex) + fileTypeName;
  drawBitmapFromSD_Buffered(imageName, 0, 0, true, false);
  currentIndex++;
  Debug::print("Current index: ");
  Serial.println(currentIndex);
  if (currentIndex == numberOfFiles)
  {
    currentIndex = 0;
  }
  delay(5000);
  isBitMapDrawing = false;
}

//Button that allows image switches 
void checkButton()
{
  if (isBitMapDrawing)
    return;
  
  else
  {
    currentTime = millis();
    bool buttonIsPressed = digitalRead(BUTTON_PIN) == LOW;
    // Check for button state change and do debounce, is required for this fast MCU
    if (buttonIsPressed != buttonWasPressed && (currentTime  -  buttonStateChangeTime > debounceTime))
    {
      buttonWasPressed = buttonIsPressed;
      buttonStateChangeTime = currentTime;

      // Button was just pressed
      if (buttonWasPressed)
      {
        isBitMapDrawing = true;
        Debug::printLine("Button is pressed");
        drawBitmap();
      }

      // Button was just released
      else
      {
        Debug::printLine("Button is released");
      }
    }
  }
}

void setup()
{
    //Button to switch images
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    //need to do this because during boot pin need to be LOW, but during SPI communication pin need to be HIGH
    pinMode(MOSI_PULL_BOOT_DOWN, OUTPUT);
    digitalWrite(MOSI_PULL_BOOT_DOWN, LOW);
    Serial.begin(115200);
    display.init(115200, true, 2, false); //Default values from example code, not much documentation to wrap my head around it.
    Debug::printLine("Display initialised");
    delay(1000);
    digitalWrite(MOSI_PULL_BOOT_DOWN, HIGH);
    delay(1000);
    //SPI communication with sd card, setup
    hspi.begin(14, 12, 13, 15);
    delay(5000);
    sdCardSetup();
    listFiles();
    Debug::print("Number of pictures: ");
    Serial.println(numberOfFiles);
    displayClenup();
    display.hibernate();
    Debug::printLine("Finished setup");    
}

void loop()
{
  checkButton();
}

uint8_t checkCorrectRotation(uint32_t width, uint32_t height)
{
  //image is flipped 
  if (width <= 480 && height <= 800 && height > 480)
  {
    Debug::printLine("Loaded image is flipped");
    return 1;
  }

  if (width > 800 || height > 480)
  {
    Debug::printLine("Wrong Resolution");
    return 10;
  }

  return 0;
}

//static const uint16_t input_buffer_pixels = 20; // may affect performance
static const uint16_t input_buffer_pixels = 800; // may affect performance
static const uint16_t max_row_width = 1448; // for up to 6" display 1448x1072
static const uint16_t max_palette_pixels = 256; // for depth <= 8
uint8_t input_buffer[3 * input_buffer_pixels]; // up to depth 24
uint8_t output_row_mono_buffer[max_row_width / 8]; // buffer for at least one row of b/w bits
uint8_t output_row_color_buffer[max_row_width / 8]; // buffer for at least one row of color bits
uint8_t mono_palette_buffer[max_palette_pixels / 8]; // palette buffer for depth <= 8 b/w
uint8_t color_palette_buffer[max_palette_pixels / 8]; // palette buffer for depth <= 8 c/w
uint16_t rgb_palette_buffer[max_palette_pixels]; // palette buffer for depth <= 8 for buffered graphics, needed for 7-color display

uint16_t read16(File& f)
{
  // BMP data is stored little-endian, same as Arduino.
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File& f)
{
  // BMP data is stored little-endian, same as Arduino.
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

void drawBitmapFromSD_Buffered(String filename, int16_t x, int16_t y, bool with_color, bool partial_update)
{
  File file;
  bool valid = false; // valid format to be handled
  bool flip = true; // bitmap is stored bottom-to-top
  bool has_multicolors = (display.epd2.panel == GxEPD2::ACeP565) || (display.epd2.panel == GxEPD2::GDEY073D46);
  uint32_t startTime = millis();
  if ((x >= display.width()) || (y >= display.height())) return;
  Debug::printLine("");
  Serial.print("Loading image '");
  Serial.print(filename);
  Debug::printLine('\'');
#if defined(ESP32)
  file = SD.open(String("/") + filename, FILE_READ);
  if (!file)
  {
    Serial.println("File not found");
    return;
  }
#else
  file = SD.open(filename);
  if (!file)
  {
    Serial.print("File not found");
    return;
  }
#endif
  // Parse BMP header
  if (read16(file) == 0x4D42) // BMP signature
  {
    uint32_t fileSize = read32(file);
    uint32_t creatorBytes = read32(file); (void)creatorBytes; //unused
    uint32_t imageOffset = read32(file); // Start of image data
    uint32_t headerSize = read32(file);
    uint32_t width  = read32(file);
    int32_t height = (int32_t) read32(file);
    uint16_t planes = read16(file);
    uint16_t depth = read16(file); // bits per pixel
    uint32_t format = read32(file);
    if ((planes == 1) && ((format == 0) || (format == 3))) // uncompressed is handled, 565 also
    {
      Serial.print("File size: "); Serial.println(fileSize);
      Serial.print("Image Offset: "); Serial.println(imageOffset);
      Serial.print("Header size: "); Serial.println(headerSize);
      Serial.print("Bit Depth: "); Serial.println(depth);
      Serial.print("Image size: ");
      Serial.print(width);
      Serial.print('x');
      Serial.println(height);

      //Resultion check and if image is flipped rotate it
      uint8_t setRotation = checkCorrectRotation(width, height);
      if (setRotation == 10)
        return;
      if (setRotation == 1)
      {
        display.setRotation(1);
      }

      // BMP rows are padded (if needed) to 4-byte boundary
      uint32_t rowSize = (width * depth / 8 + 3) & ~3;
      if (depth < 8) rowSize = ((width * depth + 8 - depth) / 8 + 3) & ~3;
      if (height < 0)
      {
        height = -height;
        flip = false;
      }
      uint16_t w = width;
      uint16_t h = height;
      if ((x + w - 1) >= display.width())  w = display.width()  - x;
      if ((y + h - 1) >= display.height()) h = display.height() - y;
      //if (w <= max_row_width) // handle with direct drawing
      {
        valid = true;
        uint8_t bitmask = 0xFF;
        uint8_t bitshift = 8 - depth;
        uint16_t red, green, blue;
        bool whitish = false;
        bool colored = false;
        if (depth == 1) with_color = false;
        if (depth <= 8)
        {
          if (depth < 8) bitmask >>= depth;
          //file.seek(54); //palette is always @ 54
          file.seek(imageOffset - (4 << depth)); //54 for regular, diff for colorsimportant
          for (uint16_t pn = 0; pn < (1 << depth); pn++)
          {
            blue  = file.read();
            green = file.read();
            red   = file.read();
            file.read();
            whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
            colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
            if (0 == pn % 8) mono_palette_buffer[pn / 8] = 0;
            mono_palette_buffer[pn / 8] |= whitish << pn % 8;
            if (0 == pn % 8) color_palette_buffer[pn / 8] = 0;
            color_palette_buffer[pn / 8] |= colored << pn % 8;
            rgb_palette_buffer[pn] = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
          }
        }
        if (partial_update) display.setPartialWindow(x, y, w, h);
        else display.setFullWindow();
        display.firstPage();
        do
        {
          //if (!overwrite) display.fillScreen(GxEPD_WHITE);
          uint32_t rowPosition = flip ? imageOffset + (height - h) * rowSize : imageOffset;
          for (uint16_t row = 0; row < h; row++, rowPosition += rowSize) // for each line
          {
            uint32_t in_remain = rowSize;
            uint32_t in_idx = 0;
            uint32_t in_bytes = 0;
            uint8_t in_byte = 0; // for depth <= 8
            uint8_t in_bits = 0; // for depth <= 8
            uint16_t color = GxEPD_WHITE;
            file.seek(rowPosition);
            for (uint16_t col = 0; col < w; col++) // for each pixel
            {
              // Time to read more pixel data?
              if (in_idx >= in_bytes) // ok, exact match for 24bit also (size IS multiple of 3)
              {
                in_bytes = file.read(input_buffer, in_remain > sizeof(input_buffer) ? sizeof(input_buffer) : in_remain);
                in_remain -= in_bytes;
                in_idx = 0;
              }
              switch (depth)
              {
                case 32:
                  blue = input_buffer[in_idx++];
                  green = input_buffer[in_idx++];
                  red = input_buffer[in_idx++];
                  in_idx++; // skip alpha
                  whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
                  colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
                  color = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
                  break;
                case 24:
                  blue = input_buffer[in_idx++];
                  green = input_buffer[in_idx++];
                  red = input_buffer[in_idx++];
                  whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
                  colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
                  color = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
                  break;
                case 16:
                  {
                    uint8_t lsb = input_buffer[in_idx++];
                    uint8_t msb = input_buffer[in_idx++];
                    if (format == 0) // 555
                    {
                      blue  = (lsb & 0x1F) << 3;
                      green = ((msb & 0x03) << 6) | ((lsb & 0xE0) >> 2);
                      red   = (msb & 0x7C) << 1;
                      color = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
                    }
                    else // 565
                    {
                      blue  = (lsb & 0x1F) << 3;
                      green = ((msb & 0x07) << 5) | ((lsb & 0xE0) >> 3);
                      red   = (msb & 0xF8);
                      color = (msb << 8) | lsb;
                    }
                    whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
                    colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
                  }
                  break;
                case 1:
                case 2:
                case 4:
                case 8:
                  {
                    if (0 == in_bits)
                    {
                      in_byte = input_buffer[in_idx++];
                      in_bits = 8;
                    }
                    uint16_t pn = (in_byte >> bitshift) & bitmask;
                    whitish = mono_palette_buffer[pn / 8] & (0x1 << pn % 8);
                    colored = color_palette_buffer[pn / 8] & (0x1 << pn % 8);
                    in_byte <<= depth;
                    in_bits -= depth;
                    color = rgb_palette_buffer[pn];
                  }
                  break;
              }
              if (with_color && has_multicolors)
              {
                // keep color
              }
              else if (whitish)
              {
                color = GxEPD_WHITE;
              }
              else if (colored && with_color)
              {
                color = GxEPD_COLORED;
              }
              else
              {
                color = GxEPD_BLACK;
              }
              uint16_t yrow = y + (flip ? h - row - 1 : row);
              display.drawPixel(x + col, yrow, color);
            } // end pixel
          } // end line
          Serial.print("page loaded in "); Serial.print(millis() - startTime); Debug::printLine(" ms");
        }
        while (display.nextPage());
        Serial.print("loaded in "); Serial.print(millis() - startTime); Debug::printLine(" ms");
      }
    }
  }
  file.close();
  if (!valid)
  {
    Debug::printLine("bitmap format not handled.");
  }
}