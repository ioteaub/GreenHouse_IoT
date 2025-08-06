# 📡 SMART GREEN HOUSE IoT UB

Proyek ini adalah sistem monitoring dan kontrol berbasis IoT menggunakan ESP32 yang terhubung ke **Firebase Realtime Database**. Sistem ini dapat membaca suhu, kelembaban tanah, mengontrol pompa air dan RGB LED melalui Firebase, serta menggunakan **WiFiManager** untuk koneksi WiFi yang mudah.

---

## 🔧 Fitur

- 🔥 Membaca suhu menggunakan sensor **DS18B20**
- 💧 Membaca kelembaban tanah menggunakan sensor analog
- 🌈 Mengontrol RGB LED melalui Firebase
- 🚰 Mengontrol pompa air (relay) melalui Firebase
- 📶 Koneksi WiFi otomatis menggunakan **WiFiManager**
- ☁️ Sinkronisasi data dan kontrol ke/dari **Firebase RTDB**

---

## 🧰 Hardware yang Dibutuhkan

- ESP32 Dev Board
- Sensor Suhu **DS18B20**
- Sensor Kelembaban Tanah (analog)
- Relay Module (untuk kontrol pompa)
- RGB LED (merah, hijau, biru)
- LED indikator WiFi
- Breadboard dan kabel jumper

---

## 📦 Library yang Digunakan

Pastikan Anda menginstal library berikut di Arduino IDE:

- `WiFiManager` - untuk konfigurasi WiFi
- `Firebase ESP Client` - untuk koneksi ke Firebase
- `OneWire` dan `DallasTemperature` - untuk pembacaan sensor DS18B20

---

## 📝 Konfigurasi Awal

### Firebase
1. Buat proyek Firebase di [https://console.firebase.google.com](https://console.firebase.google.com)
2. Aktifkan **Realtime Database**.
3. Ambil **API Key** dan **Database URL**.
4. Ganti pada bagian berikut di kode:

```cpp
#define API_KEY "YOUR_FIREBASE_API_KEY"
#define DATABASE_URL "YOUR_FIREBASE_DATABASE_URL"
