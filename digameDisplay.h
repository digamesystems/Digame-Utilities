#pragma once 
/*Functions for using the Adafruit 1.54" Monochrome eInk Display.
 *
 * See: https://www.adafruit.com/product/4196
 *
 */

// The base class GxEPD2_GFX can be used to pass references or pointers to the display
// instance as parameter, uses ~1.2k more code
// enable or disable GxEPD2_GFX base class
#define ENABLE_GxEPD2_GFX 1 // Took me a longer than it should have to find this! --

#include <digameDebug.h>
#include <GxEPD2_BW.h>
#include <EEPROM.h>

// Our pinouts to the Adafruit display.
#define EPD_DC 5     // can be any pin, but required!
#define EPD_CS 26    // can be any pin, but required!
#define EPD_BUSY 2   // can set to -1 to not use a pin (will wait a fixed delay)
#define SRAM_CS 27   // can set to -1 to not use a pin (uses a lot of RAM!)
#define SD_CS 14     // SD card chip select
#define EPD_RESET -1 // can set to -1 and share with chip Reset (can't deep sleep)

// Just grabbed the constructors we need from GxEPD2_display_selection and copied here.

// ADAFRUIT 1.54" ePaper Display. SSD1680
static GxEPD2_BW<GxEPD2_154, GxEPD2_154::HEIGHT> display1(GxEPD2_154(/*CS=5*/ EPD_CS, /*DC=*/EPD_DC, /*RST=*/EPD_RESET, /*BUSY=*/EPD_BUSY)); // GDEH0154D67

// ADAFRUIT 1.54" ePaper Display. SSD1681
static GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display2(GxEPD2_154_D67(/*CS=5*/ EPD_CS, /*DC=*/EPD_DC, /*RST=*/EPD_RESET, /*BUSY=*/EPD_BUSY)); // GDEW0154M09 200x200

// ADAFRUIT 2.13" ePaper Display. ssd1680
static GxEPD2_BW<GxEPD2_213, GxEPD2_213::HEIGHT> display3a(GxEPD2_213(/*CS=5*/ EPD_CS, /*DC=*/EPD_DC, /*RST=*/EPD_RESET, /*BUSY=*/EPD_BUSY)); //

// ADAFRUIT 2.13" ePaper Display. ssd1680
static GxEPD2_BW<GxEPD2_213_B72, GxEPD2_213_B72::HEIGHT> display3(GxEPD2_213_B72(/*CS=5*/ EPD_CS, /*DC=*/EPD_DC, /*RST=*/EPD_RESET, /*BUSY=*/EPD_BUSY)); //

// ADAFRUIT 2.13" ePaper Display. ssd1680
static GxEPD2_BW<GxEPD2_213_B73, GxEPD2_213_B73::HEIGHT> display3c(GxEPD2_213_B73(/*CS=5*/ EPD_CS, /*DC=*/EPD_DC, /*RST=*/EPD_RESET, /*BUSY=*/EPD_BUSY)); //

// ADAFRUIT 2.13" ePaper Display. ssd1680
static GxEPD2_BW<GxEPD2_213_B74, GxEPD2_213_B74::HEIGHT> display4(GxEPD2_213_B74(/*CS=5*/ EPD_CS, /*DC=*/EPD_DC, /*RST=*/EPD_RESET, /*BUSY=*/EPD_BUSY)); //

// Waveshare 2.9" display
static GxEPD2_BW<GxEPD2_290_T94_V2, GxEPD2_290_T94_V2::HEIGHT> display5(GxEPD2_290_T94_V2(/*CS=5*/ EPD_CS, /*DC=*/EPD_DC, /*RST=*/EPD_RESET, /*BUSY=*/EPD_BUSY)); //

// GxEPD2_GFX& display = display2;
static String displayType = "154_SSD1681";

static GxEPD2_GFX *mainDisplay;

//******************************************************************************************
#if defined(ESP8266) || defined(ESP32)
  #include <StreamString.h>
  #define PrintString StreamString
#else
  class PrintString : public Print, public String
  {
  public:
    size_t write(uint8_t data) override
    {
      return concat(char(data));
    };
  };
#endif

GxEPD2_GFX &getDisplay();

// Lower-Level Display Functions
void initDisplay();
void centerPrint(String s, uint16_t y); // Print a string on the display, centered, at a particular y value.
void showValue(double v);
void showPartialXY(String msg, int x, int y);
void showWhite();
void showBlack();

// Higher-Level Display Functions
void displayTitle(String title);
void displayCenteredText(String title, String s1="", String s2="", String s3="", String s4="", String s5="", String s6="", String s7="", String s8="");
void displayCenteredTextLarge(String title, String s1="", String s2="", String s3="", String s4="", String s5="", String s6="", String s7="", String s8="");

void displayRawText(String Title, String s);
void displayCopyright(); // A small copyright declaration at the bottom of the screen.
void displayUpdate();


