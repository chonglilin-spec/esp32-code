#include <Bluepad32.h>

GamepadPtr myGamepad;

// ===== LED CONFIG =====
const int STATUS_LED_PIN = 2;

// Blink timing
const unsigned long BLINK_INTERVAL = 500; // ms
unsigned long lastBlinkTime = 0;
bool ledState = false;

// ===== DEADZONE CONFIG =====
const int DEADZONE = 50;  // adjust (30–100 typical)

// Deadzone function (smooth scaling)
int applyDeadzone(int value, int deadzone) {
  if (abs(value) < deadzone) return 0;

  // Rescale so movement starts smoothly after deadzone
  if (value > 0)
    return map(value, deadzone, 512, 0, 512);
  else
    return map(value, -deadzone, -512, 0, -512);
}

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
    digitalWrite(STATUS_LED_PIN, HIGH);
  } else {
    unsigned long now = millis();
    if (now - lastBlinkTime >= BLINK_INTERVAL) {
      lastBlinkTime = now;
      ledState = !ledState;
      digitalWrite(STATUS_LED_PIN, ledState);
    }
  }

  // ===== GAMEPAD DATA WITH DEADZONE =====
  if (myGamepad && myGamepad->isConnected()) {

    int lx = applyDeadzone(myGamepad->axisX(), DEADZONE);
    int ly = applyDeadzone(myGamepad->axisY(), DEADZONE);
    int rx = applyDeadzone(myGamepad->axisRX(), DEADZONE);
    int ry = applyDeadzone(myGamepad->axisRY(), DEADZONE);

    Serial.printf("LX: %d  LY: %d  RX: %d  RY: %d\n", lx, ly, rx, ry);
    Serial.printf("Buttons: %08X\n", myGamepad->buttons());

    delay(300);
  }
}
