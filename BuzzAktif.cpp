//ini pake buzz aktif, jadi suaranya itu kecil banget,
//deketin ke kuping baru kedengeran,
//kenapa? karna kalo mau ngatur colume buzzer aktif perlu  LEDC (LED Control) yang tugasnya generate sinyal PWM
//sedangkan servo juga perlu, jadi kalo maksa buzzer aktif pake LEDC, bentrok ama servo,
//servo ga bisa jalan, dan sinya yg dikirim ultrasonik malah kebaca di buzzer.

#define BLYNK_TEMPLATE_ID "TMPL6qNLyA5xS"
#define BLYNK_TEMPLATE_NAME "UAP"
#define BLYNK_AUTH_TOKEN "VEqfVnpwF8SXLjzxLELup7wl8fF8fVxy"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <ESP32Servo.h>

char ssid[] = "sytho";
char pass[] = "12345678";

#define DHTPIN 4
#define DHTTYPE DHT11
#define BUZZER_PIN 12
#define SERVO_PIN 13
#define TRIG_PIN 5
#define ECHO_PIN 18

DHT dht(DHTPIN, DHTTYPE);
Servo feederServo;
BlynkTimer timer;
bool sudahIsi = false;

void buzzerOn() {
  digitalWrite(BUZZER_PIN, HIGH);
}

void buzzerOff() {
  digitalWrite(BUZZER_PIN, LOW);
}

void kirimData()
{
  float suhu = dht.readTemperature();
  float hum = dht.readHumidity();

  if (!isnan(suhu) && !isnan(hum))
  {
    Blynk.virtualWrite(V0, suhu);
    Blynk.virtualWrite(V1, hum);

    if (suhu > 32)
      buzzerOn();
    else
      buzzerOff();
  }

  // Ultrasonik
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  float jarak = duration * 0.034 / 2;
  Blynk.virtualWrite(V2, jarak);

  if (jarak > 15 && !sudahIsi)
  {
    feederServo.write(90);
    delay(2000);
    feederServo.write(0);
    sudahIsi = true;
  }

  if (jarak <= 15)
    sudahIsi = false;
}

void setup()
{
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  dht.begin();

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  feederServo.attach(SERVO_PIN);
  feederServo.write(0);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(3000L, kirimData);
}

void loop()
{
  Blynk.run();
  timer.run();
}