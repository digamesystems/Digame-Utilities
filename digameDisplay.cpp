/*Functions for using the Adafruit 1.54" Monochrome eInk Display.
 *
 * See: https://www.adafruit.com/product/4196
 *
 */

// The base class GxEPD2_GFX can be used to pass references or pointers to the display
// instance as parameter, uses ~1.2k more code
// enable or disable GxEPD2_GFX base class
//#define ENABLE_GxEPD2_GFX 1 // Took me a longer than it should have to find this! --

#include <digameDebug.h>
#include <digameDisplay.h>



//******************************************************************************************
GxEPD2_GFX &getDisplay()
//******************************************************************************************
{ // So we can use different displays.
  if (displayType == "154_SSD1608")
  {
    return display1; // 3; //1
  }

  if (displayType == "154_SSD1681")

  {
    return display2;
  }

  if (displayType == "213_SSD1608a")
  {
    return display3;
    // return display5;
  }

  if (displayType == "213_SSD1608b")
  {
    return display4;
    // return display5;
  }

  if (displayType == "290_Waveshare")
  {
    return display5;
    // return display5;
  }

  return display1;
}


//******************************************************************************************
void initDisplay()
//******************************************************************************************
{
  bool changeDisplayType = false;

  DEBUG_PRINTLN("  Initializing eInk Display...");
  DEBUG_PRINTLN("    Reading EEPROM");

  EEPROM.begin(10);
  //delay(1000);
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  if (EEPROM.read(0) == 255)
  {
    DEBUG_PRINTLN("    ****Display Uninitialized****");
  }

  DEBUG_PRINTLN("    Current Display Type: " + String(EEPROM.read(0)));
  DEBUG_PRINT("    Change? y/[n] (You have 2 sec to decide) ");
  unsigned long t1 = millis();
  unsigned long t2 = t1;

  while (
      !(DEBUG_UART.available()) &&
      ((t2 - t1) < 2000))
  {
    t2 = millis();
    //delay(100); // wait for data from the user...
    vTaskDelay(100 / portTICK_PERIOD_MS);
    DEBUG_PRINT(".");
  }

  DEBUG_PRINTLN();

  if (DEBUG_UART.available())
  {
    String ynString = DEBUG_UART.readStringUntil('\n');
    ynString.trim();
    if (ynString == "y")
    {
      changeDisplayType = true;
    }
  }

  if (changeDisplayType)
  {
    DEBUG_PRINTLN("   Enter Display Type 1=154_SSD1608, 2=154_SSD1681, 3=213_SSD1608a, 4=213_SSD1608b, 5=290_Waveshare:");

    while (!(DEBUG_UART.available()))
    {
      //delay(10); // wait for data from the user...
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    String inString = DEBUG_UART.readStringUntil('\n');
    inString.trim();
    DEBUG_PRINT("   You entered: ");
    DEBUG_PRINTLN(inString);

    if (inString == "1")
    {
      displayType = "154_SSD1608";
      EEPROM.write(0, 1);
      EEPROM.commit();
    }

    if (inString == "2")
    {
      displayType = "154_SSD1681";
      EEPROM.write(0, 2);
      EEPROM.commit();
    }

    if (inString == "3")
    {
      displayType = "213_SSD1608a";
      EEPROM.write(0, 3);
      EEPROM.commit();
    }

    if (inString == "4")
    {
      displayType = "213_SSD1608b";
      EEPROM.write(0, 4);
      EEPROM.commit();
    }

    if (inString == "5")
    {
      displayType = "290_Waveshare";
      EEPROM.write(0, 5);
      EEPROM.commit();
    }
  }

  DEBUG_PRINT("    Display Type: ");
  DEBUG_PRINT(EEPROM.read(0));

  if (EEPROM.read(0) == 1)
  {
    displayType = "154_SSD1608";
  }
  if (EEPROM.read(0) == 2)
  {
    displayType = "154_SSD1681";
  }
  if (EEPROM.read(0) == 3)
  {
    displayType = "213_SSD1608a";
  }
  if (EEPROM.read(0) == 4)
  {
    displayType = "213_SSD1608b";
  }
  if (EEPROM.read(0) == 5)
  {
    displayType = "290_Waveshare";
  }

  if (EEPROM.read(0) == 255)
  {
    displayType = "No Display";
  }

  DEBUG_PRINT(" = ");
  DEBUG_PRINTLN(displayType);

  if (displayType != "No Display")
  {
    GxEPD2_GFX &display = getDisplay();

    display.init(0);
    display.fillScreen(GxEPD_WHITE);
    display.refresh(false); // full update

    display.setRotation(3);
    display.setTextSize(2);
    display.setTextColor(GxEPD_BLACK);
    showWhite();

  }
  return;
}


//******************************************************************************************
void showWhite()
//******************************************************************************************
{
  GxEPD2_GFX &display = getDisplay();
  // display.init(0);
  display.clearScreen();
  display.refresh(true); // true = partial update
  // display.fillRect(0, 0, display.width(), display.height(), GxEPD_WHITE);
  while (display.nextPage())
    ;
}


//******************************************************************************************
void showBlack()
//******************************************************************************************
{
  GxEPD2_GFX &display = getDisplay();
  display.clearScreen();
  display.refresh(true); // full update
  // display.fillRect(0, 0, 250, 122, GxEPD_BLACK);
  while (display.nextPage())
    ;
}

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



//******************************************************************************************
// Print a string on the display, centered, at a particular y value.
void centerPrint(String s, uint16_t y)
//******************************************************************************************
{
  GxEPD2_GFX &display = getDisplay();
  int16_t tx, ty;
  uint16_t tw, th;
  display.getTextBounds(s, 0, 0, &tx, &ty, &tw, &th);
  uint16_t utx = ((display.width() - tw) / 2) - tx;
  uint16_t uty = y; //((display.height() - th) / 2) - ty;
  display.setCursor(utx, uty);
  display.print(s);
}


//******************************************************************************************
void displayTitle(String title)
//******************************************************************************************
{
  GxEPD2_GFX &display = getDisplay();
  display.setPartialWindow(0, 0, display.width(), display.height());
  display.firstPage();
  display.fillScreen(GxEPD_WHITE);
  display.setTextSize(3);
  centerPrint(title, 1);
}

void displayUpdate(){
  GxEPD2_GFX &display = getDisplay();  
  do  {  } while (display.nextPage());
}


//******************************************************************************************
// A small copyright declaration at the bottom of the screen.
void displayCopyright()
//******************************************************************************************
{
  GxEPD2_GFX &display = getDisplay();
  display.setTextSize(1);
  int16_t tbx, tby;
  uint16_t tbw, tbh;

  display.getTextBounds("TEST", 0, 0, &tbx, &tby, &tbw, &tbh);

  int16_t lineHeight = tbh + 0.2 * tbh;
  int16_t offset = display.height() - 3 * lineHeight - tbh/2;

  centerPrint("DIGAME VCS Family", offset);
  centerPrint("(c) 2023, Digame Systems.", offset + lineHeight);
  centerPrint("All rights reserved.", offset + 2 * lineHeight);
  displayUpdate();
}


//******************************************************************************************
void displayRawText(String title, String s)
//******************************************************************************************
{
  displayTitle(title);
  GxEPD2_GFX &display = getDisplay();
  display.setTextSize(2);
  int16_t tbx, tby;
  uint16_t tbw, tbh;
  display.getTextBounds("TEST", 0, 0, &tbx, &tby, &tbw, &tbh);
  int16_t offset = 2 * tbh;
  uint16_t x = 0;
  uint16_t y = offset; 
  display.setCursor(x, y);
  display.print(s);
}

//******************************************************************************************
void displayCenteredText(String title, String s1, String s2, String s3, String s4, String s5, String s6, String s7, String s8)
//******************************************************************************************
{
  displayTitle(title);  
  GxEPD2_GFX &display = getDisplay();
  display.setTextSize(2);

  int16_t tbx, tby;
  uint16_t tbw, tbh;
  
  display.getTextBounds("TEXT", 0, 0, &tbx, &tby, &tbw, &tbh); // Get the height of a line of text
  
  int16_t offset = 2*tbh;
  int16_t lineHeight = tbh + 0.2*tbh;

  centerPrint (s1, offset);
  centerPrint (s2, offset + 1 * lineHeight);
  centerPrint (s3, offset + 2 * lineHeight);
  centerPrint (s4, offset + 3 * lineHeight);
  centerPrint (s5, offset + 4 * lineHeight);
  centerPrint (s6, offset + 5 * lineHeight);
  centerPrint (s7, offset + 6 * lineHeight);
  centerPrint (s8, offset + 7 * lineHeight);

}

//******************************************************************************************
void displayCenteredTextLarge(String title, String s1, String s2, String s3, String s4, String s5, String s6, String s7, String s8)
//******************************************************************************************
{
  displayTitle(title);  
  GxEPD2_GFX &display = getDisplay();
  display.setTextSize(3);

  int16_t tbx, tby;
  uint16_t tbw, tbh;
  
  display.getTextBounds("TEXT", 0, 0, &tbx, &tby, &tbw, &tbh); // Get the height of a line of text
  
  int16_t offset     = (int16_t)(2.5 * tbh);
  int16_t lineHeight = tbh + 0.2 * tbh;

  centerPrint (s1, offset);
  centerPrint (s2, offset + 1 * lineHeight);
  centerPrint (s3, offset + 2 * lineHeight);
  centerPrint (s4, offset + 3 * lineHeight);
  centerPrint (s5, offset + 4 * lineHeight);
  centerPrint (s6, offset + 5 * lineHeight);
  centerPrint (s7, offset + 6 * lineHeight);
  centerPrint (s8, offset + 7 * lineHeight);

}

//******************************************************************************************
void showValue(double v)
//******************************************************************************************
{
  GxEPD2_GFX &display = getDisplay();
  int digits = 0;

  display.setTextSize(5);
  display.setTextColor(GxEPD_BLACK);

  PrintString valueString;
  valueString.print(v, digits);

  int16_t tbx, tby;
  uint16_t tbw, tbh;
  display.getTextBounds(valueString, 0, 0, &tbx, &tby, &tbw, &tbh);

  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby; //+ tbh / 2; // y is base line!

  // show what happens, if we use the bounding box for partial window
  uint16_t wx = (display.width() - tbw) / 2;
  uint16_t wy = (display.height() - tbh) / 2; // / 2;

  display.setPartialWindow(wx, wy, tbw, tbh);
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(valueString);
  } while (display.nextPage());
  // See if we can put things back the way we found them
  display.setPartialWindow(0, 0, display.width(), display.height());
  display.firstPage();
}


//******************************************************************************************
void showPartialXY(String msg, int x, int y)
//******************************************************************************************
{
  GxEPD2_GFX &display = getDisplay();
  int digits = 0;

  display.setTextSize(3);
  display.setTextColor(GxEPD_BLACK);

  int16_t tbx, tby;
  uint16_t tbw, tbh;

  display.getTextBounds(msg, 0, 0, &tbx, &tby, &tbw, &tbh);
  display.setPartialWindow(x, y, tbw, tbh);
  display.firstPage();

  do
  {
    display.setCursor(x, y);
    display.print(msg);
  } while (display.nextPage());

  // Put things back the way we found them
  display.setPartialWindow(0, 0, display.width(), display.height());
  display.firstPage();
}
