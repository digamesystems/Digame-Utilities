#ifndef __DIGAME_COUNTER_WEB_SERVER_H__
#define __DIGAME_COUNTER_WEB_SERVER_H__

/*
  Credit: This code based on some example code by Rui Santos and greatly extended for our
  application. He has asked that the following notice be included -- 

  "
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-web-server-microsd-card/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  "
*/

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include <SPIFFS.h>

#include <digameVersion.h> //for SW_VERSION.

#define debugUART Serial

#include <digameJSONConfig.h>
#include <digameNetwork.h>
#include <digameTime.h>
#include <digameDebug.h>


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
 
bool useOTAFlag = false;
bool clearCounterFlag = false;
bool resetFlag = false;
unsigned long upTimeMillis=0;
const char* http_username = "admin";
const char* http_password = "admin";

unsigned long msLastWebPageEventTime;


//*******************************************************************************************************
String processor(const String& var){
  
  //DEBUG_PRINTLN("Hello from processor");
  //DEBUG_PRINTLN(var);
  
  if(var == "config.lidarZone1Count") return F(String(config.lidarZone1Count).c_str());
  
  if(var == "config.deviceName") { 
    DEBUG_PRINTLN("config.deviceName: " + config.deviceName);
    return F(config.deviceName.c_str());
  }


  if(var == "STREAMING_ON"){if  (config.showDataStream == "true") return F("checked"); } 
  if(var == "STREAMING_OFF"){if (config.showDataStream == "false") return F("checked"); }   
  if(var == "MODEL")             return F(model.c_str());
  if(var == "MODEL_DESCRIPTION") return F(model_description.c_str()); 
  if(var == "SW_VERSION")        return F(SW_VERSION.c_str()); 
  if(var == "MAC_ADDRESS")       return F(getMACAddress().c_str()); 
  
  if(var == "config.heartbeatInterval")  return F(String(config.heartbeatInterval).c_str());
  if(var == "config.ssid")      return F(String(config.ssid).c_str());
  if(var == "config.password")  return F(String(config.password).c_str());
  if(var == "config.serverURL") return F(String(config.serverURL).c_str());

  if(var == "config.useMQTT")   return F(String(config.useMQTT).c_str());
  if(var == "config.mqttURL")      return F(String(config.mqttURL).c_str());
  if(var == "config.mqttPort")      return F(String(config.mqttPort).c_str());

  if(var == "config.loraBaseStationAddress") return F(String(config.loraBaseStationAddress).c_str());
  if(var == "config.loraAddress") return F(String(config.loraAddress).c_str());
  if(var == "config.loraNetworkID") return F(String(config.loraNetworkID).c_str());
  if(var == "config.loraBand") return F(String(config.loraBand).c_str());
  if(var == "config.loraSF") return F(String(config.loraSF).c_str());
  if(var == "config.loraBW") return F(String(config.loraBW).c_str());
  if(var == "config.loraCR") return F(String(config.loraCR).c_str());
  if(var == "config.loraPreamble") return F(String(config.loraPreamble).c_str());
  
  if(var == "config.lidarResidenceTime") return F(String(config.lidarResidenceTime).c_str());
  if(var == "config.lidarSmoothingFactor") return F(String(config.lidarSmoothingFactor).c_str());
  
  if(var == "config.lidarZone1Min") return F(String(config.lidarZone1Min).c_str());
  if(var == "config.lidarZone1Max") return F(String(config.lidarZone1Max).c_str());
  if(var == "config.lidarZone2Min") return F(String(config.lidarZone2Min).c_str());
  if(var == "config.lidarZone2Max") return F(String(config.lidarZone2Max).c_str());

  if(var == "config.logBootEvents") return F(String(config.logBootEvents).c_str());
  if(var == "config.logHeartBeatEvents") return F(String(config.logHeartBeatEvents).c_str());  
  if(var == "config.logVehicleEvents") return F(String(config.logVehicleEvents).c_str());
  if(var == "config.logRawData") return F(String(config.logRawData).c_str());
  if(var == "config.logLidarEvents") return F(String(config.logLidarEvents).c_str());
  if(var == "config.logDebugEvents") return F(String(config.logDebugEvents).c_str());
  
  if(var == "config.counterPopulation") return F(String(config.counterPopulation).c_str());
  if(var == "config.counterID") return F(String(config.counterID).c_str());
  

  if(var == "config.sens1Addr") return F(String(config.sens1Addr).c_str());
  if(var == "config.sens2Addr") return F(String(config.sens2Addr).c_str());
  if(var == "config.sens3Addr") return F(String(config.sens3Addr).c_str());
  if(var == "config.sens4Addr") return F(String(config.sens4Addr).c_str());
  
  if(var == "config.sens1Name") return F(String(config.sens1Name).c_str());
  if(var == "config.sens2Name") return F(String(config.sens2Name).c_str());
  if(var == "config.sens3Name") return F(String(config.sens3Name).c_str());
  if(var == "config.sens4Name") return F(String(config.sens4Name).c_str());

  if(var == "config.sens1MAC") return F(String(config.sens1MAC).c_str());
  if(var == "config.sens2MAC") return F(String(config.sens2MAC).c_str());
  if(var == "config.sens3MAC") return F(String(config.sens3MAC).c_str());
  if(var == "config.sens4MAC") return F(String(config.sens4MAC).c_str());
  
  if(var == "str1Count") return F(String(str1Count).c_str());
  if(var == "str2Count") return F(String(str2Count).c_str());
  if(var == "str3Count") return F(String(str3Count).c_str());
  if(var == "str4Count") return F(String(str4Count).c_str());
  
  return String();  
  
}

//*******************************************************************************************************
void redirectHome(AsyncWebServerRequest* request){
    
    DEBUG_PRINTLN("Hello from redirectHome");
    DEBUG_PRINTLN("Saving config...");

    saveConfiguration(filename,config); // Save any changes before redirecting home

    /*String RedirectUrl = "http://";
    if (ON_STA_FILTER(request)) {
      RedirectUrl += WiFi.localIP().toString();
    } else {
      RedirectUrl += WiFi.softAPIP().toString();
    }
    RedirectUrl += "/";
    
    request->redirect(RedirectUrl);
    */
    request->redirect("/");
}

void processQueryParam(AsyncWebServerRequest *request, String qParam, String *targetParam){
    //debugUART.println(qParam);
    if(request->hasParam(qParam)){
      //debugUART.println("found");
      AsyncWebParameter* p = request->getParam(qParam);

      //debugUART.print(p->value());
      //debugUART.print(" ");
      //debugUART.println(String(p->value()).length());


      if (String(p->value()).length() == 0) {
        //debugUART.println("*******BLANK ENTRY!*******");
        //debgugUART.println("...ignoring...");
      
      } else{
        *targetParam = String(p->value().c_str());
        targetParam->replace("%","_"); // Replace the template character. 
                                       // 'Might cause problems w/ some Passwords...
                                       // TODO: Think on this. Make '%' illegal in PW? 
      }
    }
}



//*******************************************************************************************************
//*******************************************************************************************************
void initWebServer() {

  Serial.println("  Initializing SPIFFS...");

  if(!SPIFFS.begin()){
    Serial.println("    File System Mount Failed!");
  } else {
    Serial.println("    SPIFFS up!");
  }

  msLastWebPageEventTime = millis(); // Initialize the web page event timer variable

  /*
 server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "Hello, world");
  });
  */

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    DEBUG_PRINTLN("Hello from /");
    //vTaskDelay((2000) / portTICK_PERIOD_MS);
    DEBUG_PRINTLN("Free Heap: " + String(ESP.getFreeHeap()));
    DEBUG_PRINTLN("server.on('/') callback");
    DEBUG_PRINTLN("WebTimer: " + String(millis() - msLastWebPageEventTime));
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    DEBUG_PRINTLN("authenticated");

    DEBUG_PRINT("Processor ADDR: ");
    char pAddr[80];
    sprintf(pAddr, " Processor ADDR: %p\n", processor);
    DEBUG_PRINTLN(pAddr);
    
    request->send(SPIFFS, "/index.htm", String(), false, processor);
    DEBUG_PRINTLN("index.htm served");
    DEBUG_PRINTLN();
    return;
  });

  server.on("/test",HTTP_GET, [](AsyncWebServerRequest *request){
    DEBUG_PRINTLN("Hello from /test");
    //vTaskDelay((2000) / portTICK_PERIOD_MS);
    request->send(SPIFFS, "/test_mode.htm", String(), false, processor);
    return;
  });


  server.on("/restart", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Restarting web server. Reload page in 60 sec...");
    restartWebServerFlag = true;
    DEBUG_PRINTLN("Setting Restart Web Server Flag...");
    return;
  });


  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SD, "/params.txt", "text/plain",true);
    redirectHome(request);
  });

  server.on("/eventlog", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SD, "/eventlog.txt", "text/plain", true);
    //redirectHome(request);
  });

  server.on("/cleareventlog", HTTP_GET, [](AsyncWebServerRequest *request){
    //request->send(SD, "/eventlog.txt", "text/plain",true);
    deleteFile("/eventlog.txt");
    saveTextFile("/eventlog.txt","");
    redirectHome(request);
  });

  server.on("/histograph", HTTP_GET, [](AsyncWebServerRequest *request){
    debugUART.println("GET /histograph");
    request->send(200, "text/plain", getDistanceHistogramChartString(config));
  });

  server.on("/histo", HTTP_GET, [](AsyncWebServerRequest *request){
    debugUART.println("GET /histo");
    request->send(200, "text/plain", getDistanceHistogramString());
  });
  
  server.on("/counterreset",HTTP_GET, [](AsyncWebServerRequest *request){
    count    = 0;
    inCount  = 0;
    outCount = 0;
    //clearLIDARDistanceHistogram();
    config.lidarZone1Count = "0"; 
    config.lidarZone2Count = "0";
    clearCounterFlag = true;
    redirectHome(request);
  });

  server.on("/generalparams",HTTP_GET, [](AsyncWebServerRequest *request){
    processQueryParam(request, "devname", &config.deviceName);
    
    String strStream;
    processQueryParam(request, "streaming", &strStream);
    debugUART.println(strStream);
    if (strStream == "ON"){config.showDataStream="true";}else{config.showDataStream="false";}

    config.logBootEvents = "";
    config.logHeartBeatEvents = "";
    config.logVehicleEvents = "";
    config.logLidarEvents ="";
    config.logRawData = "";
    config.logDebugEvents = "";
    config.useMQTT = "";

    processQueryParam(request, "logbootevents", &config.logBootEvents);
    processQueryParam(request, "logheartbeatevents", &config.logHeartBeatEvents);
    processQueryParam(request, "logvehicleevents", &config.logVehicleEvents);
    processQueryParam(request, "lograwdata", &config.logRawData);
    processQueryParam(request, "loglidarevents", &config.logLidarEvents);
    processQueryParam(request, "logdebugevents", &config.logDebugEvents);
    processQueryParam(request, "usemqtt", &config.useMQTT);
    processQueryParam(request, "mqtturl", &config.mqttURL);
    processQueryParam(request, "mqttport", &config.mqttPort);

    String strReboot;
    processQueryParam(request, "reboot", &strReboot);
    debugUART.println(strReboot);
    if (strReboot=="true"){
      resetFlag = true;
    }

    DEBUG_PRINTLN("Hello from generalparams");
    DEBUG_PRINTLN("1------")
    DEBUG_PRINTLN("config.deviceName: " + config.deviceName);
    DEBUG_PRINTLN("2-----");
    redirectHome(request);
     
  });

  server.on("/networkparams",HTTP_GET, [](AsyncWebServerRequest *request){
    processQueryParam(request, "heartbeatinterval", &config.heartbeatInterval);
    processQueryParam(request, "ssid", &config.ssid);
    processQueryParam(request, "password", &config.password);
    processQueryParam(request, "serverurl", &config.serverURL);  
    redirectHome(request);
  });

  server.on("/loraparams",HTTP_GET, [](AsyncWebServerRequest *request){
    processQueryParam(request, "basestationaddress", &config.loraBaseStationAddress);
    processQueryParam(request, "address", &config.loraAddress);
    processQueryParam(request, "networkid", &config.loraNetworkID);
    processQueryParam(request, "band", &config.loraBand);
    processQueryParam(request, "spreadingfactor", &config.loraSF);
    processQueryParam(request, "bandwidth", &config.loraBW);
    processQueryParam(request, "codingrate", &config.loraCR);
    processQueryParam(request, "preamble", &config.loraPreamble); 

    //initLoRa();
    //configureLoRa(config);
    redirectHome(request);
  });

  server.on("/lidarparams",HTTP_GET, [](AsyncWebServerRequest *request){
    processQueryParam(request, "counterid", &config.counterID);
    processQueryParam(request, "counterpopulation", &config.counterPopulation);
    processQueryParam(request, "smoothingfactor", &config.lidarSmoothingFactor);
    processQueryParam(request, "residencetime", &config.lidarResidenceTime);
    processQueryParam(request, "zone1min", &config.lidarZone1Min);
    processQueryParam(request, "zone1max", &config.lidarZone1Max);
    processQueryParam(request, "zone2min", &config.lidarZone2Min);
    processQueryParam(request, "zone2max", &config.lidarZone2Max);
    redirectHome(request);
  });


  server.on("/sensors",HTTP_GET, [](AsyncWebServerRequest *request){
    processQueryParam(request, "sens1name", &config.sens1Name);
    processQueryParam(request, "sens1addr", &config.sens1Addr);
    processQueryParam(request, "sens1mac",  &config.sens1MAC);
    
    processQueryParam(request, "sens2name", &config.sens2Name);
    processQueryParam(request, "sens2addr", &config.sens2Addr);
    processQueryParam(request, "sens2mac",  &config.sens2MAC);
    
    processQueryParam(request, "sens3name", &config.sens3Name);
    processQueryParam(request, "sens3addr", &config.sens3Addr);
    processQueryParam(request, "sens3mac",  &config.sens3MAC);
    
    processQueryParam(request, "sens4name", &config.sens4Name);
    processQueryParam(request, "sens4addr", &config.sens4Addr);
    processQueryParam(request, "sens4mac",  &config.sens4MAC);
    
    redirectHome(request);
  });

  server.on("/distance", HTTP_GET, [](AsyncWebServerRequest *request){
    //DEBUG_PRINTLN(lastDistanceMeasured);
    //request->send(200, "text/plain", String(millis()) + "," +\
    
    DEBUG_PRINTLN(String(lastDistanceMeasured));

    request->send(200, "text/plain", String(lastDistanceMeasured) + "," +\
                                      String(config.lidarZone1Count) + "," +\
                                      lastDistanceMeasured2 + "," +\
                                      String(config.lidarZone2Count));
    msLastWebPageEventTime = millis();
  });

  server.on("/counters", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", config.sens1Zone1+","+\
                                      config.sens2Zone1+","+\
                                      config.sens3Zone1+","+\
                                      config.sens4Zone1+","+\
                                      config.sens1Zone2+","+\
                                      config.sens2Zone2+","+\
                                      config.sens3Zone2+","+\
                                      config.sens4Zone2                                     
                                      );
    msLastWebPageEventTime = millis(); 
  });

  server.on("/uptime", HTTP_GET, [](AsyncWebServerRequest *request){
    String s = TimeToString(upTimeMillis/1000);
    //s = TimeToString(312847); // Testing. = (3 days 14 hours 54 min 7 seconds)
    //debugUART.print("/uptime: ");
    //debugUART.println(s);
    DEBUG_PRINTLN("UPTIME: " + s); 
    request->send(200, "text/plain", s);  // Report in seconds
    msLastWebPageEventTime = millis(); 
  });


  //server.serveStatic("/", SD, "/");
  server.serveStatic("/", SPIFFS, "/");
 

  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  DEBUG_LOG("HTTP server started.");
  restartWebServerFlag=false;

}


void restartWebServer(){
  DEBUG_PRINTLN("Shutting down...");
  server.end();
  delay(2000);
  DEBUG_PRINTLN("Restarting...");
  initWebServer();
}


#endif