#include <Adafruit_LiquidCrystal.h>
#include <Servo.h> // Library untuk mengendalikan servo

// Inisialisasi LCD, ultrasonic sensor, moisture sensor, dan servo
Adafruit_LiquidCrystal lcd_1(0);
Servo myServo; // Servo object
int trigPin = 8, echoPin = 9, moisturePin = A0, motorPin = 10, servoPin = 6;
long duration;
int distance;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(moisturePin, INPUT);
  pinMode(motorPin, OUTPUT);
  
  lcd_1.begin(16, 2);              // Inisialisasi LCD 16x2
  lcd_1.clear();
  myServo.attach(servoPin); // Hubungkan servo ke pin
  myServo.write(0); // Mulai dengan servo tertutup (sudut 0 derajat)

  lcd_1.setCursor(0, 0);
  lcd_1.print("IE-POMA");
  delay (2000);
}

void loop() {
  // Ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  // Sensor kelembapan tanah (menghitung persentase)
  int soilMoistureValue = analogRead(moisturePin);
  int soilMoisturePercent = map(soilMoistureValue, 1023, 0, 100, 0); // Konversi ke persen

  // Tampilkan data di LCD
  lcd_1.setCursor(0, 0);
  lcd_1.print("Moisture: ");
  lcd_1.print(soilMoisturePercent);
  lcd_1.print("%");
  
  lcd_1.setCursor(0, 1);
  lcd_1.print("Water level: ");
  lcd_1.print(distance);

  // Atur motor DC berdasarkan persentase kelembapan
  if (soilMoisturePercent < 60) {
    digitalWrite(motorPin, HIGH); // Motor menyala jika kelembapan < 60%
  } else {
    digitalWrite(motorPin, LOW);  // Motor mati jika kelembapan >= 60%
  }

  // Kontrol servo berdasarkan jarak ultrasonik
  if (distance <= 20) {
    myServo.write(90); // Servo terbuka pada sudut 90 derajat jika jarak <= 20 cm
  } else {
    myServo.write(0);  // Servo kembali tertutup jika jarak > 20 cm
  }

  delay(1000);
}
