#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <SPI.h>
#include <MFRC522.h>
#include <time.h>

// Wi-Fi credentials
#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "password"

// Firebase credentials
#define API_KEY "API KEY
#define DATABASE_URL "https://url"
#define USER_EMAIL "email.com"
#define USER_PASSWORD "password"

// RFID pins
#define SS_PIN 5
#define RST_PIN 4
#define Buzzer_PIN 14
#define LED_PIN 2

MFRC522 mfrc522(SS_PIN, RST_PIN);

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Cooldown
unsigned long lastReadTime = 0;
String lastTag = "";
const unsigned long tagCooldown = 2000; // 2 seconds

// ---------- Time Windows ----------

struct TimeWindow {
  int hour;
  int minute;
  int duration; // in minutes
};

// Define allowed attendance windows
TimeWindow validWindows[] = {
  {8, 30, 25},
  {9, 30, 15},   
  {11, 0, 15},
  {12, 0, 30},
  {13, 0, 15},
  {14, 30, 15},   
  {15, 20, 15}  
};

// ---------- Helper Functions ----------

String getUIDString() {
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  uid.toLowerCase();
  return uid;
}

String getTimeString() {
  time_t now;
  struct tm timeinfo;
  time(&now);
  localtime_r(&now, &timeinfo);

  char buf[30];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(buf);
}

String getDateString() {
  time_t now;
  struct tm timeinfo;
  time(&now);
  localtime_r(&now, &timeinfo);

  char dateBuf[11];
  strftime(dateBuf, sizeof(dateBuf), "%Y-%m-%d", &timeinfo);
  return String(dateBuf);
}

bool isWithinAnyAttendanceWindow() {
  time_t now;
  struct tm timeinfo;
  time(&now);
  localtime_r(&now, &timeinfo);

  int currentMinutes = timeinfo.tm_hour * 60 + timeinfo.tm_min;

  for (TimeWindow window : validWindows) {
    int startMinutes = window.hour * 60 + window.minute;
    if (currentMinutes >= startMinutes && currentMinutes <= (startMinutes + window.duration)) {
      return true;
    }
  }

  return false;
}

// UID → Student Name mapping
String getStudentName(String uid) {
  if (uid == "33dff128") return "Alice Johnson";
  if (uid == "a33dd513") return "Bob Smith";
  if (uid == "1122334455") return "Charlie Lee";
  return "Unknown";
}

// Send data to Firebase
void sendToFirebase(String rfidTag) {
  String timestamp = getTimeString();
  String name = getStudentName(rfidTag);
  String dateStr = getDateString();
  String path = "/attendance/" + rfidTag + "/" + dateStr;

  if (!isWithinAnyAttendanceWindow()) {
    Serial.println("⛔ You are late! Attendance not allowed outside valid time slots.");
    return;
  }

  // Check if attendance already exists for today
  if (Firebase.RTDB.getString(&fbdo, path + "/timestamp")) {
    String existingTimestamp = fbdo.stringData();
    if (existingTimestamp != "") {
      Serial.println("⚠️ Already punched in today at " + existingTimestamp);
      return;
    }
  }

  // Create and send attendance record
  FirebaseJson json;
  json.set("rfid", rfidTag);
  json.set("name", name);
  json.set("timestamp", timestamp);

  if (Firebase.RTDB.setJSON(&fbdo, path, &json)) {
    Serial.println("✅ Sent to Firebase:");
    Serial.println("Name: " + name);
    Serial.println("Time: " + timestamp);
  } else {
    Serial.println("❌ Firebase Error: " + fbdo.errorReason());
  }
}

// ---------- Setup ----------

void setup() {
  Serial.begin(115200);
  pinMode(Buzzer_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println(" connected!");

  // Firebase setup
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Sync time from NTP
  configTime(19800, 0, "pool.ntp.org", "time.nist.gov");

  // RFID setup
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Ready to scan RFID tags.");
}

// ---------- Main Loop ----------

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  String rfidTag = getUIDString();

  if (rfidTag == lastTag && (millis() - lastReadTime) < tagCooldown) return;
  lastTag = rfidTag;
  lastReadTime = millis();

  // Indicate scan
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(Buzzer_PIN, HIGH);
  delay(300);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(Buzzer_PIN, LOW);

  Serial.println("Scanned: " + rfidTag);
  sendToFirebase(rfidTag);
}
