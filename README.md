# 📡 RFID-Based Smart Attendance System (ESP32 + Firebase)

An RFID-based Smart Attendance System built using ESP32 and RC522 RFID reader with real-time cloud integration using Firebase Realtime Database and a responsive web dashboard.

This project automates student attendance tracking using contactless RFID authentication and cloud-based storage.

---

## 🚀 Key Features

* 📛 RFID-based student authentication (RC522)
* 📡 ESP32 Wi-Fi cloud integration
* ☁️ Real-time Firebase database logging
* 🔐 Firebase Authentication for secure access
* 🔔 LED + Buzzer feedback system
* 🌐 Real-time web dashboard
* 📊 Search, filter (Today / This Week), export (CSV/PDF)
* 📈 Scalable cloud-based architecture

---

## 🛠 Hardware Used

* ESP32 Microcontroller
* RC522 RFID Reader
* RFID Cards/Tags
* Buzzer
* LED Indicator
* 5V Power Supply

---

## 💻 Software & Technologies

* Arduino IDE
* Embedded C (ESP32 Programming)
* Firebase Realtime Database
* Firebase Authentication
* HTML
* CSS
* JavaScript
* Firebase SDK

---

## ⚙️ System Workflow

1. Student scans RFID card.
2. RC522 reads unique UID.
3. ESP32 validates and logs UID.
4. Attendance data (Name, UID, Timestamp) pushed to Firebase.
5. Web dashboard updates in real time.
6. LED + buzzer confirm scan result.

---

## 🌐 Web Dashboard Features

* Real-time attendance table
* Student name search
* Date-based filtering (Today / This Week)
* CSV / PDF export
* Secure admin login (Firebase Authentication)

---

## 📂 Project Structure

```
arduino/
  student/
    code.ino

web-dashboard/
  Webdashboard.html
```

---

## 🎯 Project Impact

* Eliminates manual attendance errors
* Prevents proxy attendance
* Provides centralized cloud storage
* Reduces administrative workload
* Scalable for large institutions

---

## 📌 Future Enhancements

* Offline mode support
* Biometric integration
* Advanced analytics dashboard
* Multi-campus deployment

---
