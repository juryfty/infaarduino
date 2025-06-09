#include <Arduino.h>
#include "ESP8266WiFi.h"
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
unsigned long lastInteraction = 0;

// OLED ustawienia
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Bitmapa ikony słońca 
const unsigned char bitmap_slonce [] PROGMEM = {
  0b00000000, 0b00000000,
  0b00000011, 0b11000000,
  0b00001111, 0b11100000,
  0b00011100, 0b01110000,
  0b00111000, 0b00111000,
  0b01110000, 0b00011100,
  0b01100000, 0b00001100,
  0b01100000, 0b00001100,
  0b01110000, 0b00011100,
  0b00111000, 0b00111000,
  0b00011100, 0b01110000,
  0b00001111, 0b11100000,
  0b00000011, 0b11000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000
  };

const char* ssid = "A5"; // Wi-Fi SSID
const char* password = "Alamakota2miwi00"; // Wi-Fi hasło
const char* apiKey = "ed5bde3e37aa34d35ac3cef08bf06714";  
const char* city = "Warsaw";

void setup() {
  Serial.begin(74880);
  delay(1000);
  Serial.println("\nŁączenie z WiFi...");

  // Inicjalizacja OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Błąd inicjalizacji OLED"));
    for (;;); // zatrzymaj jeśli błąd
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Laczenie WiFi...");
  display.display();

  // Połączenie z Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nPołączono z WiFi");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Pobranie danych pogodowych
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    String url = "http://api.openweathermap.org/data/2.5/weather?q=" + String(city) +
                 "&appid=" + apiKey + "&units=metric&lang=pl";

    http.begin(client, url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("Odpowiedź JSON:");
      Serial.println(payload);

      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        float temp = doc["main"]["temp"];
        const char* desc = doc["weather"][0]["description"];

        Serial.print("Temperatura: ");
        Serial.print(temp);
        Serial.println(" °C");

        Serial.print("Opis: ");
        Serial.println(desc);

        // Wyświetlanie na OLED
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Pogoda: Warszawa");
        display.print("Temp: ");
        display.print(temp);
        display.println(" C");
        display.println("Opis:");
        display.println(desc);

        // Dodanie bitmapy słońca
        display.drawBitmap(100, 0, bitmap_slonce, 16, 16, SSD1306_WHITE);

        display.display();
      } else {
        Serial.println("Błąd parsowania JSON");
        Serial.println(error.c_str());
      }
    } else {
      Serial.print("Błąd HTTP: ");
      Serial.println(httpCode);
    }

    http.end();
  }
}

void loop() {
  // Można tu dodać odświeżanie co kilka minut
  if (millis() - lastInteraction > 30000) {
  display.clearDisplay();
  display.display();
}

}