#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// Inisialisasi LCD (alamat I2C default 0x27, ukuran 16x2)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin untuk sensor ultrasonik
const int trigPin = 9;
const int echoPin = 10;

// Pin untuk sensor kelembapan tanah
const int soilSensorPin = A0;

// Pin untuk relay solenoid valve dan pompa air
const int solenoidRelayPin = 2;
const int pumpRelayPin = 3;

// Variabel untuk menyimpan nilai sensor
float waterLevel = 0.0; // Tinggi air dalam persen
int soilMoisture = 0;   // Nilai kelembapan tanah dalam persen

// Konstanta untuk logika sistem
const int minSoilMoisture = 30; // Batas kelembapan tanah minimum dalam persen
const float maxWaterHeight = 15.2; // Tinggi maksimum tangki dalam cm
const float minWaterHeight = 3.0;  // Tinggi minimum untuk air

void setup() {
  // Inisialisasi pin
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(solenoidRelayPin, OUTPUT);
  pinMode(pumpRelayPin, OUTPUT);

  // Matikan semua relay di awal
  digitalWrite(solenoidRelayPin, HIGH);
  digitalWrite(pumpRelayPin, HIGH);

  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Starting");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Baca ketinggian air
  waterLevel = readWaterLevel();

  // Baca kelembapan tanah
  soilMoisture = readSoilMoisture();

  // Logika kontrol solenoid valve
  if (waterLevel == 100) { 
    digitalWrite(solenoidRelayPin, LOW); // Buka solenoid valve jika water level 100%
  } else {
    digitalWrite(solenoidRelayPin, HIGH); // Tutup solenoid valve jika water level < 100%
  }

  // Logika kontrol pompa air
  if (soilMoisture < minSoilMoisture) {
    digitalWrite(pumpRelayPin, LOW); // Aktifkan pompa air
  } else {
    digitalWrite(pumpRelayPin, HIGH); // Matikan pompa air
  }

  // Update LCD
  updateLCD(waterLevel, soilMoisture);

  delay(500); // Delay untuk pembacaan ulang
}

float readWaterLevel() {
  // Mengirimkan pulsa trig ultrasonic
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Menghitung waktu pantulan echo dengan batas waktu (timeout)
  long duration = pulseIn(echoPin, HIGH, 30000); // Timeout dalam mikrodetik (30 ms)

  // Jika tidak ada pantulan (timeout), kembalikan nilai di luar jangkauan
  if (duration == 0) {
    return 0; // Nilai di luar jangkauan (atau nilai arbitrer)
  }

// Konversi waktu ke jarak (cm)
  float distance = (duration * 0.034) / 2;

  // Konversi jarak ke persentase
  float waterPercentage = 100.0 * (maxWaterHeight - distance) / (maxWaterHeight - minWaterHeight);
  waterPercentage = constrain(waterPercentage, 0, 100); // Pastikan berada di rentang 0-100%

  return waterPercentage; // Kembalikan nilai dalam persen
}

int readSoilMoisture() {
  // Baca nilai analog dari sensor kelembapan tanah
  int sensorValue = analogRead(soilSensorPin);

  // Konversi ke persen (asumsi nilai sensor 0-1023)
  int moisturePercent = map(sensorValue, 1023, 0, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100); // Batasi ke 0-100%

  return moisturePercent;
}

void updateLCD(float waterLevel, int soilMoisture) {
  lcd.clear();

  // Baris pertama: Tampilkan kelembapan tanah
  lcd.setCursor(0, 0);
  lcd.print("Soil: ");
  lcd.print(soilMoisture);
  lcd.print("%");

  // Baris kedua: Tampilkan ketinggian air
  lcd.setCursor(0, 1);
  lcd.print("Water: ");
  lcd.print(waterLevel);
  lcd.print("%");
}
