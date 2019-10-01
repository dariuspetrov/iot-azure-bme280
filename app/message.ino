#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <Wire.h>
#include "Seeed_BME280.h"

#if SIMULATED_DATA

void initSensor()
{
    // use SIMULATED_DATA, no sensor need to be inited
}

float readTemperature()
{
    return random(20, 30);
}

float readHumidity()
{
    return random(30, 40);
}

#else

static DHT dht(DHT_PIN, DHT_TYPE);
void initSensor()
{
    dht.begin();
}

float readTemperature()
{
    return dht.readTemperature();
}

float readHumidity()
{
    return dht.readHumidity();
}

#endif

bool readMessage(int messageId, char *payload)
{
    float temperature = bme280.getTemperature();
    float humidity = bme280.getHumidity();
    DynamicJsonDocument root(256);
    //JsonObject &root = jsonBuffer.createObject();
    root["deviceId"] = DEVICE_ID;
    root["messageId"] = messageId;
    bool temperatureAlert = false;

    // NAN is not the valid json, change it to NULL
    if (std::isnan(temperature))
    {
        root["temperature"] = NULL;
    }
    else
    {
        root["temperature"] = temperature;
        if (temperature > TEMPERATURE_ALERT)
        {
            temperatureAlert = true;
        }
    }

    if (std::isnan(humidity))
    {
        root["humidity"] = NULL;
    }
    else
    {
        root["humidity"] = humidity;
    }
    //root.printTo(payload, MESSAGE_MAX_LEN);
    //serializeJson(payload, Serial);
    serializeJson(root, Serial);
    return temperatureAlert;
}

void parseTwinMessage(char *message)
{
    StaticJsonDocument<256> root;
    //JsonObject &root = jsonBuffer.parseObject(message);
    auto error = deserializeJson(root, message);
    /*if (!root.success())
    {
        Serial.printf("Parse %s failed.\r\n", message);
        return;
    }*/
    // Upgrading to ArduinoJson6
    if(error){
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(error.c_str());
      return;
    }
    
    auto error1 = root["desired"]["interval"];
    if (error1)
    {
        interval = root["desired"]["interval"];
    }
    else if (root.containsKey("interval"))
    {
        interval = root["interval"];
    }
}
