#include <Arduino.h>
#include "ESP8266WiFi.h"
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Temperature_LM75_Derived.h>

// OLED ustawienia
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//LM75A
Generic_LM75 temperature;

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

const char* ssid = "XD"; // Wi-Fi SSID
const char* password = "xxx"; // Wi-Fi hasło
const char* apiKey = "xxx";  
const char* city = "Warsaw";

void setup() {
  Serial.begin(74880);
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
  Serial.print("Laczenie WiFi...");
  display.print("Laczenie WiFi...");
  display.display();

  // Połączenie z Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display.print(".");
    display.display();
  }
  
  Serial.println("\nPołączono z WiFi");
  display.println("\nPołączono z WiFi");
  Serial.println("IP: ");
  display.println("IP: ");
  Serial.print(WiFi.localIP());
  display.print(WiFi.localIP());
  display.display();
}

void pobiraniePogody() {
  // Pranie danych pogodowych
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


void loop() {

  if (WiFi.status() == WL_CONNECTED) {
        pobiraniePogody();
  }

  for (int i=0; i<10; i++) {
    float tempLM75 = temperature.readTemperatureC();
    delay(1000);
    Serial.print("Temperature = ");
    Serial.print(tempLM75);
    Serial.println(" C");

    // Tylko dolna linia — np. linia 7 (ekran 8x tekst, 0-7)
    display.fillRect(0, 56, SCREEN_WIDTH, 8, SSD1306_BLACK); // wyczyść tylko ostatnią linię
    display.setCursor(0, 56);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.print("LM75: ");
    display.print(tempLM75);
    display.println(" C");
    display.display();
  }
  

}