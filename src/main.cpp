#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "../include/wifi_credentials.h"

String key = "fee94c38aef9412e90fc11ee7754127f";
String stock = "NOK";
String url = "https://api.twelvedata.com/quote?symbol=" + stock + "&apikey=" + key;

void getData();

void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, passwd);
  Serial.println("Connecting");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected succesfully!");
}

void loop()
{
  if ((WiFi.status() == WL_CONNECTED))
  {
    HTTPClient client;
    client.begin(url);
    int httpStatus = client.GET();

    if (httpStatus)
    {
      String payload = client.getString();

      // Create a JSON document
      const size_t capacity = JSON_OBJECT_SIZE(1) + 1000; // Adjust based on your JSON structure
      DynamicJsonDocument doc(capacity);

      // Deserialize the JSON data
      DeserializationError error = deserializeJson(doc, payload);

      // Check for parsing errors
      if (error)
      {
        Serial.println("Failed to parse JSON: ");
        Serial.println(error.c_str());
        return;
      }

      // Extract the "name" field
      const float previous_close = doc["previous_close"];
      const float close = doc["close"];

      // Print the extracted value
      Serial.print(F("Previous close: "));
      Serial.println(previous_close);
      Serial.println("Close: ");
      Serial.println(close);
    }
    else
    {
      Serial.println("Error on request. Status: ");
      Serial.print(httpStatus);
    }
  }

  delay(100000);
}
