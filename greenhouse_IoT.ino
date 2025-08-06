#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <Firebase_ESP_Client.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

// Pin Configuration
const int oneWireBus = 4;
const int soilMoisturePin = 32;
const int relay = 15;
const int ledHijau = 5;
const int ledBiru = 18;
const int ledMerah = 19;
const int wifiled = 23;

// PWM Configuration
const int freq = 5000;
const int ledChannelRed = 0;
const int ledChannelGreen = 1;
const int ledChannelBlue = 2;
const int resolution = 8;

OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

// Firebase Configuration
#define API_KEY "YOUR_FIREBASE_API_KEY"
#define DATABASE_URL "YOUR_FIREBASE_DATABASE_URL"

// Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;

unsigned long sendDataPrevMillis = 0;
WiFiManager wifiManager;

void setupPWM() {
  ledcSetup(ledChannelRed, freq, resolution);
  ledcSetup(ledChannelGreen, freq, resolution);
  ledcSetup(ledChannelBlue, freq, resolution);
  
  ledcAttachPin(ledMerah, ledChannelRed);
  ledcAttachPin(ledHijau, ledChannelGreen);
  ledcAttachPin(ledBiru, ledChannelBlue);
}

void setup() {
  Serial.begin(115200);

  // Initialize GPIO
  pinMode(wifiled, OUTPUT);
  digitalWrite(wifiled, LOW); // Initially off
  pinMode(relay, OUTPUT);
  pinMode(soilMoisturePin, INPUT);
  
  // Initialize PWM for RGB LED
  setupPWM();

  // Reset WiFi settings setiap boot
  WiFi.disconnect(true);  // Hapus WiFi settings sebelumnya
  delay(1000);

  // WiFiManager Configuration
  wifiManager.setDebugOutput(false);
  wifiManager.setTimeout(180); // Timeout 3 menit
  wifiManager.setAPCallback([](WiFiManager *wm) {
    Serial.println("Masuk mode AP");
    digitalWrite(wifiled, !digitalRead(wifiled)); // Blink LED saat mode AP
  });

  // Set AP credentials (placeholder)
  const char* ap_ssid = "YOUR_AP_SSID";
  const char* ap_password = "YOUR_AP_PASSWORD";

  // Start WiFiManager dengan SSID dan password khusus
  Serial.println("Memulai WiFiManager...");
  if (!wifiManager.startConfigPortal(ap_ssid, ap_password)) {
    Serial.println("Gagal terhubung dan timeout");
    delay(3000);
    ESP.restart();
  }

  // WiFi connected
  digitalWrite(wifiled, HIGH); // LED menyala saat terhubung
  Serial.println();
  Serial.print("Terhubung ke: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase signUp OK");
    signupOK = true;
  } else {
    Serial.printf("Firebase signUp failed: %s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  sensors.begin();
}

void loop() {
  // Handle WiFi connection status
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(wifiled, LOW); // LED mati jika terputus
    if (WiFi.status() == WL_CONNECTED) {
      digitalWrite(wifiled, HIGH); // LED nyala saat terkoneksi
    }
  }

  if (Firebase.ready() && signupOK) {
    unsigned long currentMillis = millis();
    
    // Update sensor data setiap 1 detik
    if (currentMillis - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0) {
      sendDataPrevMillis = currentMillis;

      // Baca sensor
      sensors.requestTemperatures();
      float temperatureC = sensors.getTempCByIndex(0);
      int soilMoistureValue = analogRead(soilMoisturePin);

      // Kirim data ke Firebase
      if (!Firebase.RTDB.setFloat(&fbdo, "lustrum/temp", temperatureC)) {
        Serial.println("Gagal mengirim suhu: " + fbdo.errorReason());
      }
      
      if (!Firebase.RTDB.setInt(&fbdo, "lustrum/moisture", soilMoistureValue)) {
        Serial.println("Gagal mengirim kelembaban: " + fbdo.errorReason());
      }

      // Kontrol RGB LED
      if (Firebase.RTDB.getString(&fbdo, "lustrum/red")) {
        String redStr = fbdo.to<String>();
        int v_red = redStr.toInt();
        ledcWrite(ledChannelRed, v_red);
      }
      
      if (Firebase.RTDB.getString(&fbdo, "lustrum/green")) {
        String greenStr = fbdo.to<String>();
        int v_green = greenStr.toInt();
        ledcWrite(ledChannelGreen, v_green);
      }
      
      if (Firebase.RTDB.getString(&fbdo, "lustrum/blue")) {
        String blueStr = fbdo.to<String>();
        int v_blue = blueStr.toInt();
        ledcWrite(ledChannelBlue, v_blue);
      }

      // Kontrol Pompa (1 = ON, 0 = OFF)
      if (Firebase.RTDB.getString(&fbdo, "lustrum/pump")) {
        String pumpStr = fbdo.to<String>();
        int pumpStatus = pumpStr.toInt();
        digitalWrite(relay, pumpStatus == 1 ? HIGH : LOW);
      }
    }
  }
  
  delay(100);
}
