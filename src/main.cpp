#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GC9A01A.h>
#include <TimeLib.h>

// SET YOUR WIFI CREDENTIALS HERE
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

// INITIALIZING DISPLAY
Adafruit_GC9A01A tft(TFT_CS, TFT_DC);

// API SETUP
String key = "YOUR API KEY"; // SET YOUR API KEY INSIDE QUOTES
String stock = "INTC";       // SET THE STOCK SYMBOL INSIDE QUOTES
String url = "https://api.twelvedata.com/quote?symbol=" + stock + "&apikey=" + key;

// FUNCTIONS
void ledRed();
void ledGreen();
int centerX(int text_size, String text);
void drawText(int text_size, String text, int y_position, int color);
void connectWifi();

int x_pos_text;

void setup()
{
  Serial.begin(115200);

  // FOR RGB MODULE
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  tft.begin();
  tft.setRotation(4);
  tft.fillScreen(GC9A01A_BLACK);

  connectWifi();
}

void loop()
{
  if ((WiFi.status() == WL_CONNECTED))
  {

    HTTPClient client;
    client.begin(url);
    int httpStatusCode = client.GET();
    Serial.println(httpStatusCode);

    if (httpStatusCode == 200)
    {
      // RECEIVING JSON
      String payload = client.getString();

      // PARSE AND DESERIALIZE JSON
      const size_t capacity = JSON_OBJECT_SIZE(1) + 1000;
      DynamicJsonDocument doc(capacity);
      deserializeJson(doc, payload);

      // EXTRACTING CONTENT
      String symbol = doc["symbol"];
      String name = doc["name"];
      float change_float = doc["change"];
      float open_float = doc["open"];
      int time = doc["timestamp"];
      bool market_status = doc["is_market_open"];

      // MODIFYING AND MAKING CONTENT PRINTABLE
      time += CURRENT_TIMEZONE;
      String open = String(open_float);
      String change = String(change_float);
      String market_open = "OPEN";
      String market_closed = "CLOSED";
      open += "$";
      change += "%";

      // TURNING UNIX TIMESTAMP TO READABLE DATA
      tmElements_t timeinfo;
      char buffer[20];
      breakTime(time, timeinfo);

      sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
              timeinfo.Year + 1970, timeinfo.Month, timeinfo.Day,
              timeinfo.Hour, timeinfo.Minute, timeinfo.Second);

      String timestamp = String(buffer);

      // PRINTING TO DISPLAY
      tft.fillScreen(GC9A01A_BLACK);
      drawText(SIZE_BIG, symbol, 50, GC9A01A_WHITE);
      drawText(SIZE_BIG, open, 100, GC9A01A_WHITE);

      if (change_float >= 0)
      {
        drawText(SIZE_MEDIUM, change, 150, GC9A01A_GREEN);
        ledGreen();
      }
      else
      {
        drawText(SIZE_MEDIUM, change, 150, GC9A01A_RED);
        ledRed();
      }

      drawText(SIZE_SMALL, name, 190, GC9A01A_WHITE);
      drawText(SIZE_SMALL, timestamp, 200, GC9A01A_WHITE);

      if (market_status)
      {
        drawText(SIZE_SMALL, market_open, 210, GC9A01A_GREEN);
      }
      else
      {
        drawText(SIZE_SMALL, market_closed, 210, GC9A01A_RED);
      }
    }
    else
    {
      // IF ERROR WITH HTTP REQUEST
      tft.fillScreen(GC9A01A_BLACK);
      drawText(SIZE_MEDIUM, "REQUEST ERROR", 100, GC9A01A_WHITE);
      drawText(SIZE_SMALL, "HTTP STATUS CODE:", 140, GC9A01A_WHITE);
      drawText(SIZE_SMALL, String(httpStatusCode), 150, GC9A01A_WHITE);
    }
    delay(120000); // 2 MINUTES DELAY
  }
  else if (WiFi.status() == WL_CONNECTION_LOST)
  {
    tft.fillScreen(GC9A01A_BLACK);
    drawText(SIZE_MEDIUM, "CONNECTION", 70, GC9A01A_WHITE);
    drawText(SIZE_MEDIUM, "LOST", 100, GC9A01A_WHITE);
    delay(5000);
    connectWifi();
  }
}

// FUNCTION FOR CENTERING TEXTS ON X AXIS
int centerX(int magnification, String text)
{
  int default_font_width = 6;
  int symbol_width = default_font_width * magnification;
  int string_width = text.length() * symbol_width;
  int x_position = (GC9A01A_TFTWIDTH - string_width) / 2;

  return x_position;
}

// FUNCTION FOR TEXT DRAWING
void drawText(int text_size, String text, int y_position, int color)
{
  tft.setTextColor(color);
  tft.setTextSize(text_size);
  x_pos_text = centerX(text_size, text);
  tft.setCursor(x_pos_text, y_position);
  tft.print(text);
}

// FUNCTION FOR ESTABLISHING WIFI CONNECTION
void connectWifi()
{
  WiFi.begin(ssid, passwd);
  int loading_bar = 40;

  while (WiFi.status() != WL_CONNECTED)
  {
    drawText(SIZE_MEDIUM, "Connecting", 70, GC9A01A_WHITE);
    drawText(SIZE_MEDIUM, "to WIFI", 100, GC9A01A_WHITE);

    tft.setCursor(loading_bar, 140);
    tft.print(".");
    loading_bar += 10;

    // RESET LOADING BAR
    if (loading_bar == 180)
    {
      loading_bar = 40;
      tft.fillScreen(GC9A01A_BLACK);
      drawText(SIZE_MEDIUM, "Connecting", 70, GC9A01A_WHITE);
      drawText(SIZE_MEDIUM, "to WIFI", 100, GC9A01A_WHITE);
    }
    delay(500);
  }

  tft.fillScreen(GC9A01A_BLACK);
  drawText(SIZE_MEDIUM, "CONNECTED", 100, GC9A01A_WHITE);
}

// FUNCTION FOR TURNING RED LED ON
void ledRed()
{
  analogWrite(PIN_RED, 255);
  analogWrite(PIN_GREEN, 0);
  analogWrite(PIN_BLUE, 0);
}

// FUNCTION FOR TURNING GREEN LED ON
void ledGreen()
{
  analogWrite(PIN_RED, 0);
  analogWrite(PIN_GREEN, 255);
  analogWrite(PIN_BLUE, 0);
}