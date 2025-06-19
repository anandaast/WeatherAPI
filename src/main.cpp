#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* city = "Malang";
const char* apiKey = "318561f6c2e84dac4ca956ec8786238a";

// URL API cuaca
String server = "https://api.openweathermap.org/data/2.5/weather?q=" + String(city) + "&units=metric&appid=" + String(apiKey);

LiquidCrystal_I2C lcd(0x27, 16, 2);

float temp = 0.0;
int humidity = 0;
String description = "";

unsigned long lastDataFetch = 0;
unsigned long lastDisplaySwitch = 0;
bool showTemp = true;

void loop() {
  unsigned long currentTime = millis();

  // Ambil data setiap 60 detik
  if (currentTime - lastDataFetch > 60000 || lastDataFetch == 0) {
    lastDataFetch = currentTime;
    Serial.println("Fetching weather data...");

    if (WiFi.status() == WL_CONNECTED) {
      WiFiClientSecure client;
      client.setInsecure();

      HTTPClient http;
      http.setTimeout(5000); // Timeout biar nggak ngegantung
      http.begin(client, server);
      int httpCode = http.GET();

      if (httpCode > 0) {
        String payload = http.getString();
        Serial.println(payload);  // Cek isi JSON

        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
          temp = doc["main"]["temp"];
          humidity = doc["main"]["humidity"];
          description = doc["weather"][0]["description"].as<String>();

          Serial.print("Temp: "); Serial.println(temp);
          Serial.print("Humidity: "); Serial.println(humidity);
          Serial.print("Desc: "); Serial.println(description);

          // TAMPILKAN LANGSUNG KE LCD
          lcd.clear();

          // Baris 1: Temp & Hum
          lcd.setCursor(0, 0);
          lcd.print("T:");
          lcd.print(temp, 1); // 1 angka di belakang koma
          lcd.print("C ");
          lcd.print("H:");
          lcd.print(humidity);
          lcd.print("%");

          // Baris 2: Weather Description (potong kalau >16)
          lcd.setCursor(0, 1);
          if (description.length() > 16) {
            lcd.print(description.substring(0, 16));
          } else {
            String paddedDesc = description;
            while (paddedDesc.length() < 16) paddedDesc += ' ';
            lcd.print(paddedDesc);
          }

        } else {
          Serial.println("JSON parsing failed");
        }
      } else {
        Serial.println("HTTP request failed");
      }

      http.end();
    } else {
      Serial.println("WiFi disconnected");
    }
  }
}



