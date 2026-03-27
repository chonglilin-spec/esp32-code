#include <Bluepad32.h>

GamepadPtr myGamepad;

// ===== LED CONFIG =====
// Status LED pin (GPIO 2 = built-in LED on many ESP32 boards)
const int STATUS_LED_PIN = 2;

// Blink timing
const unsigned long BLINK_INTERVAL = 500; // ms
unsigned long lastBlinkTime = 0;
bool ledState = false;

void onConnectedGamepad(GamepadPtr gp) {
  myGamepad = gp;
  Serial.println("Gamepad connected");
}

void onDisconnectedGamepad(GamepadPtr gp) {
  myGamepad = nullptr;
  Serial.println("Gamepad disconnected");
}

void setup() {
  Serial.begin(115200);
  delay(1000); // IMPORTANT

  // Set LED pin
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  Serial.println("Starting Bluepad32...");
  BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);
  Serial.println("Waiting for controller...");
}

void loop() {
  bool newData = BP32.update();
  if (!newData) {
    return;
  }

  // ===== LED STATUS LOGIC =====
  if (myGamepad && myGamepad->isConnected()) {
    // Solid ON when connected
    digitalWrite(STATUS_LED_PIN, HIGH);
  } else {
    // Blink when not connected
    unsigned long now = millis();
    if (now - lastBlinkTime >= BLINK_INTERVAL) {
      lastBlinkTime = now;
      ledState = !ledState;
      digitalWrite(STATUS_LED_PIN, ledState);
    }
  }

  // ===== GAMEPAD DATA =====
  if (myGamepad && myGamepad->isConnected()) {
    Serial.printf("Buttons: %08X\n", myGamepad->buttons());
    delay(300);

    float x = myGamepad->axisX();
    float y = myGamepad->axisY();
    Serial.printf("Axis X: %.2f, Axis Y: %.2f\n", x, y);

    double degrees = atan2(y, x) * 180.0 / PI;
    Serial.printf("Angle: %.2f degrees\n", degrees);

    if (degrees >= 316 && degrees <= 360 || degrees >= 0 && degrees < 44) {
      Serial.println("Direction: Right");
    } else if (degrees >= 45 && degrees =< 135) {
      Serial.println("Direction: Up");
    } else if (degrees >= 136 && degrees < 224) {
      Serial.println("Direction: Left");
    } else if (degrees >= 225 && degrees < 315) {
      Serial.println("Direction: Down");
    }
  }
}
