#include <math.h>
#include <Bluepad32.h>

GamepadPtr myGamepad;

// ===== LED CONFIG =====
const int STATUS_LED_PIN = 2;
const unsigned long BLINK_INTERVAL = 500;
unsigned long lastBlinkTime = 0;
bool ledState = false;

// ===== MOTOR CONFIG (L298N DUAL MOTOR) =====
// LEFT MOTOR
const int ENA = 14;
const int IN1 = 26;
const int IN2 = 27;

// RIGHT MOTOR
const int ENB = 25;
const int IN3 = 33;
const int IN4 = 32;

// ===== GAMEPAD CALLBACKS =====
void onConnectedGamepad(GamepadPtr gp) {
  myGamepad = gp;
  Serial.println("Gamepad connected");
}

void onDisconnectedGamepad(GamepadPtr gp) {
  myGamepad = nullptr;
  Serial.println("Gamepad disconnected");
}

// ===== MOTOR CONTROL FUNCTIONS =====
void leftMotorStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
}

void rightMotorStop() {
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 0);
}

void leftMotorForward(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, speed);
}

void leftMotorBackward(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, speed);
}

void rightMotorForward(int speed) {
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, speed);
}

void rightMotorBackward(int speed) {
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, speed);
}

void allStop() {
  leftMotorStop();
  rightMotorStop();
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  // Motor pins
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  allStop();

  Serial.println("Starting Bluepad32...");
  BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);
  Serial.println("Waiting for controller...");
}

// ===== LOOP =====
void loop() {
  bool newData = BP32.update();
  if (!newData) return;

  // LED status
  if (myGamepad && myGamepad->isConnected()) {
    digitalWrite(STATUS_LED_PIN, HIGH);
  } else {
    unsigned long now = millis();
    if (now - lastBlinkTime >= BLINK_INTERVAL) {
      lastBlinkTime = now;
      ledState = !ledState;
      digitalWrite(STATUS_LED_PIN, ledState);
    }
  }

  // ===== GAMEPAD DATA =====
  if (myGamepad && myGamepad->isConnected()) {

    float x = myGamepad->axisX();
    float y = myGamepad->axisY();

    // Deadzone
    if (fabs(x) < 10) x = 0;
    if (fabs(y) < 10) y = 0;

    // Convert to degrees
    double degrees = atan2(y, x) * 180.0 / PI;
    degrees += 180;

    int speed = 200; // PWM 0–255

    // ===== DIRECTION LOGIC =====
    if ((degrees >= 316 && degrees <= 360) || (degrees >= 0 && degrees < 44)) {
      Serial.println("Direction: Up");
      leftMotorForward(speed);
      rightMotorForward(speed);

    } else if (degrees >= 45 && degrees <= 135) {
      Serial.println("Direction: Right");
      leftMotorForward(speed);
      rightMotorBackward(speed);

    } else if (degrees >= 136 && degrees < 224) {
      Serial.println("Direction: Down");
      leftMotorBackward(speed);
      rightMotorBackward(speed);

    } else if (degrees >= 225 && degrees < 315) {
      Serial.println("Direction: Left");
      leftMotorBackward(speed);
      rightMotorForward(speed);

    } else {
      Serial.println("Direction: None");
      allStop();
    }
  }
}
