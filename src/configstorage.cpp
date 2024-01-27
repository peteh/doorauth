#include "configstorage.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

bool formatLittleFS()
{
    log_w("need to format LittleFS: ");
    LittleFS.end();
    LittleFS.begin();
    log_i("Success: %d", LittleFS.format());
    return LittleFS.begin();
}

void saveSettings(Config &config)
{
    // Open file for writing
    File file = LittleFS.open(CONFIG_FILENAME, "w");
    if (!file)
    {
        log_e("Failed to create config file");
        return;
    }
    // Allocate a temporary JsonDocument
    // Don't forget to change the capacity to match your requirements.
    // Use arduinojson.org/assistant to compute the capacity.
    StaticJsonDocument<1024> doc;

    // Set the values in the document
    doc["restartCounter"] = config.restartCounter;
    doc["wifiDisconnectCounter"] = config.wifiDisconnectCounter;
    doc["mqttDisconnectCounter"] = config.mqttDisconnectCounter;
    doc["wifiSsid"] = config.wifiSsid;
    doc["wifiPassword"] = config.wifiPassword;
    doc["mqttServer"] = config.mqttServer;
    doc["mqttPort"] = config.mqttPort;
    doc["mqttUser"] = config.mqttUser;
    doc["mqttPassword"] = config.mqttPassword;

    // Serialize JSON to file
    if (serializeJson(doc, file) == 0)
    {
        log_e("Failed to write config to file");
    }

    // Close the file
    file.close();
}

void loadSettings(Config &config)
{
    // Open file for reading
    // TODO: check if file exists
    File file = LittleFS.open(CONFIG_FILENAME, "r");

    // Allocate a temporary JsonDocument
    // Don't forget to change the capacity to match your requirements.
    // Use arduinojson.org/v6/assistant to compute the capacity.
    StaticJsonDocument<1024> doc;

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
        log_e("Failed to read file, using default configuration");
    }

    // Copy values from the JsonDocument to the Config
    config.restartCounter = doc["restartCounter"] | config.restartCounter;
    config.wifiDisconnectCounter = doc["wifiDisconnectCounter"] | config.wifiDisconnectCounter;
    config.mqttDisconnectCounter = doc["mqttDisconnectCounter"] | config.mqttDisconnectCounter;

    if (doc.containsKey("wifiSsid"))
    {
        strncpy(config.wifiSsid, doc["wifiSsid"].as<const char *>(), sizeof(config.wifiSsid));
    }
    if (doc.containsKey("wifiPassword"))
    {
        strncpy(config.wifiPassword, doc["wifiPassword"].as<const char *>(), sizeof(config.wifiPassword));
    }
    if (doc.containsKey("mqttServer"))
    {
        strncpy(config.mqttServer, doc["mqttServer"].as<const char *>(), sizeof(config.mqttServer));
    }
    config.mqttPort = doc["mqttPort"] | config.mqttPort;
    if (doc.containsKey("mqttUser"))
    {
        strncpy(config.mqttUser, doc["mqttUser"].as<const char *>(), sizeof(config.mqttUser));
    }
    if (doc.containsKey("mqttPassword"))
    {
        strncpy(config.mqttPassword, doc["mqttPassword"].as<const char *>(), sizeof(config.mqttPassword));
    }

    // Close the file (Curiously, File's destructor doesn't close the file)
    file.close();
}