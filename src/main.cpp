#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GC9A01A.h>
#include <TimeLib.h>
#include "../include/wifi_credentials.h"

// DISPLAY PINS
#define TFT_CS 22
#define TFT_DC 16
// RGB PINS
#define PIN_RED 26
#define PIN_GREEN 27
#define PIN_BLUE 25
// TEXT SIZES
#define SIZE_BIG 5
#define SIZE_MEDIUM 3
#define SIZE_SMALL 1
// TIMEZONE
#define CURRENT_TIMEZONE 7200 // Helsinki (UTC+2) (7200 seconds = 2 hours)

// CREATING TFT OBJECT
Adafruit_GC9A01A tft(TFT_CS, TFT_DC);

// API SETUP
String key = "YOUR API KEY HERE"; // SET YOUR API KEY INSIDE QUOTES
String stock = "TSLA";            // SET THE STOCK SYMBOL INSIDE QUOTES
String url = "https://api.twelvedata.com/quote?symbol=" + stock + "&apikey=" + key;

// FUNCTIONS
void colorRed();
void colorGreen();
int centerX(int text_size, String text);

int x_pos_text;

void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, passwd);

  // FOR RGB MODULE
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);

  tft.begin();
  tft.setRotation(4);
  tft.fillScreen(GC9A01A_BLACK);
  tft.setTextColor(GC9A01A_WHITE);
  tft.setTextSize(SIZE_MEDIUM);

  x_pos_text = centerX(SIZE_MEDIUM, "Connecting");
  tft.setCursor(x_pos_text, 70);
  tft.print("Connecting");
  x_pos_text = centerX(SIZE_MEDIUM, "to WIFI");
  tft.setCursor(x_pos_text, 100);
  tft.print("to WIFI");

  // WHILE CONNECTING
  int loading_bar = 40;

  while (WiFi.status() != WL_CONNECTED)
  {
    tft.setCursor(loading_bar, 140);
    tft.print(".");
    loading_bar += 10;

    // RESET BAR
    if (loading_bar == 180)
    {
      loading_bar = 40;
      tft.fillScreen(GC9A01A_BLACK);
      x_pos_text = centerX(SIZE_MEDIUM, "Connecting");
      tft.setCursor(x_pos_text, 70);
      tft.print("Connecting");
      x_pos_text = centerX(SIZE_MEDIUM, "to WIFI");
      tft.setCursor(x_pos_text, 100);
      tft.print("to WIFI");
    }
    delay(500);
  }

  tft.fillScreen(GC9A01A_BLACK);
  x_pos_text = centerX(SIZE_MEDIUM, "CONNECTED");
  tft.setCursor(x_pos_text, 100);
  tft.print("Connected");
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

      const size_t capacity = JSON_OBJECT_SIZE(1) + 1000; // Adjust based on your JSON structure
      DynamicJsonDocument doc(capacity);
      DeserializationError error = deserializeJson(doc, payload);

      if (error)
      {
        Serial.println("Failed to parse JSON: ");
        Serial.println(error.c_str());
        return;
      }

      // EXTRACTING CONTENT
      float change_float = doc["change"];
      float open_float = doc["open"];
      const char *symbol = doc["symbol"];
      const char *name = doc["name"];
      int time = doc["timestamp"];
      bool market_status = doc["is_market_open"];

      time += CURRENT_TIMEZONE;

      String open = String(open_float);
      String change = String(change_float);
      String market_open = "OPEN";
      String market_closed = "CLOSED";
      String timestamp;

      // TURNING UNIX TIMESTAMP TO READABLE DATE
      tmElements_t timeinfo;
      char buffer[20];
      breakTime(time, timeinfo);

      sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
              timeinfo.Year + 1970, timeinfo.Month, timeinfo.Day,
              timeinfo.Hour, timeinfo.Minute, timeinfo.Second);

      timestamp = String(buffer);
      open += "$";
      change += "%";

      // PRINTING VALUES TO DISPLAY
      tft.fillScreen(GC9A01A_BLACK);
      tft.setTextColor(GC9A01A_WHITE);

      // SYMBOL
      tft.setTextSize(SIZE_BIG);
      x_pos_text = centerX(SIZE_BIG, symbol);
      tft.setCursor(x_pos_text, 50);
      tft.print(symbol);
      // OPEN
      tft.setTextSize(SIZE_BIG);
      x_pos_text = centerX(SIZE_BIG, open);
      tft.setCursor(x_pos_text, 100);
      tft.print(open);
      // CHANGE
      tft.setTextSize(SIZE_MEDIUM);
      x_pos_text = centerX(SIZE_MEDIUM, change);
      tft.setCursor(x_pos_text, 150);

      if (change_float > 0)
      {
        colorGreen();
        tft.setTextColor(GC9A01A_GREEN);
        tft.print(change);
      }
      else if (change_float < 0)
      {
        colorRed();
        tft.setTextColor(GC9A01A_RED);
        tft.print(change);
      }

      // NAME
      tft.setTextColor(GC9A01A_WHITE);
      tft.setTextSize(SIZE_SMALL);
      x_pos_text = centerX(SIZE_SMALL, name);
      tft.setCursor(x_pos_text, 190);
      tft.print(name);
      // TIMESTAMP
      tft.setTextColor(GC9A01A_WHITE);
      tft.setTextSize(SIZE_SMALL);
      x_pos_text = centerX(SIZE_SMALL, timestamp);
      tft.setCursor(x_pos_text, 200);
      tft.print(timestamp);

      // MARKET STATUS
      tft.setTextSize(SIZE_SMALL);
      if (market_status)
      {
        tft.setTextColor(GC9A01A_GREEN);
        x_pos_text = centerX(SIZE_SMALL, market_open);
        tft.setCursor(x_pos_text, 210);
        tft.print(market_open);
      }
      else
      {
        tft.setTextColor(GC9A01A_RED);
        x_pos_text = centerX(SIZE_SMALL, market_closed);
        tft.setCursor(x_pos_text, 230);
        tft.print(market_closed);
      }
      // END OF IF STATEMENT
    }
    else
    {
      // IF ERROR WITH REQUEST
      tft.fillScreen(GC9A01A_BLACK);
      x_pos_text = centerX(SIZE_SMALL, "REQUEST ERROR");
      tft.setCursor(x_pos_text, 70);
      tft.print("REQUEST ERROR");
    }
    delay(300000); // 5 MINUTES DELAY
  }
  else
  {
    // IF WIFI STATUS IS NOT CONNECTED
    tft.fillScreen(GC9A01A_BLACK);
    x_pos_text = centerX(SIZE_SMALL, "CONNECTION LOST");
    tft.setCursor(x_pos_text, 70);
    tft.print("CONNECTION LOST");
    delay(5000);
  }
}

// FUNCTION FOR CENTERING TEXTS
int centerX(int magnification, String text)
{
  int default_font_width = 6;
  int letter_width = default_font_width * magnification;
  int string_width = text.length() * letter_width;
  int x_position = (GC9A01A_TFTWIDTH - string_width) / 2;

  return x_position;
}
// FUNCTION FOR TURNING RED LED ON
void colorRed()
{
  analogWrite(PIN_RED, 255);
  analogWrite(PIN_GREEN, 0);
  analogWrite(PIN_BLUE, 0);
}
// FUNCTION FOR TURNING GREEN LED ON
void colorGreen()
{
  analogWrite(PIN_RED, 0);
  analogWrite(PIN_GREEN, 255);
  analogWrite(PIN_BLUE, 0);
}