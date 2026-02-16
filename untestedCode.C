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
  BP32.update();

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
  }
}
