/* digameNetwork.h
 *
 *  Functions for logging into a wifi network and reporting the device's
 *  MAC address, and POSTING a JSON message to a server.
 *
 *  Copyright 2021, Digame Systems. All rights reserved.
 */

#ifndef __DIGAME_NETWORK_H__
#define __DIGAME_NETWORK_H__

#include <WiFi.h>             // WiFi stack
#include <HTTPClient.h>       // To post to the ParkData Server
#include <digameJSONConfig.h> // for Config struct that holds network credentials
#include <digameTime.h>       // for getRTCTime()

#define debugUART Serial

// Globals
bool wifiConnected = false;
unsigned long msLastConnectionAttempt; // Timer value of the last time we tried to connect to the wifi.
HTTPClient http;                       // The class we use to POST messages
unsigned long msLastPostTime;          // Timer value of the last time we did an http POST.

//*****************************************************************************
// Return the device's MAC address as a String
String getMACAddress()
{

    byte mac[6];
    String retString;

    WiFi.macAddress(mac);

    char buffer[3];
    for (int i = 0; i<5; i++){
      sprintf(buffer, "%02x", mac[i]);
      retString = String(retString + buffer + ":");
    }
    sprintf(buffer, "%02x", mac[5]);
    retString = String(retString + buffer);

    /*
    retString = String(String(mac[0], HEX) + ":");
    retString = String(retString + String(mac[1], HEX) + ":");
    retString = String(retString + String(mac[2], HEX) + ":");
    retString = String(retString + String(mac[3], HEX) + ":");
    retString = String(retString + String(mac[4], HEX) + ":");
    retString = String(retString + String(mac[5], HEX));
    */

    return retString;
}

//*****************************************************************************
// Return the last four digits of the device's MAC address as a String
String getShortMACAddress()
{
    byte mac[6];
    String retString;

    WiFi.macAddress(mac);
    // DEBUG_PRINTLN(mac);
    char buffer[3];
    sprintf(buffer, "%02x", mac[4]);
    retString = String(retString + buffer);
    sprintf(buffer, "%02x", mac[5]);
    retString = String(retString + buffer);

    return retString;
}
//*****************************************************************************
// Enable WiFi and log into the network
bool enableWiFi(Config config)
{
    String ssid = config.ssid;
    String password = config.password;
    DEBUG_PRINTLN("  Starting WiFi");
    DEBUG_PRINTLN("    SSID: " + ssid);
    DEBUG_PRINTLN("    Password: " + password);
    DEBUG_PRINTLN("    Server URL: " + config.serverURL);
    
    WiFi.disconnect();   // Disconnect the network
    WiFi.mode(WIFI_OFF); // Switch WiFi off

    //setCpuFrequencyMhz(80); //TODO: Figure out a better place for this.
    
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    WiFi.mode(WIFI_STA); // Station mode
    vTaskDelay(250 / portTICK_PERIOD_MS);

    String hostName = config.deviceName;
    hostName.replace(" ", "_");
    WiFi.setHostname(hostName.c_str());         // define hostname
    WiFi.begin(ssid.c_str(), password.c_str()); // Log in

    bool timedout = false;
    unsigned long wifiTimeout = 60000;
    unsigned long tstart = millis();
    DEBUG_PRINT("    ")
    while (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        DEBUG_PRINT(".");
        if ((millis() - tstart) > wifiTimeout)
        {
            DEBUG_PRINTLN("    Timeout attempting WiFi connection.");
            timedout = true;
            break;
        }
    }

    msLastConnectionAttempt = millis(); // Timer value of the last time we tried to connect to the wifi.

    if (timedout)
    {
        wifiConnected = false;
        return false;
    }
    else
    {
        DEBUG_PRINTLN("");
        DEBUG_PRINTLN("    WiFi connected.");
        DEBUG_PRINT(  "    IP address: ");
        DEBUG_PRINTLN(WiFi.localIP());
        wifiConnected = true;

        // Your Domain name with URL path or IP address with path
        // http.begin(config.serverURL);

        return true;
    }
}

bool initWiFi(Config config)
{
    return enableWiFi(config);
}

//*****************************************************************************
void disableWiFi()
{
    WiFi.setSleep(true);
    WiFi.disconnect(true);  // Disconnect from the network
    WiFi.mode(WIFI_OFF);    // Switch WiFi off
    //setCpuFrequencyMhz(40); // Drop cpu down to conserve power
    DEBUG_PRINTLN("");
    DEBUG_PRINTLN("WiFi disconnected!");
    wifiConnected = false;
}

//*****************************************************************************
// Save a single JSON message to the server. TODO: Better option for retries.
//*****************************************************************************
bool postJSON(String jsonPayload, Config config)
{

    if (WiFi.status() != WL_CONNECTED) // If we're not connected to the wifi, try to connect.
    {
        DEBUG_PRINTLN("WiFi not connected.");
        DEBUG_PRINTLN("Attempting to connect to WiFi."); 
        DEBUG_LOG("WiFi not connected. Attempting to connect to WiFi.");
        if (enableWiFi(config) == false)
        {
            DEBUG_PRINTLN("Unable to connect to WiFi.");
            DEBUG_LOG("Unable to connect to WiFi. - Exiting postJSON().");            
            return false; // If we can't connect to the WiFi, we can't POST.
        };
        DEBUG_PRINTLN("WiFi connected. ");
        DEBUG_LOG("WiFi connected. ");
        restartWebServerFlag = true; // If we just connected to the wifi, we need to restart the web server. 
    }

    unsigned long t1 = millis();

    // Create the http client object and set the headers to JSON
    http.begin(config.serverURL);
    http.addHeader("Content-Type", "application/json");

    String strResult =      "Posting JSON to server...\n"; //+ config.serverURL + "\n";
    strResult = strResult + " RTC Time: " + getRTCTime() + "\n";
    strResult = strResult + " Payload length: " + String(jsonPayload.length()) + "\n";
    //strResult = strResult + "HTTP POST Begin Time:   " + String(millis() - t1) + "\n";
    
    t1 = millis();
    int httpResponseCode = http.POST(jsonPayload);

    strResult = strResult + " POST Time:     " + String(millis() - t1) + "\n";
    strResult = strResult + " Response code: " + String(httpResponseCode) + "\n";
    
    if (httpResponseCode != HTTP_CODE_OK)
    {
        strResult = strResult + "***** ERROR *****\n";
        if (httpResponseCode == 404){
            strResult = strResult + "404: Not Found\n";
        } else {
            strResult = strResult + http.errorToString(httpResponseCode) + "\n";
        }

        delay(3000); // Wait a bit before trying again.
    }

    if (config.showDataStream == "false")
    {
        DEBUG_PRINTLN(strResult);
        DEBUG_PRINTLN("Message to Server: ");
        DEBUG_PRINTLN(jsonPayload);
    }

    DEBUG_LOG(strResult);

    // Free resources
    http.end();

    msLastPostTime = millis(); // Log the time of the last post attempt
        
    return (httpResponseCode == HTTP_CODE_OK);
    
}

#endif //__DIGAME_NETWORK_H__
