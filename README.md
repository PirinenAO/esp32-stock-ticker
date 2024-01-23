# ESP32 stock ticker

This ESP32 program utilizes the [Twelvedata.com](https://twelvedata.com/) API to request real-time data for a specified stock.<br>
The retrieved information is displayed on a GC9A01A LCD display and is updated every 2 minutes.<br>
The program also integrates an RGB LED module to visually indicate whether the stock's change is positive or negative.

## DEPENDENCIES
- Arduino framework (including WiFi.h and HTTPClient.h)
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
- [Adafruit_GC9A01A](https://github.com/adafruit/Adafruit_GC9A01A) (requires Adafruit GFX library)
- [TimeLib](https://github.com/PaulStoffregen/Time)

## SETUP
**Wi-Fi credentials**
- Open wifi_credentials.h headerfile.
- Add your wifi credentials (SSID and password) to the file.
  
**Twelvedata API key**
- Create an account on [Twelvedata.com](https://twelvedata.com/) and obtain your API key.
- Copy & paste your key to the key variable in main.cpp.
  
**Select stock**
- Add your desired stock symbol to stock variable in main.cpp.

*Notice that free twelvedata user has limited access to the stock markets*

## PINOUT
| ESP32 GPIO | GC9A01A | RGB MODULE |  
| ---------- | ------- | ---------- |
| P4         | RST     |            |
| P22        | CS      |            | 
| P16        | DC      |            | 
| P23        | SDA     |            | 
| P18        | SCL     |            |
| GND        | GND     | GND        |
| 3.3V       | VCC     |            | 
| P26        |         | R          | 
| P27        |         | G          |
| P25        |         | B          | 

<img src="https://github.com/PirinenAO/esp32-stock-ticker/assets/119351375/bf5ae02b-1504-4f65-9193-a72d62b98c17" alt="Start" width="400" height="400">
