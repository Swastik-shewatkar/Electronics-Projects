/*
============================================================
Project  : IoT RFID Security Patrol Monitoring System
Author   : Swastik Shewatkar
Board    : ESP32
Module   : MFRC522 RFID
Purpose  : Logs guard patrol data (UID, Location, Time)
           to Google Sheets using Wi-Fi
============================================================
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include "time.h"

// ============================================================
// 🔌 PIN CONFIGURATION
// ============================================================

#define WIFI_LED_PIN 2
#define SCAN_LED_PIN 27

#define SS_PIN  21
#define RST_PIN 22

MFRC522 rfid(SS_PIN, RST_PIN);

// ============================================================
// 📍 DEVICE CONFIGURATION (CHANGE PER DEVICE)
// ============================================================

#define LOCATION_NAME "Checkpoint-2"
#define GUARD_UID     "339F0B2A"
#define GUARD_NAME    "SecurityGuard"

// ============================================================
// 🌐 WIFI CONFIGURATION
// ============================================================

const char* WIFI_SSID     = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// Google Apps Script URL
String SHEET_URL =
"https://script.google.com/macros/s/YOUR_SCRIPT_ID/exec";

// ============================================================
// ⏰ TIME CONFIGURATION (NTP)
// ============================================================

const char* NTP_SERVER = "pool.ntp.org";
const long  GMT_OFFSET_SEC = 19800;   // IST
const int   DAYLIGHT_OFFSET_SEC = 0;

// ============================================================
// ⏱ DEBOUNCE SETTINGS
// ============================================================

const unsigned long SCAN_DELAY = 3000;
unsigned long lastScanTime = 0;

// ============================================================
// 🔧 FUNCTION: URL ENCODE
// ============================================================

String urlEncode(const String &input) {
  String encoded = "";
  
  for (char c : input) {
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      encoded += c;
    } else {
      char buf[4];
      sprintf(buf, "%%%02X", (uint8_t)c);
      encoded += buf;
    }
  }
  
  return encoded;
}

// ============================================================
// 📡 FUNCTION: READ RFID CARD
// ============================================================

bool readRFID(String &uid) {

  if (!rfid.PICC_IsNewCardPresent()) return false;
  if (!rfid.PICC_ReadCardSerial())   return false;

  uid = "";

  for (byte i = 0; i < rfid.uid.size; i++) {
    char buffer[3];
    sprintf(buffer, "%02X", rfid.uid.uidByte[i]);
    uid += buffer;
  }

  uid.toUpperCase();

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  return true;
}

// ============================================================
// ☁️ FUNCTION: SEND DATA TO GOOGLE SHEET
// ============================================================

void sendToGoogleSheet(const String &uid) {

  if (WiFi.status() != WL_CONNECTED) return;

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;

  char dateStr[12];
  char timeStr[12];

  strftime(dateStr, sizeof(dateStr), "%d/%m/%y", &timeinfo);
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);

  // Create request URL
  String url = SHEET_URL +
               "?name="     + urlEncode(GUARD_NAME) +
               "&guard="    + urlEncode(uid) +
               "&location=" + urlEncode(LOCATION_NAME) +
               "&date="     + urlEncode(dateStr) +
               "&time="     + urlEncode(timeStr);

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;

  if (http.begin(client, url)) {
    int httpCode = http.GET();

    Serial.print("HTTP Response: ");
    Serial.println(httpCode);

    http.end();
  }
}

// ============================================================
// 🚀 SETUP FUNCTION
// ============================================================

void setup() {

  Serial.begin(115200);

  pinMode(WIFI_LED_PIN, OUTPUT);
  pinMode(SCAN_LED_PIN, OUTPUT);

  // Initialize RFID
  SPI.begin();
  rfid.PCD_Init();

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
  digitalWrite(WIFI_LED_PIN, HIGH);

  // Initialize Time
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    Serial.println("Syncing time...");
    delay(1000);
  }

  Serial.println("Time Synced");
}

// ============================================================
// 🔁 MAIN LOOP
// ============================================================

void loop() {

  String uid;

  if (readRFID(uid)) {

    // LED indication
    digitalWrite(SCAN_LED_PIN, HIGH);
    delay(1000);
    digitalWrite(SCAN_LED_PIN, LOW);

    // Check authorized card
    if (uid != GUARD_UID) {
      Serial.println("Unauthorized Card!");
      return;
    }

    // Prevent duplicate scans
    if (millis() - lastScanTime < SCAN_DELAY) return;
    lastScanTime = millis();

    Serial.println("Valid Scan at " LOCATION_NAME);

    // Send data to cloud
    sendToGoogleSheet(uid);
  }

  delay(100);
}
