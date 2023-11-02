#ifndef __DIGAME_JSON_CONFIG_H__
#define __DIGAME_JSON_CONFIG_H__

// Is this the right place for all this SD card-specific stuff? TODO: refactor.
#define ADAFRUIT_EINK_SD true // Some Adafruit Eink Displays have an integrated SD card so we don't need a separate module
#define debugUART Serial

#include <SPI.h> // SPI bus functions to talk to the SD Card
#include <SD.h>  // SD file handling

#if ADAFRUIT_EINK_SD
#define SD_CS 14 // SD card chip select
#else
#define SD_CS 4
#endif

#include <ArduinoJson.h>

/* Big TODO: digameJSONConfig wants to be a class! Figure out he right way to slice this.

   - All the elements in the Config struct below could get folded in.
   - A number of the simpler file i/o routines could get put into a digameFile class which
     could be declared in the config class.
   - Keep init/load/save/Config in the class.

   This'll have implications, since a number of the digame Apps are relying on a global
   Config struct. After refactoring, we could be handing around a reference to a config
   class instead.

   JMP 6/17/22

*/

// Counter values for each counter TODO:add to config.
unsigned long count = 0; // The number of vehicle events recorded
unsigned long inCount = 0;
unsigned long outCount = 0;
String str1Count = "0";
String str2Count = "0";
String str3Count = "0";
String str4Count = "0";
String strTotal = "0";
String lastDistanceMeasured = "0";
String lastDistanceMeasured2 = "0";

bool restartWebServerFlag = false; // Flag to restart the web server after a config change.

// Our configuration structure.
//
struct Config
{
  String configured = "false"; // Factory configuration flag. Set to "true" after cal/test.
                               // Used to branch the UI for testing at MFG time.

  // Parameters found in PARAMS.TXT on the SD card.
  // Fails over to these values if not found.

  String deviceName = "Digame Systems"; //"YOUR_DEVICE_NAME"; //

  // Network:
  String heartbeatInterval = "3600"; // Once an hour by default
  String ssid = "Bighead";           // "YOUR_SSID";     // Wireless network name.
  String password = "billgates";     // "YOUR_PASSWORD"; // Network PW

  // String serverURL           = "https://trailwaze.info/zion/lidar_sensor_import.php"; // The ParkData server URL
  String serverURL = "http://199.21.201.53/trailwaze/zion/lidar_sensor_import.php"; // http server. Faster!

  // MQTT - Experimental. A second reporting channel in addition to posts to the serverURL above
  String useMQTT = "";
  String mqttURL = "test.mosquitto.org";
  String mqttPort = "1883";

  
  // Debugging
  String showDataStream = "false";

  // Logging to SD card
  String logBootEvents = "";
  String logHeartBeatEvents = "";
  String logVehicleEvents = "";
  String logRawData = "";
  String logLidarEvents = "";
  String logDebugEvents = "";

  // Counters
  String counterPopulation = "1";
  String counterID = "1";

  // LoRa:
  String loraBaseStationAddress = "1";
  String loraAddress = "10";
  String loraNetworkID = "7";
  String loraBand = "915000000";
  String loraSF = "10";
  String loraBW = "7";
  String loraCR = "1";
  String loraPreamble = "7";

  // LIDAR Parameters:
  String lidarDetectionAlgorithm = "Threshold";
  String lidarUpdateInterval = "10";
  String lidarSmoothingFactor = "0.6";
  String lidarResidenceTime = "5";
  String lidarZone1Min = "0";
  String lidarZone1Max = "300";
  String lidarZone2Min = "400";
  String lidarZone2Max = "700";
  String lidarZone1Count = "0";
  String lidarZone2Count = "0";

  // Sensors a basestation can see:
  String sens1Addr = "10";
  String sens1Name = "Sensor 1";
  String sens1MAC = "aa:bb:cc:dd:ee:01";
  String sens1Zone1 = "0";
  String sens1Zone2 = "0";

  String sens2Addr = "11";
  String sens2Name = "Sensor 2";
  String sens2MAC = "aa:bb:cc:dd:ee:02";
  String sens2Zone1 = "0";
  String sens2Zone2 = "0";

  String sens3Addr = "12";
  String sens3Name = "Sensor 3";
  String sens3MAC = "aa:bb:cc:dd:ee:03";
  String sens3Zone1 = "0";
  String sens3Zone2 = "0";

  String sens4Addr = "13";
  String sens4Name = "Sensor 4";
  String sens4MAC = "aa:bb:cc:dd:ee:04";
  String sens4Zone1 = "0";
  String sens4Zone2 = "0";

  // String displayType = "SSD1608"; // so we can switch displays at run time based on the config file.
};

// Config config;

const char *filename = "/params.txt"; // <- SD library uses 8.3 filenames
const char *histoFilename = "/histo.csv";

bool initSDCard();
void loadConfiguration(const char *filename, Config &config);
void saveConfiguration(const char *filename, Config &config);
void printFile(const char *filename);

// Test for SD Card, Load Config from file
bool initJSONConfig(const char *filename);

//****************************************************************************************
// See if the card is present and can be initialized.
bool initSDCard()
{

  Serial.println("  Initializing SD Card...");

  if (!SD.begin(SD_CS))
  {
    Serial.println("    Card Mount Failed");
    return false;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println("    No SD card attached");
    return false;
  }

  Serial.print("    SD Card Type: ");
  if (cardType == CARD_MMC)
  {
    Serial.println("MMC");
  }
  else if (cardType == CARD_SD)
  {
    Serial.println("SDSC");
  }
  else if (cardType == CARD_SDHC)
  {
    Serial.println("SDHC");
  }
  else
  {
    Serial.println("UNKNOWN");
  }
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("    SD Card Size: %lluMB\n", cardSize);
  return true;
}

//****************************************************************************************
// If we add parameters to the config struct, the params.txt may not have an entry for new
// fields. - In those cases, just use the default values from the config struct.
void initConfigEntry(String *target, String value)
{

  if (value.length() > 0)
  {
    // debugUART.print("Entry has length! Setting value to... ");
    // debugUART.println(value);
    *target = value;
  }
  else
  {
    // debugUART.print("Entry has NO length. Using default value of... ");
    // debugUART.println(*target);
  }
}

//****************************************************************************************
// Loads the configuration from a file
void loadConfiguration(const char *filename, Config &config)
{
  // Open file for reading
  debugUART.println("    Opening file for read...");
  File file = SD.open(filename);
  // saveConfiguration(filename, config);
  if (!file)
  {
    Serial.println(F("    Failed to open file. Creating it from default parameters."));
    saveConfiguration(filename, config);
    Serial.println("    Trying again...");
    debugUART.println("    Opening file for read...");
    file = SD.open(filename);
  }

  if (!file)
  {
    debugUART.println("    Load parameters failed!");
    return;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use https://arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<2048> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);

  if (error)
  {
    Serial.println(F("    Failed to parse file, using default configuration"));
    file.close();
    return;
  }

  // initConfigEntry((const char *)doc["flubart"], &config.flubart);
  // initConfigEntry((const char *)doc["name"], &config.deviceName);

  // Copy values from the JsonDocument to the Config
  initConfigEntry(&config.deviceName, (const char *)doc["name"]);
  initConfigEntry(&config.heartbeatInterval, (const char *)doc["network"]["heartbeatInterval"]);

  initConfigEntry(&config.ssid, (const char *)doc["network"]["ssid"]);
  initConfigEntry(&config.password, (const char *)doc["network"]["password"]);
  initConfigEntry(&config.serverURL, (const char *)doc["network"]["serverURL"]);

  initConfigEntry(&config.useMQTT, (const char *)doc["mqtt"]["useMQTT"]);
  initConfigEntry(&config.mqttURL, (const char *)doc["mqtt"]["mqttURL"]);
  initConfigEntry(&config.mqttPort, (const char *)doc["mqtt"]["mqttPort"]);

  initConfigEntry(&config.loraBaseStationAddress, (const char *)doc["lora"]["baseStationAddress"]);
  initConfigEntry(&config.loraAddress, (const char *)doc["lora"]["address"]);
  initConfigEntry(&config.loraNetworkID, (const char *)doc["lora"]["networkID"]);
  initConfigEntry(&config.loraBand, (const char *)doc["lora"]["band"]);
  initConfigEntry(&config.loraSF, (const char *)doc["lora"]["spreadingFactor"]);

  initConfigEntry(&config.loraBW, (const char *)doc["lora"]["bandwidth"]);
  initConfigEntry(&config.loraCR, (const char *)doc["lora"]["codingRate"]);
  initConfigEntry(&config.loraPreamble, (const char *)doc["lora"]["preamble"]);

  initConfigEntry(&config.lidarDetectionAlgorithm, (const char *)doc["lidar"]["detectionAlgorithm"]);
  initConfigEntry(&config.lidarUpdateInterval, (const char *)doc["lidar"]["updateInterval"]);
  initConfigEntry(&config.lidarSmoothingFactor, (const char *)doc["lidar"]["smoothingFactor"]);
  initConfigEntry(&config.lidarResidenceTime, (const char *)doc["lidar"]["residenceTime"]);
  initConfigEntry(&config.lidarZone1Min, (const char *)doc["lidar"]["zone1Min"]);
  initConfigEntry(&config.lidarZone1Max, (const char *)doc["lidar"]["zone1Max"]);
  initConfigEntry(&config.lidarZone2Min, (const char *)doc["lidar"]["zone2Min"]);
  initConfigEntry(&config.lidarZone2Max, (const char *)doc["lidar"]["zone2Max"]);

  initConfigEntry(&config.lidarZone1Count, "0"); //(const char *)doc["lidar"]["zone1Count"]);
  initConfigEntry(&config.lidarZone2Count, "0"); //(const char *)doc["lidar"]["zone2Count"]);

  initConfigEntry(&config.logBootEvents, (const char *)doc["log"]["bootEvents"]);
  initConfigEntry(&config.logHeartBeatEvents, (const char *)doc["log"]["heartBeatEvents"]);
  initConfigEntry(&config.logVehicleEvents, (const char *)doc["log"]["vehicleEvents"]);
  initConfigEntry(&config.logRawData, (const char *)doc["log"]["rawData"]);
  initConfigEntry(&config.logLidarEvents, (const char *)doc["log"]["lidar"]);
  initConfigEntry(&config.logDebugEvents, (const char *)doc["log"]["debug"]);

  initConfigEntry(&config.counterPopulation, (const char *)doc["counter"]["population"]);
  initConfigEntry(&config.counterID, (const char *)doc["counter"]["id"]);

  initConfigEntry(&config.sens1Name, (const char *)doc["sensor"]["1"]["name"]);
  initConfigEntry(&config.sens1Addr, (const char *)doc["sensor"]["1"]["addr"]);
  initConfigEntry(&config.sens1MAC, (const char *)doc["sensor"]["1"]["mac"]);
  initConfigEntry(&config.sens2Name, (const char *)doc["sensor"]["2"]["name"]);
  initConfigEntry(&config.sens2Addr, (const char *)doc["sensor"]["2"]["addr"]);
  initConfigEntry(&config.sens2MAC, (const char *)doc["sensor"]["2"]["mac"]);
  initConfigEntry(&config.sens3Name, (const char *)doc["sensor"]["3"]["name"]);
  initConfigEntry(&config.sens3Addr, (const char *)doc["sensor"]["3"]["addr"]);
  initConfigEntry(&config.sens3MAC, (const char *)doc["sensor"]["3"]["mac"]);
  initConfigEntry(&config.sens4Name, (const char *)doc["sensor"]["4"]["name"]);
  initConfigEntry(&config.sens4Addr, (const char *)doc["sensor"]["4"]["addr"]);
  initConfigEntry(&config.sens4MAC, (const char *)doc["sensor"]["4"]["mac"]);

  // Close the file (Curiously, File's destructor doesn't close the file)
  file.close();
  printFile(filename);
}

//****************************************************************************************
// Saves the configuration to a file
void saveConfiguration(const char *filename, Config &config)
{

  DEBUG_PRINTLN("Hello from saveConfiguration()");

  SD.remove(filename);

  // Open file for writing
  if (config.showDataStream == "false")
  {
    // debugUART.println("    Opening file for write...");
  }

  File file = SD.open(filename, FILE_WRITE);
  if (!file)
  {
    Serial.println(F("    Failed to create file!"));
    return;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use https://arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<2048> doc;

  // Copy values from the Config struct to the JsonDocument
  doc["name"] = config.deviceName;

  doc["network"]["heartbeatInterval"] = config.heartbeatInterval;
  doc["network"]["ssid"] = config.ssid;
  doc["network"]["password"] = config.password;
  doc["network"]["serverURL"] = config.serverURL;

  doc["mqtt"]["useMQTT"] = config.useMQTT;
  doc["mqtt"]["mqttURL"] = config.mqttURL;
  doc["mqtt"]["mqttPort"] = config.mqttPort;

  doc["lora"]["baseStationAddress"] = config.loraBaseStationAddress;
  doc["lora"]["address"] = config.loraAddress;
  doc["lora"]["networkID"] = config.loraNetworkID;
  doc["lora"]["band"] = config.loraBand;
  doc["lora"]["spreadingFactor"] = config.loraSF;
  doc["lora"]["bandwidth"] = config.loraBW;
  doc["lora"]["codingRate"] = config.loraCR;
  doc["lora"]["preamble"] = config.loraPreamble;

  doc["lidar"]["detectionAlgorithm"] = config.lidarDetectionAlgorithm;
  doc["lidar"]["updateInterval"] = config.lidarUpdateInterval;
  doc["lidar"]["smoothingFactor"] = config.lidarSmoothingFactor;
  doc["lidar"]["residenceTime"] = config.lidarResidenceTime;
  doc["lidar"]["zone1Min"] = config.lidarZone1Min;
  doc["lidar"]["zone1Max"] = config.lidarZone1Max;
  doc["lidar"]["zone2Min"] = config.lidarZone2Min;
  doc["lidar"]["zone2Max"] = config.lidarZone2Max;
  doc["lidar"]["zone1Count"] = config.lidarZone1Count;
  doc["lidar"]["zone2Count"] = config.lidarZone2Count;

  doc["log"]["bootEvents"] = config.logBootEvents;
  doc["log"]["heartBeatEvents"] = config.logHeartBeatEvents;
  doc["log"]["vehicleEvents"] = config.logVehicleEvents;
  doc["log"]["rawData"] = config.logRawData;
  doc["log"]["lidar"] = config.logLidarEvents;
  doc["log"]["debug"] = config.logDebugEvents;
  

  doc["counter"]["population"] = config.counterPopulation;
  doc["counter"]["id"] = config.counterID;

  doc["sensor"]["1"]["name"] = config.sens1Name;
  doc["sensor"]["1"]["addr"] = config.sens1Addr;
  doc["sensor"]["1"]["mac"] = config.sens1MAC;
  doc["sensor"]["2"]["name"] = config.sens2Name;
  doc["sensor"]["2"]["addr"] = config.sens2Addr;
  doc["sensor"]["2"]["mac"] = config.sens2MAC;
  doc["sensor"]["3"]["name"] = config.sens3Name;
  doc["sensor"]["3"]["addr"] = config.sens3Addr;
  doc["sensor"]["3"]["mac"] = config.sens3MAC;
  doc["sensor"]["4"]["name"] = config.sens4Name;
  doc["sensor"]["4"]["addr"] = config.sens4Addr;
  doc["sensor"]["4"]["mac"] = config.sens4MAC;

  // doc["displayType"] = config.displayType;

  // Serialize JSON to file
  if (config.showDataStream == "false")
  {
    // debugUART.println("    Writing file...");
  }

  if (serializeJson(doc, file) == 0)
  {
    // Serial.println(F("    Failed to write to file!"));
  }

  // Close the file
  if (config.showDataStream == "false")
  {
    // debugUART.println("*   Done saving parameters.");
  }

  file.close();
}

//****************************************************************************************
void deleteFile(const char *filename)
{
  // Delete existing file, otherwise the information is appended to the file
  // debugUART.println("    Erasing old file...");
  SD.remove(filename);
}

//****************************************************************************************
// Save some text to a file
void saveTextFile(const char *filename, String contents)
{

  // debugUART.println(initSDCard());
  // debugUART.print("  Saving data to: ");
  // debugUART.println(filename);

  deleteFile(filename);

  // Open file for writing
  // debugUART.println("    Opening file for write...");
  File file = SD.open(filename, FILE_WRITE);

  if (!file)
  {
    // debugUART.println(F("    Failed to create file!"));
    return;
  }

  // debugUART.println("    Writing file...");
  file.print(contents);

  // Close the file
  // debugUART.println("  Done.");
  file.close();
}

//****************************************************************************************
// Save some text to a file
void appendTextFile(const char *filename, String contents)
{

  // Open file for writing
  //debugUART.println("    Opening file for write...");
  File file = SD.open(filename, FILE_APPEND);

  if (!file)
  {
    debugUART.println(F("    Failed to create file!"));
    return;
  }

  //debugUART.println("    Writing file...");
  file.print(contents);

  // Close the file
  file.close();
}

//****************************************************************************************
// Prints the content of a file to the Serial
void printFile(const char *filename)
{
  // Open file for reading
  File file = SD.open(filename);
  if (!file)
  {
    Serial.println(F("Failed to read file"));
    return;
  }

  // Extract each characters by one by one
  Serial.print("    ");
  while (file.available())
  {
    Serial.print((char)file.read());
  }
  Serial.println();

  // Close the file
  file.close();
}

//****************************************************************************************
//
bool initJSONConfig(const char *filename, Config &config)
{

  bool sdCardPresent = false;
  debugUART.print("  Testing for SD Card Module... ");
  sdCardPresent = initSDCard();
  if (sdCardPresent)
  {
    debugUART.println("  Module found. (Reading parameters from SD Card.)");
    loadConfiguration(filename, config);
    return true;
  }
  else
  {
    debugUART.println("  ERROR! Module NOT found. (Parameters set to default values.)");
    return false;
  }
}

// Performance issue?
// ------------------
//
// File is an unbuffered stream, which is not optimal for ArduinoJson.
// See: https://arduinojson.org/v6/how-to/improve-speed/

// See also
// --------
//
// https://arduinojson.org/ contains the documentation for all the functions
// used above. It also includes an FAQ that will help you solve any
// serialization or deserialization problem.
//
// The book "Mastering ArduinoJson" contains a case study of a project that has
// a complex configuration with nested members.
// Contrary to this example, the project in the book uses the SPIFFS filesystem.
// Learn more at https://arduinojson.org/book/
// Use the coupon code TWENTY for a 20% discount ❤❤❤❤❤

#endif