/*
  Power management functions for the ESP32 platform
*/
#ifndef __DIGAME_POWER_MGT_H__
#define __DIGAME_POWER_MGT_H__

#include <digameDebug.h> // DEBUG_PRINT functions
#include <WiFi.h>        // WiFi stack
#include "driver/adc.h"  // ADC functions. (Allows us to turn off to save power.)
#include <esp_bt.h>      // Bluetooth control functions
#include <esp_wifi.h>

#define LOW_POWER 1
#define MEDIUM_POWER 2
#define FULL_POWER 3

int powerMode = FULL_POWER;

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  30        /* Time ESP32 will go to sleep (in seconds) */

/********************************************************************************
* Method to print the reason by which ESP32
* has been awakened 
 ********************************************************************************/

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

//*****************************************************************************
// Sleep the ESP32 for a while
void lightSleepMSec(unsigned long ms)
{
  unsigned long mS_TO_S_FACTOR = 1000;
  esp_sleep_enable_timer_wakeup(ms * mS_TO_S_FACTOR);
  esp_light_sleep_start();
}


//*****************************************************************************
// Deep Sleep the ESP32 for a while
void deepSleepMSec(unsigned long ms)
{
  unsigned long mS_TO_S_FACTOR = 1000;

  esp_sleep_enable_timer_wakeup( ms * uS_TO_S_FACTOR);
  DEBUG_PRINTLN("    Setup ESP32 to sleep for " + String(TIME_TO_SLEEP) + " Seconds");
  esp_sleep_enable_timer_wakeup(ms * mS_TO_S_FACTOR);
  DEBUG_PRINTLN("      Going into deep sleep now. Nitey nite.");
  delay(1000);
  Serial.flush(); 
  esp_deep_sleep_start();
}


//*****************************************************************************
// Disable Bluetooth, the ADC sub-system, WiFi and drop the CPU down to 40Mhz.
void setLowPowerMode()
{
  DEBUG_PRINT("    Switching to Low Power Mode... ");
  delay(500);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  btStop();
  adc_power_off(); // Depricated
  // adc_power_release();
  esp_wifi_stop();
  esp_bt_controller_disable();
  setCpuFrequencyMhz(40); // Slow down the CPU
  delay(500);
  /*DEBUG_PRINTLN("    Done. Low Power Mode Enabled.");
  delay(500);
  */
  powerMode = LOW_POWER;
}

//*****************************************************************************
// Disable Bluetooth, the ADC sub-system, and drop the CPU down to 80MHz.
// Leave WiFi active.
void setMediumPowerMode()
{
  DEBUG_PRINTLN("    Switching to Medium Power Mode... ");
  delay(500);
  btStop();
  // adc_power_off();
  esp_bt_controller_disable();
  setCpuFrequencyMhz(80); // Slow down the CPU
  // delay(500);
  // DEBUG_PRINTLN("    Done. Medium Power Mode Enabled.");
  delay(500);
  powerMode = MEDIUM_POWER;
}

//*****************************************************************************
// Run the CPU Flat out at 240MHz w/ WiFi active. Turn off and Bluetooth.
void setFullPowerMode()
{
  DEBUG_PRINTLN("    Switching to Full Power Mode... ");
  delay(500);
  setCpuFrequencyMhz(240); // Speed up the CPU
  btStop();
  esp_bt_controller_disable();
  // adc_power_on();          // Turn on the ADCs for WiFi
  adc_power_acquire();
  delay(500);
  DEBUG_PRINTLN("    Done. Full Power Mode Enabled. (240 MHz)");
  delay(500);
  powerMode = FULL_POWER;
}

#endif
